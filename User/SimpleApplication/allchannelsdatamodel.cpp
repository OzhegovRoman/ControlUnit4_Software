#include "allchannelsdatamodel.h"
#include <QBrush>
#include <QDebug>
#include <cmath>
#include "Drivers/drivers.h"

AllChannelsDataModel::AllChannelsDataModel(QObject *parent):
    QAbstractListModel (parent)
{

}

int AllChannelsDataModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return drivers.count();
}

int AllChannelsDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

void AllChannelsDataModel::initialize(const QVector<CommonDriver *>& mDrivers)
{
    for (auto driver: mDrivers){
        drivers.append(driver);
    }
}

QVariant AllChannelsDataModel::data(const QModelIndex &index, int role) const
{
    int idx = index.row();
    {
        auto driver = qobject_cast<SspdDriverM0*>(drivers[idx]);
        if (driver){
            if (role == Qt::DisplayRole)
                return QString("%1 uA").arg(driver->current()->currentValue()*1e6, 6, 'f', 1);
            if (role == Qt::BackgroundRole){
                if (!driver->status()->currentValue().stShorted){
                    if (qAbs(driver->voltage()->currentValue())<0.01)
                        return QBrush(Qt::green);
                    return QBrush(Qt::red);
                }
            }
        }
    }

    {
        auto driver = qobject_cast<SspdDriverM1*>(drivers[idx]);
        if (driver){
            if (role == Qt::DisplayRole)
                return QString("%1 uA").arg(driver->current()->currentValue()*1e6, 6, 'f', 1);
            if (role == Qt::BackgroundRole){
                if (!driver->status()->currentValue().stShorted){
                    if (qAbs(driver->voltage()->currentValue())<0.01)
                        return QBrush(Qt::green);
                    return QBrush(Qt::red);
                }
            }
        }
    }

    {
        auto driver = qobject_cast<TempDriverM0*>(drivers[idx]);
        if (driver){
            if ((std::isnan(driver->temperature()->currentValue())) || (qAbs(driver->temperature()->currentValue()) < 1e-5)){
                if (role == Qt::DisplayRole)
                    return QString("Not connected");
                if (role == Qt::BackgroundRole)
                    return QBrush(Qt::red);
            }
            else
                if (role == Qt::DisplayRole){
                    return QString("%1 K").arg(driver->temperature()->currentValue(), 4, 'f', 2);
                }
        }
    }

    {
        auto driver = qobject_cast<TempDriverM1*>(drivers[idx]);
        if (driver){
            if (role == Qt::DisplayRole){
                QString tmpString;
                for (int i = 0; i < 4; ++i){
                    if (driver->defaultParam(i).enable)
                        tmpString.append(QString("T%1: %2 K; ")
                                         .arg(i)
                                         .arg(driver->currentTemperature(i), 4,'f', 2));
                }
                tmpString.chop(2);
                return tmpString;
            }
        }
    }

    {
        auto driver = qobject_cast<HeaterDriverM0*>(drivers[idx]);
        if (driver){
            if (role == Qt::DisplayRole){
                return QString("No Data");
            }
        }
    }

    {
        auto driver = qobject_cast<SisControlLineDriverM0*>(drivers[idx]);
        if (driver){
            if (role == Qt::DisplayRole){
                return QString("I: %1 mA").arg(driver->current()->currentValue() * 1e3, 4,'f', 2);
            }
        }
    }

    {
        auto driver = qobject_cast<SisBiasSourceDriverM0*>(drivers[idx]);
        if (driver){
            if (role == Qt::DisplayRole){
                return QString("U: %1 mV; I: %2 uA")
                        .arg(driver->voltage()->currentValue() * 1e3, 4,'f', 2)
                        .arg(driver->current()->currentValue() * 1e6, 4,'f', 1)
                        ;
            }
        }
    }
    return QVariant();
}

QVariant AllChannelsDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Vertical){
        if (qobject_cast<SspdDriverM0*>(drivers[section]))
            return QString("SSPD#%1").arg(section);
        if (qobject_cast<SspdDriverM1*>(drivers[section]))
            return QString("SSPD#%1").arg(section);
        if (qobject_cast<TempDriverM0*>(drivers[section]))
            return ("Temperature");
        if (qobject_cast<TempDriverM1*>(drivers[section]))
            return ("Temperature");
        if (qobject_cast<HeaterDriverM0*>(drivers[section]))
            return ("Heater");
        if (qobject_cast<SisControlLineDriverM0*>(drivers[section]))
            return ("SIS Control Line");
        if (qobject_cast<SisBiasSourceDriverM0*>(drivers[section]))
            return ("SIS Bias Source");
    }
    return QAbstractListModel::headerData(section, orientation, role);
}

bool AllChannelsDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return true;
}

Qt::ItemFlags AllChannelsDataModel::flags(const QModelIndex &index) const
{
    if (qobject_cast<SspdDriverM0*>(drivers[index.row()]))
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    if (qobject_cast<SspdDriverM1*>(drivers[index.row()]))
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    if (qobject_cast<SisControlLineDriverM0*>(drivers[index.row()]))
        return Qt::ItemIsSelectable |  Qt::ItemIsEditable | Qt::ItemIsEnabled ;
    return Qt::NoItemFlags;
}
