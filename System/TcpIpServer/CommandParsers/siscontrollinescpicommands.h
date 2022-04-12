#ifndef SISCONTROLLINESCPICOMMANDS_H
#define SISCONTROLLINESCPICOMMANDS_H

#include <QObject>
#include "commonscpicommands.h"

class SisControlLineScpiCommands : public CommonScpiCommands
{
    Q_OBJECT
public:
    SisControlLineScpiCommands(QObject * parent = nullptr);

    // CommonScpiCommands interface
protected:
    bool executeCommand(QString command, QString params);
};

#endif // SISCONTROLLINESCPICOMMANDS_H
