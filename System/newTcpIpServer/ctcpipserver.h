#ifndef CTCPIPSERVER_H
#define CTCPIPSERVER_H

#include <QTcpServer>

class cuIOInterface;
class cCommandParser;
class cCommandExecutor;

class cTcpIpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit cTcpIpServer(QObject * parent = nullptr);
    ~cTcpIpServer() override;
    void StartServer();
    void stop();

    cuIOInterface *interface() const;
    void setInterface(cuIOInterface *interface);

    static void consoleWrite(QString string);
    static void consoleWriteDebug(QString string);
    static void consoleWriteError(QString string);

protected:
    void incomingConnection(qintptr handle) override;

private:
    cuIOInterface *mInterface;
    cCommandParser *mParser;
    cCommandExecutor *mExecutor;
};

#endif // CTCPIPSERVER_H
