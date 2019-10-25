#ifndef CCOMMANDEXECUTER_H
#define CCOMMANDEXECUTER_H

#include <QObject>
#include <QTimer>
#include "cdeviceinfo.h"

class cuIOInterface;
class SettingsProvider;

class Command
{
public:
    Command(QObject *process = nullptr, QByteArray data = QByteArray())
        : mProcess(process)
        , mData(data)
    {}
    QObject *process() const
    {
        return mProcess;
    }
    void setProcess(QObject *process)
    {
        mProcess = process;
    }

    QByteArray data() const
    {
        return mData;
    }
    void setData(const QByteArray &value)
    {
        mData = value;
    }
private:
    QObject *mProcess;
    QByteArray mData;
};

class cCommandExecutor : public QObject
{
    Q_OBJECT
public:
    explicit cCommandExecutor(QObject *parent = nullptr);

    cuIOInterface *interface() const;
    void setInterface(cuIOInterface *interface);

signals:
    void finished();
    void inited();
    void sendAnswer(QObject * process, QByteArray data);

public slots:
    void doWork();
    void stop();
    void executeCommand(QObject* tcpIpProcess, QByteArray data);

private slots:
    void process();

private:
    bool mStopFlag;
    cuIOInterface *mInterface;
    QTimer *processTimer;
    QList<Command> cmdList;
    SettingsProvider *settings;

    void initialize();
    void parse(const QByteArray& data);
    bool isRawData(const QByteArray &ba);
    bool checkDevice(const cDeviceInfo &info);
    void serverRequest(const QByteArray &packet);
    void prepareAnswer(quint8 address, quint8 command, quint8 dataLength, char *data);
    bool addDevice(quint8 address);

};

#endif // CCOMMANDEXECUTER_H
