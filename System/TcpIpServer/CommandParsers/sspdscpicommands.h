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

private:
    processingAnswer processingCommandSDM0(quint8 address, QString command, QString params);
    processingAnswer processingCommandSDM1(quint8 address, QString command, QString params);
};

#endif // SSPDSCPICOMMANDS_H
