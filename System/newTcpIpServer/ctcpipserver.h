#ifndef CTCPIPSERVER_H
#define CTCPIPSERVER_H

#include <QTcpServer>

class cuIOInterface;
class cCommandParser;

class cTcpIpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit cTcpIpServer(QObject * parent = nullptr);
    void StartServer();

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
};

#endif // CTCPIPSERVER_H
