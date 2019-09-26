#ifndef TCPIPVALIDATOR_H
#define TCPIPVALIDATOR_H

#include <QObject>

#include <QQmlEngine>
#include <QJSEngine>


class TcpIpValidator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString textToValidate READ textToValidate WRITE setTextToValidate)
    Q_PROPERTY(bool validate READ validate)

public:
    QString textToValidate() const;
    void setTextToValidate(const QString &textToValidate);

    bool validate();

signals:
    void validateChanged();

private:
    QString mTextToValidate;
};

static QObject *tcpIpValidatorProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    auto *singletonClass = new TcpIpValidator();
    return singletonClass;
}


#endif // TCPIPVALIDATOR_H
