#include "cabstractcommandparser.h"

cAbstractCommandParser::cAbstractCommandParser(QObject *parent)
    : QObject(parent)
{
    mExecutor = qobject_cast<cCommandExecutor*>(parent);
}

bool cAbstractCommandParser::parse(const QByteArray &ba)
{
    if (isCorrectCommand(ba))
        return doIt(ba);
    return false;
}

bool cAbstractCommandParser::isCorrectCommand(const QByteArray &ba)
{
    Q_UNUSED(ba)
    return false;
}

bool cAbstractCommandParser::doIt(const QByteArray &ba)
{
    Q_UNUSED(ba)
    return false;
}

cCommandExecutor *cAbstractCommandParser::executor() const
{
    return mExecutor;
}

void cAbstractCommandParser::setExecutor(cCommandExecutor *executor)
{
    mExecutor = executor;
}
