#ifndef ABSTRACTDRIVER_V2_H
#define ABSTRACTDRIVER_V2_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include "../cuiodeviceimpl.h"

class DriverProperty_p;

class AbstractDriver : public cuIODeviceImpl
{
    Q_OBJECT
    using cuIODeviceImpl::sendMsg;

public:
    explicit AbstractDriver(QObject * parent = nullptr);

    bool waitingAnswer();

    int timeOut() const;
    void setTimeOut(int value);

    void sendMsg(quint8 command, QByteArray data);

    // cuIODeviceImpl interface
protected:
    void appendProperty(DriverProperty_p* property);
    virtual bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data) override;

private:
    bool mAnswerReceive {false};
    bool mTimeOut {false};
    int mDriverTimeOut {200};
    QTimer *mTimer;
    QVector<DriverProperty_p*> mProperties;

    friend DriverProperty_p;
};

#endif // ABSTRACTDRIVER_V2_H
