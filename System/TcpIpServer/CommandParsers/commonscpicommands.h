#ifndef COMMONSCPICOMMANDS_H
#define COMMONSCPICOMMANDS_H

#include <QObject>
#include "cabstractcommandparser.h"
#include "../../Libs/StarProtocol/star_prc_structs.h"

class CommonScpiCommands: public cAbstractCommandParser
{
    Q_OBJECT
public:
    explicit CommonScpiCommands(QObject * parent = nullptr);

    enum processingAnswer{
        PA_Ok,
        PA_TimeOut,
        PA_ErrorData,
        PA_None
    };

    pair_t<float> pairFromJsonString(QString str, bool *ok);
    void sendPairFloat(pair_t<float> data);

    // cAbstractCommandParser interface
protected:
    bool isCorrectCommand(const QByteArray &ba);
    bool doIt(const QByteArray &ba);
    virtual bool executeCommand(QString command, QString params);
};

#endif // COMMONSCPICOMMANDS_H
