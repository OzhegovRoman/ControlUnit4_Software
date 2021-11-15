#ifndef HEATERSCPICOMMANDS_H
#define HEATERSCPICOMMANDS_H

#include <QObject>
#include "commonscpicommands.h"

class HeaterScpiCommands : public CommonScpiCommands
{
    Q_OBJECT
public:
    explicit HeaterScpiCommands(QObject * parent = nullptr);

    // CommonScpiCommands interface
protected:
    bool executeCommand(QString command, QString params);
};

#endif // HEATERSCPICOMMANDS_H
