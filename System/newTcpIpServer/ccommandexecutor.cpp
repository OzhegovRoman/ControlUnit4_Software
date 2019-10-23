#include "ccommandexecutor.h"
#include <QThread>
#include "ctcpipserver.h"

cCommandExecutor::cCommandExecutor(QObject *parent)
    : QObject(parent)
    , mStopFlag(false)
{

}

void cCommandExecutor::doWork()
{
    cTcpIpServer::consoleWriteDebug("Command executor started");
    while (!mStopFlag){
        QThread::msleep(10);
    }
    cTcpIpServer::consoleWriteDebug("Command executor finished");
    emit finished();
}

void cCommandExecutor::stop()
{
    cTcpIpServer::consoleWriteDebug("Command executor get STOP signal");
    mStopFlag = true;
}
