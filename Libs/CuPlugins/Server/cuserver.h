#ifndef CUSERVER_H
#define CUSERVER_H

#include <QObject>
#include "../cuiodeviceimpl.h"
#include <QTcpServer>
#include <QSignalMapper>
#include <QTcpSocket>
#include <QTimer>
#include "../cudid.h"
#include "servercommands.h"
#include "../StarProtocol/star_prc_structs.h"

typedef QPair<QTcpSocket *, QByteArray> SocketData;

struct deviceInfo {
    quint8  devAddress;
    QString devType;
    cUDID   devUDID;
    QString devModVersion;
    QString devHwVersion;
    QString devFwVersion;
    QString devDescription;
};

class cuServer: public cuIODeviceImpl
{
    Q_OBJECT
public:
    explicit cuServer(QObject *parent = nullptr);
    ~cuServer();
    void initialize();

    void initializeDeviceList();
    void clearDeviceList();
    void saveDeviceInformation();

    int serverTimeOut() const;
    void setServerTimeOut(int serverTimeOut);

protected:
    bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data);

private slots:
    void newConnection();
    void dataReady(QObject *sender);
    void destroySocket(QObject *sender);

    void timeOut();

    void systemRequest(QString str, QString params);
    void generalRequest(QString str, QString params);
    void sspdRequest(QString str, QString params);
    void temperatureRequest(QString str, QString params);
    void serverRequest(const QByteArray &packet);
    void deviceRequest(deviceInfo *info, QByteArray &packet);
    void sendAnswer(quint8 address, quint8 command, quint8 dataLength, quint8* data);
    void sendString(QString str);

private:
    QTcpServer * mTcpServer;
    QSignalMapper *mDataReadyMapper, *mDestroySocketMapper;
    QList<SocketData> mSocketsData;
    bool mWaitingForAnswer;
    int mServerTimeOut;
    QTimer *mTimer;
    QTcpSocket *mLastSocket;
    QList<deviceInfo> mAvailableDevices;
    bool isSCPICommand;

    static bool sortInfo(const deviceInfo &dInfo1, const deviceInfo &dInfo2);

    bool isDataReady(const QByteArray &ba);
    void sendSocketsDataToDevices();
    deviceInfo* currentDevice(quint8 address);
    bool addDevice(quint8 address);
    void globalDeviceSearch();
    void readDeviceInformation();
    void checkDevicesInformation();
    bool isDeviceInformationCorrect(deviceInfo info);
    void sendPairFloat(pair_t<float> data);
    bool getPairFromJsonString(QString str, pair_t<float> *data);
};

#endif // CUSERVER_H
