#include "tcpipvalidator.h"
#include <QDebug>
#include <QRegExp>

QString TcpIpValidator::textToValidate() const
{
    return mTextToValidate;
}

void TcpIpValidator::setTextToValidate(const QString &textToValidate)
{
    qDebug()<<"textToValidate"<<textToValidate;
    mTextToValidate = textToValidate;
    emit validateChanged();
}

bool TcpIpValidator::validate()
{
    QRegExp rx("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return rx.indexIn(mTextToValidate) == 0;
}
