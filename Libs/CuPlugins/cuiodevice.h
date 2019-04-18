#ifndef CUIODEVICE_H
#define CUIODEVICE_H

#include <QObject>
#include "Interfaces/cuiointerface.h"

#define MAX_COUNT_DEVICES   32

class cuIODevice : public QObject
{
    Q_OBJECT
public:
    explicit cuIODevice(QObject *parent = nullptr): QObject(parent){}

    virtual cuIOInterface *iOInterface() const = 0;
    virtual void setIOInterface(cuIOInterface *iOInterface) = 0;

    virtual quint8 devAddress() const = 0;
    virtual void setDevAddress(const quint8 &devAddress) = 0;

signals:
    void errorInformation(QString);

public slots:
    virtual void sendMsg(quint8 command, quint8 dataLength, quint8* data) = 0;
    virtual void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data) = 0;
};


#endif // CUIODEVICE_H
