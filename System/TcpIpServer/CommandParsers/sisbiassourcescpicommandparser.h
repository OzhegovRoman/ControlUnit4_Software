#ifndef SISBIASSOURCESCPICOMMANDPARSER_H
#define SISBIASSOURCESCPICOMMANDPARSER_H

#include <QObject>
#include "commonscpicommands.h"

class SisBiasSourceScpiCommandParser : public CommonScpiCommands
{
    Q_OBJECT
public:
    explicit SisBiasSourceScpiCommandParser(QObject * parent = nullptr);

    // CommonScpiCommands interface
protected:
    virtual bool executeCommand(QString command, QString params) override;
};

#endif // SISBIASSOURCESCPICOMMANDPARSER_H
