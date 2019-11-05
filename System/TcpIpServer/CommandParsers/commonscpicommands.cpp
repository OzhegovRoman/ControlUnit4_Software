#include "commonscpicommands.h"
#include "../ctcpipserver.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

CommonScpiCommands::CommonScpiCommands(QObject *parent)
    : cAbstractCommandParser (parent)
{

}

bool CommonScpiCommands::isCorrectCommand(const QByteArray &ba)
{
    Q_UNUSED(ba)
    return true;
}

bool CommonScpiCommands::doIt(const QByteArray &ba)
{
    // преобразуем в QString;
    QString tmpStr = QString(ba).simplified();
    QStringList cmdList = tmpStr.split(" ");
    QString cmd = cmdList[0];
    QString params;
    if (cmdList.count() > 1){
        cmdList.removeFirst();
        params = cmdList.join(" ");
    }

    return executeCommand(cmd, params);
}

bool CommonScpiCommands::executeCommand(QString command, QString params)
{
    cTcpIpServer::consoleWriteDebug(QString("SCPI command: %1. Params: %2").arg(command).arg(params));

    if (!executor()){
        cTcpIpServer::consoleWriteError("executor empty");
        return false;
    }


    if (command == "*IDN?"){
        executor()->prepareAnswer("Scontel ControlUnit,00001,0.01.02\r\n");
        return true;
    }

    return false;
}

pair_t<float> CommonScpiCommands::pairFromJsonString(QString str, bool *ok)
{
    pair_t<float> data;
    if (ok)
        *ok = false;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(str.toUtf8()));
    if (!jsonDoc.isArray()){
        return data;
    }
    QJsonArray value = jsonDoc.array();
    if (value.size()!=2){
        return data;
    }
    data.first  = static_cast<float>(value[0].toDouble());
    data.second = static_cast<float>(value[1].toDouble());
    if (ok)
        *ok = true;
    return data;
}

void CommonScpiCommands::sendPairFloat(pair_t<float> data)
{
    QJsonArray value;
    value.append(static_cast<double>(data.first));
    value.append(static_cast<double>(data.second));
    QJsonDocument jsonDoc(value);
    executor()->prepareAnswer(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
}
