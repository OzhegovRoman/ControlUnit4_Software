#include "curs485iointerface.h"
#include "cstarprotocolpc.h"
#include <QElapsedTimer>

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
//    qDebug() << mSerialPort;
//    qDebug()<<ba.toHex();

    mSerialPort->write((const char*)starProtocol.buffer(), l);
    mSerialPort->flush();
#ifdef RASPBERRY_PI
    //    qApp->processEvents();
        QThread::usleep(2000);

    // внимание-внимание flush отправляет в буфер? чтобы начать отправку надо сделать
#else
        qApp->processEvents();
        QThread::usleep(2);
        mSerialPort->waitForBytesWritten(5); //INFO: АЛЯРМА!!! добавил эту хрень и всё заработало (но не с первой же отправки подрубает)
#endif
    setReceverEnable();
    return true;
}

bool cuRs485IOInterface::pInitialize()
{
    cStarProtocolPC::instance().clearBuffer();

    if (mPortName.isEmpty()) return false; //имя порта не установлено

    if (mSerialPort == nullptr) {
        mSerialPort = new QSerialPort(mPortName);
        mSerialPort->setBaudRate(UART_BAUD_RATE);
        mSerialPort->setDataBits(QSerialPort::Data8);
        mSerialPort->setParity(QSerialPort::NoParity);
    }

    if (!mSerialPort->isOpen())
        if (!mSerialPort->open(QIODevice::ReadWrite)) {
            deleteLater();
            return false;
        }

    // когда данные будут готовы для чтения будем читать их через соединение сигнал-слот
    connect(mSerialPort, &QSerialPort::readyRead, this, &cuRs485IOInterface::dataReady, Qt::QueuedConnection);
    return true;
}

void cuRs485IOInterface::dataReady()
{
    QByteArray ba = mSerialPort->readAll();
    for (int i = 0; i < ba.length(); ++i) {
        uint8_t ch = ba[i];
        cStarProtocolPC& sp = cStarProtocolPC::instance();
        if (ch == START_PACKET) sp.clearBuffer();

        sp.addData(&ch,1);

        if (ch == END_PACKET) {
            if (sp.encodeBuffer() == StartProtocol::speOk) {
                emit msgReceived(sp.headPacket().Address,
                                 sp.headPacket().Command,
                                 sp.headPacket().dataLength,
                                 sp.buffer());
            }
        }
    }
}
