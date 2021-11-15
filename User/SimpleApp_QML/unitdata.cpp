#include "unitdata.h"

UnitData::UnitData(QObject *parent)
    : QObject(parent)
    , mItems (new QVector<DataItem>())
{
    initialize();
    connect(this, SIGNAL(newDataSetted(int, double)), SLOT(setData(int, double)));
}

QVector<DataItem> *UnitData::items()
{
    return mItems;
}

void UnitData::setData(int index, double value)
{
    if (index<0 || index >=mItems->size())
        return;
    if (mItems->at(index).value == value)
        return;

    (*mItems)[index].value = value;
    emit dataChanged(index);
}

void UnitData::setData(int index, bool value)
{
    if ((index >= 0) && (index < mItems->size()))
        setData(index, value ? 1.0 : 0);
}

void UnitData::setData(QString name, double value)
{
    setData(getIndexByName(name), value);
}

void UnitData::setData(QString name, bool value)
{
    setData(getIndexByName(name), value);
}

double UnitData::value(QString name)
{
    return mItems->at(getIndexByName(name)).value;
}

void UnitData::initialize()
{
}

int UnitData::currentAddress() const
{
    return mCurrentAddress;
}

void UnitData::setCurrentAddress(int currentAddress)
{
    mCurrentAddress = currentAddress;
}

QString UnitData::currentType() const
{
    return mCurrentType;
}

void UnitData::setCurrentType(const QString &currentType)
{
    mCurrentType = currentType;
}

int UnitData::getIndexByName(QString name)
{
    for (int i = 0; i < mItems->size(); i++){
        if (mItems->at(i).name == name)
            return i;
    }
    return -1;
}
