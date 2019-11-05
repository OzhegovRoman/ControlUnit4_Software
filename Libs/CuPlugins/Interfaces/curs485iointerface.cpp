#include "curs485iointerface.h"
#include "cstarprotocolpc.h"

#ifdef RASPBERRY_PI
#include "../RaspPiMMap/rasppimmap.h"
#endif


cuRs485IOInterface::cuRs485IOInterface(QObject *parent):
    cuIOInterfaceImpl(parent),
    mSerialPort(nullptr),
    mPortName(QString())
{

}

cuRs485IOInterface::~cuRs485IOInterface()
{
    if (mSerialPort != nullptr){
        if (mSerialPort->isOpen()) mSerialPort->close();
        delete mSerialPort;
    }
}

QString cuRs485IOInterface::portName() const
{
    return mPortName;
}

void cuRs485IOInterface::setPortName(const QString &portName)
{
    mPortName = portName;
}

void cuRs485IOInterface::setReceverEnable()
{
#ifdef RASPBERRY_PI
    mmapGpio rpiGpio;
    rpiGpio.writePinLow(24);
#endif
}

void cuRs485IOInterface::setTransmitterEnable()
{
#ifdef RASPBERRY_PI
    mmapGpio rpiGpio;
    rpiGpio.setPinDir(24, mmapGpio::OUTPUT);
    rpiGpio.writePinHigh(24);
#endif
}

bool cuRs485IOInterface::pSendMsg(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    setTransmitterEnable();
    // если вдруг не инициализировано устройство, то его надо проинициализировать
    if (mSerialPort == nullptr)
        if (!initialize()) return false;

    // кодируем дату
    cStarProtocolPC& starProtocol = cStarProtocolPC::instance();
    starProtocol.codeData(address, command, dataLength, reinterpret_cast<char*>(data));

    // аккуратно отсылаем данные в SerialPort
    QByteArray ba = QByteArray((const char*)starProtocol.buffer(),MaxBufferSize);
    int l = ba.indexOf(END_PACKET)+1;

    mSerialPort->write((const char*)starProtocol.buffer(),l);
    mSerialPort->flush();
    bool ok = mSerialPort->waitForBytesWritten(5);


    setReceverEnable();
    return ok;
}

bool cuRs485IOInterface::pInitialize()
{
//    qDebug()<<"cuRs485IOInterface::pInitialize()";

    cStarProtocolPC::instance().clearBuffer();

    if (mPortName.isEmpty()) return false; //имя порта не установлено

    QList<QSerialPortInfo> serialPortsList = QSerialPortInfo::availablePorts();
//    qDebug()<<"Available serial ports:";
//    foreach (QSerialPortInfo serialPortInfo, serialPortsList) {
//        qDebug()<<serialPortInfo.portName()<<serialPortInfo.description();
//    }

    if (mSerialPort == nullptr) {
        mSerialPort = new QSerialPort(mPortName);
        mSerialPort->setBaudRate(UART_BAUD_RATE);
        mSerialPort->setDataBits(QSerialPort::Data8);
        mSerialPort->setParity(QSerialPort::NoParity);
    }

    if (!mSerialPort->isOpen())
        if (!mSerialPort->open(QIODevice::ReadWrite)) {
            qDebug()<<"Error at opening serialPort";
            qDebug()<<mSerialPort->error();
            deleteLater();
            return false;
        }

    // когда данные будут готовы для чтения будем читать их через соединение сигнал-слот
    connect(mSerialPort, SIGNAL(readyRead()), this, SLOT(dataReady()));
    return true;
}

void cuRs485IOInterface::dataReady()
{
//    qDebug()<<"RS485 data ready";
    QByteArray ba = mSerialPort->readAll();
//    qDebug()<<"Data"<<ba.toHex().data();
    for (int i = 0; i < ba.length(); ++i) {
        uint8_t ch = ba[i];
        cStarProtocolPC& sp = cStarProtocolPC::instance();
        if (ch == START_PACKET) sp.clearBuffer();

        sp.addData(&ch,1);

        if (ch == END_PACKET) {
            if (sp.encodeBuffer() == NO_ERROR) {
                emit msgReceived(sp.headPacket().Address,
                                 sp.headPacket().Command,
                                 sp.headPacket().dataLength,
                                 sp.buffer());
            }
        }
    }
}
