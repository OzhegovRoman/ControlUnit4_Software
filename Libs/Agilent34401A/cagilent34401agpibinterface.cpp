#include "cagilent34401agpibinterface.h"
#include "decl-32.h"

#include <QDebug>

cAgilent34401aGpibInterface::cAgilent34401aGpibInterface():
    cAgilent34401A(),
    mGpibAddress(0),
    dvm(0),
    mTimeOut(T1s)
{

}

cAgilent34401aGpibInterface::~cAgilent34401aGpibInterface()
{
   if (isInited()) ibonl(dvm, 0);
}

quint64 cAgilent34401aGpibInterface::writeAgilent(QString str)
{
    ibwrt(dvm, (PVOID) str.toLocal8Bit().data(), str.size());
    if (ThreadIbsta() & ERR) {
        gpibError("ibwrt error");
    }
    return ThreadIbcnt();
}

qreal cAgilent34401aGpibInterface::readAgilent()
{
    char buffer[1024];
    ibrd(dvm, buffer, 1024);
    QString dataBuffer;
    dataBuffer.append(QByteArray(buffer, ThreadIbcnt()));
    if (!dataBuffer.contains("\n")) return 0;
    qreal data = dataBuffer.toFloat();
    emit dataReaded(data);
    return data;
}

bool cAgilent34401aGpibInterface::initialize()
{
    if (gpibAddress() > 31) {
        gpibError("Wrong address");
        return false;
    }

    if (timeOut() > T1000s) {
        gpibError("Wrong TimeOuts");
        return false;
    }

    dvm = ibdev(0, gpibAddress(), 0, mTimeOut, 1, 0);
    if (dvm < 0) {
        gpibError("ibdev error");
        return false;
    }

    ibclr(dvm);
    if (ThreadIbsta() & ERR) {
        gpibError("ibclr error");
        return false;
    }

    return cAgilent34401A::initialize();
}

quint8 cAgilent34401aGpibInterface::timeOut() const
{
    return mTimeOut;
}

void cAgilent34401aGpibInterface::setTimeOut(const quint8 &timeOut)
{
    mTimeOut = timeOut;
}


void cAgilent34401aGpibInterface::gpibError(QString message)
{
    QString resultStr;
    int ibsta = ThreadIbsta();
    int iberr = ThreadIberr();
    int ibcnt = ThreadIbcnt();

    resultStr.append(QString("%1\r\nibsta = 0x%2, iberr = 0x%3\r\n")
                     .arg(message)
                     .arg(ibsta, 0, 16)
                     .arg(iberr, 0, 16));

    resultStr.append("ibsta ===> ");
    if (ibsta & ERR) resultStr.append("ERR ");
    if (ibsta & TIMO) resultStr.append("TMO ");
    if (ibsta & END) resultStr.append("END ");
    if (ibsta & SRQI) resultStr.append("SRQI ");
    if (ibsta & RQS) resultStr.append("RQS ");
    if (ibsta & CMPL) resultStr.append("CMPL ");
    if (ibsta & LOK) resultStr.append("LOK ");
    if (ibsta & REM) resultStr.append("REM ");
    if (ibsta & CIC) resultStr.append("CIC ");
    if (ibsta & ATN) resultStr.append("ATN ");
    if (ibsta & TACS) resultStr.append("TACS ");
    if (ibsta & LACS) resultStr.append("LACS ");
    if (ibsta & DTAS) resultStr.append("DTAS ");
    if (ibsta & DCAS) resultStr.append("DCAS ");

    resultStr.append("\r\niberr ===> ");
    if (iberr & EDVR) resultStr.append("EDVR ");
    if (iberr & ECIC) resultStr.append("ECIC ");
    if (iberr & ENOL) resultStr.append("ENOL ");
    if (iberr & EADR) resultStr.append("EADR ");
    if (iberr & EARG) resultStr.append("EARG ");
    if (iberr & ESAC) resultStr.append("ESAC ");
    if (iberr & EABO) resultStr.append("EABO ");
    if (iberr & ENEB) resultStr.append("ENEB ");
    if (iberr & EDMA) resultStr.append("EDMA ");
    if (iberr & EOIP) resultStr.append("EOIP ");
    if (iberr & ECAP) resultStr.append("ECAP ");
    if (iberr & EFSO) resultStr.append("EFSO ");
    if (iberr & EBUS) resultStr.append("EBUS ");
    if (iberr & ESTB) resultStr.append("ESTB ");
    if (iberr & ESRQ) resultStr.append("ESRQ ");
    if (iberr & ETAB) resultStr.append("ETAB ");

    resultStr.append(QString("\r\nibcnt = %1").arg(ibcnt));
    emit errorMessage(resultStr);

    // Call the ibonl function to disable the hardware and software.
    ibonl(dvm, 0);
}

quint8 cAgilent34401aGpibInterface::gpibAddress() const
{
    return mGpibAddress;
}

void cAgilent34401aGpibInterface::setGpibAddress(const quint8 &gpibAddress)
{
    mGpibAddress = gpibAddress;
    if (isInited()) initialize();
}


