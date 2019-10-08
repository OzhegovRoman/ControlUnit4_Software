#include "devicemodel.h"
#include "devicelist.h"

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent)
    , mList(nullptr)
{
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !mList)
        return 0;

    return mList->items().size();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !mList)
        return QVariant();

    const DeviceItem item = mList->items().at(index.row());
    switch (role){
    case TypeRole:
        return QVariant(item.m_type);
    case AddressRole:
        return QVariant(item.m_address);
    }

    return QVariant();
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[TypeRole]     = "type";
    names[AddressRole]  = "address";
    return names;
}

DeviceList *DeviceModel::list() const
{
    return mList;
}

void DeviceModel::setList(DeviceList *list)
{
    beginResetModel();

    if (mList)
        mList->disconnect(this);

    mList = list;

    if (mList){
        connect(mList, &DeviceList::preItemAppended, this, [=](){
           const int index = mList->items().size();
           beginInsertRows(QModelIndex(),index, index);
        });
        connect(mList, &DeviceList::postItemAppended, this, [=](){
            endInsertRows();
        });
        connect(mList, &DeviceList::preItemRemoved, this, [=](int index){
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(mList, &DeviceList::postItemRemoved, this, [=](){
            endRemoveRows();
        });
    }

    endResetModel();
}
