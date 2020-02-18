#include "devicelist.h"

DeviceList::DeviceList(QObject *parent) : QObject(parent)
{
}

QVector<DeviceItem> DeviceList::items() const
{
    return mItems;
}

bool DeviceList::setItemAt(int index, const DeviceItem &item)
{
    if (index < 0 || index >= mItems.size())
        return false;

    const DeviceItem &oldItem = mItems.at(index);
    if (item.m_type == oldItem.m_type && item.m_address == oldItem.m_address)
        return false;

    mItems[index] = item;
    return true;
}

void DeviceList::appendItem()
{
    DeviceItem item;
    item.m_address = -1;
    item.m_type = "undefined";
    appendItem(item);
}

void DeviceList::appendItem(const DeviceItem &item)
{
    emit preItemAppended();
    mItems.append(item);
    emit postItemAppended();
}

void DeviceList::removeAll()
{
    while (mItems.size() > 0) {
        emit preItemRemoved(0);
        mItems.removeAt(0);
        emit postItemRemoved();
    }
}
