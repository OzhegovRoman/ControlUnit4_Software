#ifndef CAGILENT34401AVIASINTERFACE_H
#define CAGILENT34401AVIASINTERFACE_H

#include <QObject>
#include "cagilent34401a.h"
#include "../VISA/visa.h"
#include <QLibrary>

#define MAX_SCPI_LEN    255
#define DEFAULT_TMO     5000

class cAgilent34401aVisaInterface : public cAgilent34401A
{
    Q_OBJECT
public:
    explicit cAgilent34401aVisaInterface();
    ~cAgilent34401aVisaInterface();

    QString visaDeviceName() const;
    void setVisaDeviceName(const QString &visaDeviceName);

    //cAgilent34401A interface
    bool isVisaInited() const;

protected:
    quint64 writeAgilent(QString str);
    qreal readAgilent();
    bool initialize();

private:
    QString mVisaDeviceName;
    ViSession visaSession;
    bool mVisaInited;
};

#endif // CAGILENT34401AVIASINTERFACE_H
