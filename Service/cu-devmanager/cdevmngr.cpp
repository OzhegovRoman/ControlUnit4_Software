#include "cdevmngr.h"
#include <QElapsedTimer>
#include <QRegExp>
#include <QSettings>
#include "Drivers/commondriver.h"
#include "Interfaces/curs485iointerface.h"
#include "Interfaces/cutcpsocketiointerface.h"
#include "../qCustomLib/qCustomLib.h"

cDevMngr::cDevMngr()
    : mPortName(QString("ttyS0"))
    , reader(new ConsoleReader())
{

}


QString cDevMngr::portName() const
{
    return mPortName;
}

void cDevMngr::setPortName(const QString &portName)
{
    mPortName = portName;
}

QString cDevMngr::tcpIpAddress() const
{
    return mTcpIpAddress;
}

void cDevMngr::setTcpIpAddress(const QString &tcpIpAddress)
{
    mTcpIpAddress = tcpIpAddress;
}

bool cDevMngr::isTcpIpProtocol() const
{
    return mTcpIpProtocolEnabled;
}

void cDevMngr::setTcpIpProtocolEnabled(bool value)
{
    mTcpIpProtocolEnabled = value;
}

bool cDevMngr::addDevice(int address)
{
    deleteDevice(address);// сначала удалим? чтобы не было повторов

    if (isTcpIpProtocol()){
        cuTcpSocketIOInterface mInterface;
        mInterface.setAddress(convertToHostAddress(tcpIpAddress()));
        mInterface.setPort(SERVER_TCPIP_PORT);
        auto ad = static_cast<quint8>(address);
        mInterface.sendMsg(SERVER_ADDRESS, CMD_SERVER_ADD_DEVICE, 1, &ad);
        QElapsedTimer mTimer;
        mTimer.start();
        while(mTimer.elapsed()<1000){
            qApp->processEvents();
        }
        return true;
    }

    cuRs485IOInterface mInterface;
    mInterface.setPortName(portName());
    CommonDriver driver;
    driver.setIOInterface(&mInterface);
    driver.setDevAddress(static_cast<quint8>(address));

    deviceInfo newDevice;
    newDevice.devAddress = static_cast<quint8>(address);
    bool ok = false;
    newDevice.devType = driver.deviceType()->getValueSync(&ok, 5);
    if (ok)
        newDevice.devUDID = driver.UDID()->getValueSync(&ok, 5);
    if (ok)
        newDevice.devModVersion = driver.modificationVersion()->getValueSync(&ok, 5);
    if (ok)
        newDevice.devHwVersion = driver.hardwareVersion()->getValueSync(&ok, 5);
    if (ok)
        newDevice.devFwVersion = driver.firmwareVersion()->getValueSync(&ok, 5);
    if (ok)
        newDevice.devDescription = driver.deviceDescription()->getValueSync(&ok, 5);
    if (ok)
        mDevList.append(newDevice);

    sortDevList();
    return ok;
}

bool cDevMngr::deleteDevice(int address)
{
    if (isTcpIpProtocol()) return true;

    for (int i = 0; i < mDevList.count(); i++)
        if (address == mDevList[i].devAddress)
            mDevList.removeAt(i);
    return true;
}

void cDevMngr::initializeDeviceList()
{
    QSettings settings("Scontel", "RaspPi Server");
    int size = settings.beginReadArray("devices");

    mDevList.clear();

    for (int i = 0; i < size; ++i) {
        deviceInfo device;
        settings.setArrayIndex(i);
        device.devAddress = static_cast<quint8>(settings.value("devAddress", 255).toInt());
        device.devType = settings.value("devType","None").toString();
        device.devDescription = settings.value("devDescription", "None").toString();
        device.devUDID.setUDID((quint8*)settings.value("devUDID","0").toByteArray().data());
        device.devFwVersion = settings.value("devFwVersion", "None").toString();
        device.devHwVersion = settings.value("devHwVersion", "None").toString();
        device.devModVersion = settings.value("devModVersion", "None").toString();
        mDevList.append(device);
    }

    sortDevList();
}

void cDevMngr::updateDeviceListViaTcpIp()
{
    mDevList.clear();

    cuTcpSocketIOInterface mInterface;
    mInterface.setAddress(convertToHostAddress(tcpIpAddress()));
    mInterface.setPort(SERVER_TCPIP_PORT);

    bool ok;
    QString answer = mInterface.tcpIpQuery("SYST:DEVL?\r\n", 1000, &ok);
    answer = answer.replace(QRegExp("\r\n"),"");
    QStringList mDevList_tmp = answer.split(";<br>");
    for (auto mDevInfo: mDevList_tmp){
        QStringList mDevData = mDevInfo.split(": ");
        if (mDevData.count() == 3){
            deviceInfo tmpInfo;
            tmpInfo.devAddress = static_cast<quint8>(mDevData[1].split("=")[1].toInt());
            tmpInfo.devType = mDevData[2].split("=")[1];
            mDevList.append(tmpInfo);
        }
    }
    sortDevList();
}

void cDevMngr::clearDeviceList()
{
    mDevList.clear();
}

void cDevMngr::saveDeviceList()
{
    if (isTcpIpProtocol()){
        cuTcpSocketIOInterface mInterface;
        mInterface.setAddress(convertToHostAddress(tcpIpAddress()));
        mInterface.setPort(SERVER_TCPIP_PORT);
        mInterface.sendMsg(SERVER_ADDRESS, CMD_SERVER_SAVE_DEVICE_LIST, 0, nullptr);
        QElapsedTimer mTimer;
        mTimer.start();
        while(mTimer.elapsed()<1000){
            qApp->processEvents();
        }
        return;
    }

    QSettings settings("Scontel", "RaspPi Server");
    settings.beginWriteArray("devices");
    for (int i = 0; i < mDevList.count(); ++i){
        settings.setArrayIndex(i);
        settings.setValue("devAddress", mDevList[i].devAddress);
        settings.setValue("devType", mDevList[i].devType);
        settings.setValue("devUDID",QByteArray((const char *)mDevList[i].devUDID.UDID(),12));
        settings.setValue("devFwVersion",mDevList[i].devFwVersion);
        settings.setValue("devHwVersion",mDevList[i].devHwVersion);
        settings.setValue("devModVersion",mDevList[i].devModVersion);
        settings.setValue("devDescription",mDevList[i].devDescription);
    }
    settings.endArray();
}

QList<deviceInfo> cDevMngr::devList() const
{
    return mDevList;
}

void cDevMngr::run()
{
    // связываем все сигналы консоли и запускаем ее

    reader->run();
    QObject::connect(reader, &ConsoleReader::quit, [](){exit(0);});
    QObject::connect(reader, &ConsoleReader::list, [=](){
        if (isTcpIpProtocol())
            updateDeviceListViaTcpIp();
        reader->write(QString("Device count: %1").arg(devList().count()));
        for (int i = 0; i < devList().count(); ++i){
            reader->write(QString("Dev: %1; address: %2; type: %3")
                          .arg(i)
                          .arg(devList()[i].devAddress)
                          .arg(devList()[i].devType));
        }
        reader->newCommand();
    });
    QObject::connect(reader, &ConsoleReader::clear, [=](){
        if (isTcpIpProtocol()){
            reader->write("ERROR: Server doesn't support this command. Available only in serial interface mode");
        }
        else{
            clearDeviceList();
            reader->write("DONE!!!");
        }
        reader->newCommand();
    });
    QObject::connect(reader, &ConsoleReader::save, [=](){
        saveDeviceList();
        reader->write("DONE!!!");
        reader->newCommand();
    });
    QObject::connect(reader, &ConsoleReader::add, [=](const int address){
        if (addDevice(address))
            reader->write("DONE!!!");
        else
            reader->write("ERROR!!!");
        reader->newCommand();
    });
    QObject::connect(reader, &ConsoleReader::remove, [=](const int address){
        if (isTcpIpProtocol())
            reader->write("ERROR: Server doesn't support this command. Available only in serial interface mode");
        else
            if (deleteDevice(address))
                reader->write("DONE!!!");
            else
                reader->write("ERROR!!!");
        reader->newCommand();
    });

    QObject::connect(reader, &ConsoleReader::description, [=](const int address){
        cuIOInterfaceImpl *mInterface = nullptr;
        if (isTcpIpProtocol()){
            cuTcpSocketIOInterface *pInterface = new cuTcpSocketIOInterface();
            pInterface->setAddress(convertToHostAddress(tcpIpAddress()));
            pInterface->setPort(SERVER_TCPIP_PORT);
            mInterface = pInterface;
        }
        else{
            cuRs485IOInterface *pInterface = new cuRs485IOInterface();
            pInterface->setPortName(portName());
            mInterface = pInterface;
        }

        CommonDriver driver;
        driver.setIOInterface(mInterface);
        driver.setDevAddress(static_cast<quint8>(address));

        reader->write(driver.deviceDescription()->getValueSync(nullptr, 5));

        mInterface->deleteLater();
        reader->newCommand();
    });
}

void cDevMngr::sortDevList()
{
    std::sort(mDevList.begin(), mDevList.end(),
          [](const deviceInfo & a, const deviceInfo &b)
    {
        return a.devAddress < b.devAddress;
    });
}
