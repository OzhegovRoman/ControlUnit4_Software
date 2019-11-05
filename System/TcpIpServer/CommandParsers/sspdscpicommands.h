#ifndef SSPDSCPICOMMANDS_H
#define SSPDSCPICOMMANDS_H

#include <QObject>
#include "commonscpicommands.h"

class SspdScpiCommands: public CommonScpiCommands
{
    Q_OBJECT
public:
    explicit SspdScpiCommands(QObject * parent = nullptr);
    // CommonScpiCommands interface
protected:
    bool executeCommand(QString command, QString params) override;
};

#endif // SSPDSCPICOMMANDS_H
