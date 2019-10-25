#include "settingsprovider.h"
#include <QSettings>
#include "ctcpipserver.h"

SettingsProvider::SettingsProvider(QObject *parent)
    : QObject(parent)
{

}

void SettingsProvider::prepareDeviceInfoList()
{
    cTcpIpServer::consoleWriteDebug("CommandParser: preparing DeviceInfoList");
    QSettings settings("Scontel", "RaspPi Server");
    int size = settings.beginReadArray("devices");
    mDeviceInfoList.clear();
    for (int i = 0; i < size; ++i){
        cDeviceInfo deviceInfo;
        settings.setArrayIndex(i);
        deviceInfo.setAddress(static_cast<quint8>(settings.value("devAddress", 255).toInt()));
        deviceInfo.setType(settings.value("devType","None").toString());
        deviceInfo.setDescription(settings.value("devDescription", "None").toString());
        deviceInfo.setUDID(cUDID(reinterpret_cast<quint8*>(settings.value("devUDID","0").toByteArray().data())));
        deviceInfo.setFirmwareVersion(settings.value("devFwVersion", "None").toString());
        deviceInfo.setHardwareVersion(settings.value("devHwVersion", "None").toString());
        deviceInfo.setModificationVersion(settings.value("devModVersion", "None").toString());
        mDeviceInfoList.append(deviceInfo);
    }
    settings.endArray();
    sortDeviceInfoList();
    cTcpIpServer::consoleWriteDebug(QString("Loaded devices from SETTINGS: %1").arg(mDeviceInfoList.size()));
}

void SettingsProvider::saveDeviceInformation()
{
    // пишем данные в файла настроек
    cTcpIpServer::consoleWriteDebug("Saving Device Information");
    QSettings settings("Scontel", "RaspPi Server");
    settings.beginWriteArray("devices");
    for (int i = 0; i < mDeviceInfoList.count(); ++i){
        settings.setArrayIndex(i);
        settings.setValue("devAddress", mDeviceInfoList[i].address());
        settings.setValue("devType", mDeviceInfoList[i].type());
        settings.setValue("devUDID",QByteArray(reinterpret_cast<const char*>(mDeviceInfoList[i].UDID().UDID()),12));
        settings.setValue("devFwVersion",mDeviceInfoList[i].firmwareVersion());
        settings.setValue("devHwVersion",mDeviceInfoList[i].hardwareVersion());
        settings.setValue("devModVersion",mDeviceInfoList[i].modificationVersion());
        settings.setValue("devDescription",mDeviceInfoList[i].description());
    }
    settings.endArray();
}

int SettingsProvider::deviceCount()
{
    return mDeviceInfoList.size();
}

cDeviceInfo SettingsProvider::at(int index) const
{
    return  mDeviceInfoList.at(index);
}

void SettingsProvider::remove(int index)
{
    mDeviceInfoList.removeAt(index);
}

void SettingsProvider::removeDeviceWithAddress(quint8 address)
{
    for (auto iter = mDeviceInfoList.begin(); iter < mDeviceInfoList.end();){
        if (iter->address() == address)
            iter = mDeviceInfoList.erase(iter);
        else
            iter++;
    }
}

void SettingsProvider::append(const cDeviceInfo &info)
{
    mDeviceInfoList.append(info);
    sortDeviceInfoList();
}

void SettingsProvider::sortDeviceInfoList()
{
    // сортируем по адресу
    std::sort(mDeviceInfoList.begin(), mDeviceInfoList.end(),
              [](cDeviceInfo& a, cDeviceInfo& b)
    {
        return a.address() < b.address();
    });
}
