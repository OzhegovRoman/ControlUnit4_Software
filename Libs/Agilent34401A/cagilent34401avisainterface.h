#ifndef CAGILENT34401AVIASINTERFACE_H
#define CAGILENT34401AVIASINTERFACE_H

#include <QObject>
#include "cagilent34401a.h"
#include "../VISA/visa.h"
#include <QLibrary>

#define MAX_SCPI_LEN    255
#define DEFAULT_TMO     5000

using lviWrite = ViStatus (*)(ViSession, ViBuf, ViUInt32, ViPUInt32);
using lviRead  = ViStatus (*)(ViSession, ViBuf, ViUInt32, ViPUInt32);

class cAgilent34401aVisaInterface : public cAgilent34401A
{
    Q_OBJECT
public:
    explicit cAgilent34401aVisaInterface();
    ~cAgilent34401aVisaInterface() override;

    QString visaDeviceName() const;
    void setVisaDeviceName(const QString &visaDeviceName);

    //cAgilent34401A interface
    bool isVisaInited() const;

protected:
    quint64 writeAgilent(QString str) override;
    qreal readAgilent() override;
    bool initialize() override;

private:
    QString mVisaDeviceName;
    ViSession visaSession{};
    bool mVisaInited;
};

#endif // CAGILENT34401AVIASINTERFACE_H
