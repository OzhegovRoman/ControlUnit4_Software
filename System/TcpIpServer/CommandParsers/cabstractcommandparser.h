#ifndef CABSTRACTCOMMANDPARSER_H
#define CABSTRACTCOMMANDPARSER_H

#include <QObject>
#include "../ccommandexecutor.h"

class cAbstractCommandParser : public QObject
{
    Q_OBJECT
public:
    explicit cAbstractCommandParser(QObject *parent = nullptr);
    bool parse(const QByteArray &ba);

    virtual void initializeParser(){}

    cCommandExecutor *executor() const;
    void setExecutor(cCommandExecutor *executor);

protected:
    virtual bool isCorrectCommand(const QByteArray &ba);
    virtual bool doIt(const QByteArray &ba);
private:
    cCommandExecutor * mExecutor;

};

#endif // CABSTRACTCOMMANDPARSER_H
