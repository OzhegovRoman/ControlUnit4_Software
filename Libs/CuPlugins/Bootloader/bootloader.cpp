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
    mSerialPort(new QSerialPort(this)),
    mHotPlugFlag(false)
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

    if (!mHotPlugFlag)
        rebootDevice();

    delayMs(10);

    if (!openSerialPort()) return;

    if (!initializeLoader()) {
        mSerialPort->close();
        return;
    }

    emit infoLoader(QString("Info: %1").arg(getBootloaderInfo()));

    bool completeFlag = false;

    while (!completeFlag){
        clearProgram();
        completeFlag = writeProgram(fileName);
    }

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
    emit infoLoader("Getting bootloader info");
    qApp->processEvents();

    writeData("G");

    if (!mSerialPort->waitForReadyRead(2000)){
        emit errorLoader(tr("No any answer from device"));
        emit loaderFinished();
        return QString();
    }

    delayMs(100);

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
    if (mHotPlugFlag)
        emit infoLoader("Please connect device to the ControlUnit");

    qApp->processEvents();

    while (true) {
        writeData("Init");

        if (!(mSerialPort->waitForReadyRead(2000) || mHotPlugFlag)){
            emit errorLoader(tr("No any answer from device"));
            emit loaderFinished();
            return false;
        }

        QByteArray ba = mSerialPort->read(2);

        if ((ba.size() == 2) && (QString(ba).indexOf("Ok") == 0))
                return true;

        if (!mHotPlugFlag) {
            emit errorLoader(tr("Wrong answer from Bootloader"));
            emit loaderFinished();
            return false;
        }
    }
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
        emit errorLoader(tr("Wrong answer from Bootloader"));
        emit loaderFinished();
        return;
    }
}

bool Bootloader::isHotPlugFlagSetted() const
{
    return mHotPlugFlag;
}

void Bootloader::setHotPlugFlag(bool hotPlugFlag)
{
    mHotPlugFlag = hotPlugFlag;
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

    // сбрасываем адрес начала чтения
    emit infoLoader("Reset pointer to the program memory");
    qApp->processEvents();

    while (true){
        writeData("C");
        if (mSerialPort->waitForReadyRead(100)){
            QByteArray ba = mSerialPort->read(2);
            if ((ba.size() == 2) && (QString(ba).indexOf("Ok") == 0))
                //сбросили
                break;
        }
    }

    emit infoLoader("Delete Old Program (it may takes up to 5 seconds)");
    qApp->processEvents();

    writeData("D");

    if (!mSerialPort->waitForReadyRead(5000)){
        emit errorLoader(tr(""));
        emit errorLoader(tr("Error! No any answer from Bootloader"));
        emit loaderFinished();
        return;
    }

    QByteArray ba;
    ba =  mSerialPort->read(2);
    emit infoLoader(QString(ba));
    if (!((ba.size() == 2) && (QString(ba).indexOf("Ok") == 0))) {
        emit errorLoader("");
        emit errorLoader(tr("Error! Wrong answer from Bootloaser"));
        emit loaderFinished();
        return;
    }
}

bool Bootloader::writeProgram(const QString& fileName)
{
    qApp->processEvents();
    emit infoLoader("Start programming device");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorLoader(tr("Can't open *.bin file"));
        emit loaderFinished();
        return false;
    }
    uint8_t Count = 128;

    uint32_t fileSize = file.size();
    uint32_t pos = 0;
    char buffer[Count+5];

    bool nextFlag = true;
    bool fatalError = false;

    while (!file.atEnd()) {
        qApp->processEvents();
        Count = 128;
        memset(buffer, 0xFF, Count+5);
        buffer[0] = 'P';
        if (nextFlag)
            file.read(&buffer[1],128);
        uint32_t data = cStarProtocolPC::instance().crc32stm32((uint8_t*)buffer + 1, Count, true);
        memcpy(buffer+Count+1, &data, 4);

        writeData(QByteArray(buffer, Count+5));

        if (!mSerialPort->waitForReadyRead(5000)){
            fatalError = true;
            break;
        }

        delayMs(5);
        QByteArray ba = mSerialPort->read(2);
        if (ba.size() != 2) { //что-то пошло не так надо перезапустить процесс
            fatalError = true;
            break;
        }

        if (QString(ba).indexOf("Er") == 0) { //ошибка пакета? пересылаем вновь
            emit infoLoader("Packet error, try send packet again");
            nextFlag = false;
            continue;
        }

        if (QString(ba).indexOf("Ok") == 0) { //ошибка пакета? пересылаем вновь
            pos += 128;
            emit loaderProgressChanged((pos*100.0)/fileSize);
            nextFlag = true;
            continue;
        }

        fatalError = true;
        break;
    }

    if (fatalError){
        file.close();
        emit errorLoader("");
        emit errorLoader(tr("Fatal error. Bootloader will be start again"));
        emit infoLoader(tr("Restart bootloader"));
        emit restart();
        return false;
    }

    emit loaderProgressChanged(100);
    return true;
}
