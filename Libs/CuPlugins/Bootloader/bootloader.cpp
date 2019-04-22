#include "bootloader.h"
#include "QFileInfo"
#include "Drivers/adriver.h"
#include "Interfaces/curs485iointerface.h"
#include "QElapsedTimer"
#include "star_prc_structs.h"
#include "cstarprotocolpc.h"

#ifdef RASPBERRY_PI
#include "../RaspPiMMap/rasppimmap.h"
#endif

Bootloader::Bootloader(QObject *parent):
    QObject(parent),
    mSerialPort(new QSerialPort(this))
{

}

void Bootloader::program(const QString& fileName)
{
    if (mPortName.isEmpty()) {
        emit errorLoader(tr("PortName is not setted"));
        emit loaderFinished();
        return;
    }

    QFileInfo info(fileName);
    if (!info.exists()) {// файла такого нет
        emit errorLoader(QString(tr("Can't find file: %1")).arg(fileName));
        emit loaderFinished();
        return;
    }

    emit infoLoader("Reboot devices");
    rebootDevice();

    delayMs(10);

    if (!openSerialPort()) return;

    if (!initializeLoader()) {
        mSerialPort->close();
        return;
    }

    emit infoLoader(QString("Bootloader: %1").arg(getBootloaderInfo()));

    clearProgram();
    writeProgram(fileName);

    startApplication();
    mSerialPort->close();
    emit loaderFinished();
}

QString Bootloader::portName() const
{
    return mPortName;
}

void Bootloader::setPortName(const QString &portName)
{
    mPortName = portName;
}

QString Bootloader::getBootloaderInfo()
{
    emit infoLoader("getBootloaderInfo");
    qApp->processEvents();

    writeData("G");

    emit infoLoader("getBootloaderInfo");
    if (!mSerialPort->waitForReadyRead(2000)){
        emit errorLoader(tr("No any answer from device"));
        emit loaderFinished();
        return QString();
    }

    delayMs(100);

    emit infoLoader("pass");
    QByteArray ba = mSerialPort->read(128);
    if (!ba.size()) {
        emit errorLoader(tr("Wrong answer from Bootloader"));
        emit loaderFinished();
        return QString();
    }

    return QString(ba);
}

void Bootloader::rebootDevice()
{
    /// Создаем драйвер общего назначения и приделываем ему интерфейс
    AbstractDriver driver;
    driver.setDevAddress(ANY_DEVICE_ADDRESS);
    cuRs485IOInterface interface;
    interface.setPortName(mPortName);
    driver.setIOInterface(&interface);
    driver.rebootDevice();
}

bool Bootloader::initializeLoader()
{
    emit infoLoader("Initialization Bootloader");
    qApp->processEvents();

    writeData("Init");

    if (!mSerialPort->waitForReadyRead(2000)){
        emit errorLoader(tr("No any answer from device"));
        emit loaderFinished();
        return false;
    }

    char buffer[10];
    int numread =  mSerialPort->read(buffer, 2);
    if (!((numread == 2) && (QString(buffer).indexOf("Ok") == 0))) {
        emit errorLoader(tr("Wrong answer from Bootloader"));
        emit loaderFinished();
        return false;
    }

    return true;
}

bool Bootloader::openSerialPort()
{
    emit infoLoader("Opening Serial Port");
    qApp->processEvents();

    mSerialPort->setPortName(mPortName);
    mSerialPort->setBaudRate(UART_BAUD_RATE);
    mSerialPort->setDataBits(QSerialPort::Data8);
    mSerialPort->setParity(QSerialPort::NoParity);

    if (!mSerialPort->isOpen())
        if (!mSerialPort->open(QIODevice::ReadWrite)) {
            emit errorLoader(QString(tr("Error at opening serialPort: %1")).arg(mSerialPort->error()));
            emit loaderFinished();
            return false;
        }
    return true;
}

void Bootloader::startApplication()
{
    emit infoLoader("Start Application");
    qApp->processEvents();

    writeData("S");

    if (!mSerialPort->waitForReadyRead(1000)){
        emit errorLoader(tr("No any answer from Bootloader"));
        emit loaderFinished();
        return;
    }

    char buffer[10];
    int numread =  mSerialPort->read(buffer, 2);
    if (!((numread == 2) && (QString(buffer).indexOf("Ok") == 0))) {
        emit errorLoader(tr("Wrong answer from Bootloaser"));
        emit loaderFinished();
        return;
    }
}

void Bootloader::delayMs(int value)
{
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed()<value)
        qApp->processEvents();
}

void Bootloader::startTransmitte()
{
#ifdef RASPBERRY_PI
    mmapGpio rpiGpio;
    rpiGpio.setPinDir(24, mmapGpio::OUTPUT);
    rpiGpio.writePinHigh(24);
#endif
}

void Bootloader::startReceive()
{
#ifdef RASPBERRY_PI
    mmapGpio rpiGpio;
    rpiGpio.writePinLow(24);
#endif
}

bool Bootloader::writeData(const QByteArray& data)
{
    startTransmitte();
    delayMs(5);
    mSerialPort->write(data);
    mSerialPort->flush();
    bool ok = mSerialPort->waitForBytesWritten(5);
    startReceive();
    return ok;
}

void Bootloader::clearProgram()
{
    emit infoLoader("Delete Old Program");
    qApp->processEvents();

    writeData("D");

    if (!mSerialPort->waitForReadyRead(5000)){
        emit errorLoader(tr("No any answer from Bootloader"));
        emit loaderFinished();
        return;
    }

    char buffer[10];
    int numread =  mSerialPort->read(buffer, 2);
    qDebug()<<buffer;
    if (!((numread == 2) && (QString(buffer).indexOf("Ok") == 0))) {
        emit errorLoader(tr("Wrong answer from Bootloaser"));
        emit loaderFinished();
        return;
    }
}

void Bootloader::writeProgram(const QString& fileName)
{
    qApp->processEvents();
    emit infoLoader("Start programming device");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorLoader(tr("Can't open *.bin file"));
        emit loaderFinished();
        return;
    }
    uint8_t Count = 128;

    uint32_t fileSize = file.size();
    uint32_t pos = 0;
    qDebug()<<"Progress:"<<pos<<"/"<<fileSize;
    char buffer[Count+5];

    while (!file.atEnd()) {
        qApp->processEvents();
        Count = 128;
        memset(buffer, 0xFF, Count+5);
        buffer[0] = 'P';
        file.read(&buffer[1],128);
        uint32_t data = cStarProtocolPC::instance().crc32stm32((uint8_t*)buffer + 1, Count, true);
        memcpy(buffer+Count+1, &data, 4);

        writeData(QByteArray(buffer, Count+5));

        if (!mSerialPort->waitForReadyRead(5000)){
            emit errorLoader(tr("No any answer from device"));
            emit loaderFinished();
            file.close();
            return;
        }

        delayMs(5);

        Count = 2;
        int numread =  mSerialPort->read(buffer, Count);
        if (!((numread == 2) && (QString(buffer).indexOf("Ok") == 0))) {
            emit errorLoader(tr("Wrong answer from Bootloader"));
            emit loaderFinished();
            file.close();
            return;
        }
        pos += 128;
        emit loaderProgressChanged((pos*100.0)/fileSize);
    }
    emit loaderProgressChanged(100);
}
