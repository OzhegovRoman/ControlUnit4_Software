#ifndef TEMPERATURESCPICOMMANDS_H
#define TEMPERATURESCPICOMMANDS_H

#include <QObject>
#include "commonscpicommands.h"

class TemperatureScpiCommands: public CommonScpiCommands
{
    Q_OBJECT
public:
    explicit TemperatureScpiCommands(QObject * parent = nullptr);

    // CommonScpiCommands interface
protected:
    bool executeCommand(QString command, QString params) override;
};

#endif // TEMPERATURESCPICOMMANDS_H
