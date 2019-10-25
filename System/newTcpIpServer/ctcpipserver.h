#ifndef CTCPIPSERVER_H
#define CTCPIPSERVER_H

#include <QTcpServer>

class cuIOInterface;
class cCommandExecutor;

class cTcpIpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit cTcpIpServer(QObject * parent = nullptr);
    ~cTcpIpServer() override;

    void initialize();
    void stop();

    static void consoleWrite(QString string);
    static void consoleWriteDebug(QString string);
    static void consoleWriteError(QString string);

    cCommandExecutor *executor() const;
    void setExecutor(cCommandExecutor *executor);

protected:
    void incomingConnection(qintptr handle) override;

private:
    cCommandExecutor *mExecutor;
private slots:
    void startServer();
    void sendAnswer(QObject * process, QByteArray data);
};

#endif // CTCPIPSERVER_H
