#ifndef CURS485IOINTERFACE_H
#define CURS485IOINTERFACE_H

#include <QObject>
#include <QtSerialPort>
#include "cuiointerfaceimpl.h"

//TODO: Покрыть тестами. Но как????
class cuRs485IOInterface : public cuIOInterfaceImpl
{
    Q_OBJECT
public:
    explicit cuRs485IOInterface(QObject *parent = nullptr);
    ~cuRs485IOInterface();

    QString portName() const;
    void setPortName(const QString &portName);

    void setReceverEnable();
    void setTransmitterEnable();

protected:
    bool pSendMsg(quint8 address, quint8 command, quint8 dataLength, quint8* data);
    bool pInitialize();

private:
    QSerialPort *mSerialPort;
    QString mPortName;
private slots:
     void dataReady();
};

#endif // CURS485IOINTERFACE_H
