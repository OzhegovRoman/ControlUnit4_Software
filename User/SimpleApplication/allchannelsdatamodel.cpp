#include "allchannelsdatamodel.h"
#include <QBrush>
#include <QDebug>
#include <cmath>

AllChannelsDataModel::AllChannelsDataModel(QObject *parent):
    QAbstractListModel (parent)
{

}

int AllChannelsDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return devices.count();
}

int AllChannelsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

void AllChannelsDataModel::initialize(const QString& info)
{
    for (const QString& str : info.split("\r\n")){
        QStringList strL = str.split(": ");
        if (strL.count()>2)
        {
            deviceInfo_t tmpDeviceInfo{};
            tmpDeviceInfo.current = 0;
            tmpDeviceInfo.voltage = 0;
            tmpDeviceInfo.temperature = 0;
            tmpDeviceInfo.isShorted = true;
            for (const QString& x : strL){
                QStringList strL2 = x.split("=");
                if (strL2.count() == 2){
                    if (strL2[0] == "address")
                        tmpDeviceInfo.devAddress = strL2[1].toInt();
                    if (strL2[0] == "type")
                        tmpDeviceInfo.isSspd = strL2[1].contains("CU4SD");
                }
            }
            devices.append(tmpDeviceInfo);
        }
    }
}

QVariant AllChannelsDataModel::data(const QModelIndex &index, int role) const
{
    int idx = index.row();
    if (devices[idx].isSspd){
        if (role == Qt::DisplayRole)
            return QString("%1 uA").arg(devices[idx].current*1e6, 6, 'f', 1);
        if (role == Qt::BackgroundRole){
            if (!devices[idx].isShorted){
                if (qAbs(devices[idx].voltage)<0.01)
                    return QBrush(Qt::green);
                return QBrush(Qt::red);
            }
        }
    }
    else{
        if ((std::isnan(devices[idx].temperature)) || (devices[idx].temperature == 0)){
            if (role == Qt::DisplayRole)
                return QString("Not connected");
            if (role == Qt::BackgroundRole)
                return QBrush(Qt::red);
        }
        else
            if (role == Qt::DisplayRole)
                return QString("%1 K").arg(devices[idx].temperature, 4, 'f', 2);
    }
    return QVariant();
}

QVariant AllChannelsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Vertical){
        if (devices[section].isSspd)
            return QString("SSPD#%1").arg(section);
        return ("Temperature");
    }
    return QAbstractListModel::headerData(section, orientation, role);
}

bool AllChannelsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index);
    Q_UNUSED(value);
    if (role == Qt::EditRole){
        qDebug()<<"setData";
    }
    return true;
}

Qt::ItemFlags AllChannelsDataModel::flags(const QModelIndex &index) const
{
    if (devices[index.row()].isSspd)
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    return Qt::NoItemFlags;
}
