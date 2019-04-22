#ifndef CAGILENT34401AGPIBINTERFACE_H
#define CAGILENT34401AGPIBINTERFACE_H

#include <QObject>
#include "cagilent34401a.h"

class cAgilent34401aGpibInterface : public cAgilent34401A
{
    Q_OBJECT
public:
    explicit cAgilent34401aGpibInterface();
    ~cAgilent34401aGpibInterface() override;

    quint8 gpibAddress() const;
    void setGpibAddress(const quint8 &gpibAddress);

    quint8 timeOut() const;
    void setTimeOut(const quint8 &timeOut);

protected:
    quint64 writeAgilent(QString str) override;
    qreal readAgilent() override;
    bool initialize() override;
private:
    quint8 mGpibAddress;
    int dvm;
    quint8 mTimeOut;
    void gpibError(const QString& message);
};

#endif // CAGILENT34401AGPIBINTERFACE_H
