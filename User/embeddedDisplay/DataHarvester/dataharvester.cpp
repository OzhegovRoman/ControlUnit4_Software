#include "dataharvester.h"
#include "Interfaces/cutcpsocketiointerface.h"
#include "Interfaces/curs485iointerface.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/tempdriverm0.h"
#include "Drivers/tempdriverm1.h"
#include "../qCustomLib/qCustomLib.h"
#include "servercommands.h"
#include <QSettings>
#include <QDebug>

DataHarvester::DataHarvester(cuIOInterfaceImpl *interface)
    : QObject ()
    , mInterface (interface)
    , mMode (hmSleep)
{
    qDebug()<<"create DataHarvester";
}

void DataHarvester::initialize()
{
    initializeDriverList();
}

DataHarvester::harvesterMode DataHarvester::mode() const
{
    return mMode;
}

void DataHarvester::setMode(const DataHarvester::harvesterMode &mode)
{
    mMode = mode;
}

QVector<CommonDriver *> DataHarvester::drivers() const
{
    return mDrivers;
}

cuIOInterfaceImpl *DataHarvester::interface() const
{
    return mInterface;
}

void DataHarvester::setInterface(cuIOInterfaceImpl *interface)
{
    mInterface = interface;
}

void DataHarvester::initializeDriverList()
{
    if (mInterface == nullptr)
        return;

    QString answer;

    auto interface = qobject_cast<cuTcpSocketIOInterface*>(mInterface);
    if (interface){
        qDebug()<<"interface->tcpIpQuery";
        bool ok = false;
        int count = 5;
        while (!ok && count > 0){
            answer = interface->tcpIpQuery("SYST:DEVL?\r\n", 500, &ok);
            count--;
        }
        qDebug()<<answer;
    }
    else {
        QSettings settings("Scontel", "RaspPi Server");
        int size = settings.beginReadArray("devices");

        answer.append(QString("DevCount: %1\r\n").arg(size));

        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);

            answer.append(";<br>");
            answer.append(QString("Dev%1: address=%2: type=%3\r\n")
                          .arg(i)
                          .arg(settings.value("devAddress", 255).toInt())
                          .arg(settings.value("devType","None").toString()));
        }

        settings.endArray();
    }

    QStringList list = answer.split("<br>");
    // составляем список всех устройств, каждый из них инициализируем  и т. д.
    for (QString str : list) {
        QStringList lList = str.split(':');
        if (lList.size() == 3){ //должно быть описание устройства
            //первое значение DevX - X порядковый номер

            int address = lList[1].split('=')[1].toInt();
            QString type = lList[2].split('=')[1];

            CommonDriver* tmpDriver = nullptr;

            if (type.contains("CU4SDM0")){
                //данное устройство - SspdDriver
                tmpDriver = new SspdDriverM0(this);
            }
            else if (type.contains("CU4SDM1")){
                //данное устройство - SspdDriver
                tmpDriver = new SspdDriverM1(this);
            }
            else if (type.contains("CU4TDM0"))
                //данное устройство - TempDriver
                tmpDriver = new TempDriverM0(this);
            else if (type.contains("CU4TDM1")){
                //данное устройство - TempDriver
                tmpDriver = new TempDriverM1(this);
            }

            if (tmpDriver == nullptr)
                continue;

            tmpDriver->setDevAddress(static_cast<quint8>(address));
            tmpDriver->setIOInterface(mInterface);
            tmpDriver->deviceType()->getValueSync(nullptr, 5);

            mDrivers.append(tmpDriver);

            if (tmpDriver->deviceType()->currentValue().contains("CU4TDM1")){
                //данное устройство - TempDriverM1
                qobject_cast<TempDriverM1*>(tmpDriver)->readDefaultParams();
            }

        }
    }
}
