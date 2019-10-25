#ifndef SETTINGSPROVIDER_H
#define SETTINGSPROVIDER_H

#include <QObject>
#include "cdeviceinfo.h"

class SettingsProvider: public QObject
{
    Q_OBJECT
public:
    explicit SettingsProvider(QObject *parent = nullptr);

    void prepareDeviceInfoList();
    void saveDeviceInformation();

    int deviceCount();
    cDeviceInfo at(int index) const;
    void set(const cDeviceInfo &info, const int index);
    void remove(int index);
    void removeDeviceWithAddress(quint8 address);
    void append(const cDeviceInfo &info);


private:
    QList<cDeviceInfo> mDeviceInfoList;
    void sortDeviceInfoList();
};

#endif // SETTINGSPROVIDER_H
