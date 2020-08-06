#include "sspddata.h"

UnitData::UnitData(QObject *parent)
    : QObject(parent)
{
    initialize();
    connect(this, SIGNAL(newDataSetted(int, double)), SLOT(setData(int, double)));
}

QVector<DataItem> UnitData::items() const
{
    return mItems;
}

void UnitData::setData(int index, double value)
{
    if (index<0 || index >=mItems.size())
        return;
    if (mItems[index].value == value)
        return;

    mItems[index].value = value;
    emit dataChanged(index);
}

void UnitData::setData(int index, bool value)
{
    setData(index, value ? 1.0 : 0);
}

void UnitData::initialize()
{
    mItems.append({"current", "Current (uA)", 0, "changable", 1, "Bias", 0.1});
    mItems.append({"voltage", "Voltage (mV)", 0, "unchangable", 2, "Bias", 0});
    mItems.append({"short", "Shorted", 0, "switch", 0, "Bias", 0});
    mItems.append({"amplifier", "Amplifier", 1, "switch", 0, "Signal", 0});
    mItems.append({"cmp", "Trigger (mV)", 0, "changable", 0, "Signal", 10});
    mItems.append({"cmp_on", "Comparator", 0, "switch", 0, "Signal", 0});
    mItems.append({"counter", "Counts", 0, "unchangable", 0, "Counter", 0});
    mItems.append({"counter_timeOut", "TimeOut (sec)", 0, "changable", 2, "Counter", 0.1});
    mItems.append({"autoreset_on", "Autoreset", 0, "switch", 0, "Autoreset", 0});
    mItems.append({"threshold", "Threshold (V)", 0, "changable", 2, "Autoreset", 0.1});
    mItems.append({"timeOut", "TimeOut (sec)", 0, "changable", 2, "Autoreset", 0.1});
}

int UnitData::getIndexByName(QString name)
{
    for (int i = 0; i < mItems.size(); i++){
        if (mItems[i].name == name)
            return i;
    }
    return -1;
}
