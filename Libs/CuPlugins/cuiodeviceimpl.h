#ifndef CUIODEVICEIMPL_H
#define CUIODEVICEIMPL_H

#include <QObject>
#include "cuiodevice.h"

class cuIODeviceImpl : public cuIODevice
{
    Q_OBJECT
public:
    explicit cuIODeviceImpl(QObject *parent = nullptr);

    cuIOInterface *iOInterface() const override;
    void setIOInterface(cuIOInterface *iOInterface) override;

    quint8 devAddress() const override;
    void setDevAddress(const quint8 &devAddress) override;

public slots:
    void sendMsg(quint8 command, quint8 dataLength, quint8* data) override;
    void msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data) override;
protected:
    virtual bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data) = 0;
private:
    cuIOInterface *mIOInterface{nullptr};
    quint8 mDevAddress{0};

};

#endif // CUIODEVICEIMPL_H
