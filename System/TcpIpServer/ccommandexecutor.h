#ifndef CCOMMANDEXECUTER_H
#define CCOMMANDEXECUTER_H

#include <QObject>
#include <QTimer>
#include "cdeviceinfo.h"
#include "settingsprovider.h"

class cuIOInterface;
class cAbstractCommandParser;

class cCommandExecutor : public QObject
{
    Q_OBJECT
public:
    explicit cCommandExecutor(QObject *parent = nullptr);

    void prepareAnswer(quint8 address, quint8 command, quint8 dataLength, char *data);
    void prepareAnswer(QString answer);
    bool addDevice(quint8 address);

    cuIOInterface *interface() const;
    void setInterface(cuIOInterface *interface);

    SettingsProvider *settings() const;
    void setSettings(SettingsProvider *value);

    void moveToThread(QThread *thread);

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
    cuIOInterface *mInterface;
    QTimer *processTimer;
    QList<QPair <QObject*, QByteArray> > cmdList;
    QObject* currentTcpIpProcess;
    SettingsProvider *mSettings;
    QList<cAbstractCommandParser*> parsers;
    bool mStopFlag;

    void initialize();
    bool checkDevice(const cDeviceInfo &info);

};

#endif // CCOMMANDEXECUTER_H
