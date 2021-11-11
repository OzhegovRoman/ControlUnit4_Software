#include "unitdatamodel.h"
#include "unitdata.h"
#include <QDebug>

UnitDataModel::UnitDataModel(QObject *parent)
    : QAbstractListModel(parent)
    , mData(nullptr)
{
}

int UnitDataModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !mData)
        return 0;

    return mData->items()->size();
}

QVariant UnitDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !mData)
        return QVariant();

    const DataItem item = mData->items()->at(index.row());
    switch (role) {
    case NameRole:
        return QVariant(item.name);
    case TitleRole:
        return QVariant(item.title);
    case ValueRole:
        return QVariant(item.value);
    case TypeRole:
        return QVariant(item.type);
    case FixedRole:
        return QVariant(item.fixed);
    case GroupRole:
        return QVariant(item.group);
    case StepRole:
        return QVariant(item.step);
    }

    return QVariant();
}

bool UnitDataModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        if (role == ValueRole)
            emit newDataSetted(index.row(), value.toDouble());
        return true;
    }
    return false;
}

Qt::ItemFlags UnitDataModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

QHash<int, QByteArray> UnitDataModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[NameRole]     = "name";
    names[TitleRole]    = "title";
    names[ValueRole]    = "value";
    names[TypeRole]     = "type";
    names[FixedRole]    = "fixed";
    names[GroupRole]    = "group";
    names[StepRole]     = "step";
    return names;
}

UnitData *UnitDataModel::data() const
{
    return mData;
}

void UnitDataModel::setData(UnitData *data)
{
    if (mData)
        mData->disconnect(this);

    mData = data;
    if (mData){
        connect(mData, &UnitData::dataChanged, this, [=](int index) {
            emit dataChanged(this->index(index), this->index(index), QVector<int>() << ValueRole);
        });
        connect(this, SIGNAL(newDataSetted(int,double)), mData, SIGNAL(newDataSetted(int,double)));
    }
}
