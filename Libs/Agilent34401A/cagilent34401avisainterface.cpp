#include "cagilent34401avisainterface.h"
#include <QDebug>
#include <QThread>

cAgilent34401aVisaInterface::cAgilent34401aVisaInterface():
    mVisaInited{false}
{
    // инитим прямо тут
#ifdef WIN32
    QLibrary lib("visa32");
#elif WIN64
    QLibrary lib("visa64");
#endif
    if (lib.load())
        mVisaInited = true;
   qDebug()<<"mVisaInited"<<mVisaInited;
}

cAgilent34401aVisaInterface::~cAgilent34401aVisaInterface()
= default;

QString cAgilent34401aVisaInterface::visaDeviceName() const
{
    return mVisaDeviceName;
}

void cAgilent34401aVisaInterface::setVisaDeviceName(const QString &visaDeviceName)
{
    mVisaDeviceName = visaDeviceName;
}

quint64 cAgilent34401aVisaInterface::writeAgilent(QString str)
{
    long unsigned int retCount = 0;
    QLibrary lib("visa32");

    if (!lib.load()){
        mVisaInited = false;
        return false;
    }

    auto write = reinterpret_cast<lviWrite> (lib.resolve("viWrite"));

    ViStatus status = write(visaSession,
                              reinterpret_cast<unsigned char*>(str.toLocal8Bit().data()),
                              static_cast<ViUInt32>(str.length()),
                              &retCount);
    if (status > VI_SUCCESS){
        return 0;
    }
    return retCount;
}

qreal cAgilent34401aVisaInterface::readAgilent()
{
    ViByte rdBuff[MAX_SCPI_LEN];
    unsigned long retCount = 0;
    QLibrary lib("visa32");

    if (!lib.load()){
        mVisaInited = false;
        return false;
    }
    volatile lviRead read = reinterpret_cast<lviRead> (lib.resolve("viRead"));
    if (!read)
        return 0;

    ViStatus status = read(visaSession, rdBuff, MAX_SCPI_LEN, &retCount);

    if (status > VI_SUCCESS){
        qDebug()<<"error at read some data to agilent";
        return 0;
    }

    // я не понимаю, какая то магическачя хрень
    // без qDebuq() код вылетает в релизе в ошибку
    qDebug();

    QString dataBuffer;
    dataBuffer.append(QByteArray(reinterpret_cast<char*> (rdBuff),
                                 static_cast<int>(retCount)));
    qreal data = dataBuffer.toDouble();
    emit dataReaded(data);
    return data;
}

bool cAgilent34401aVisaInterface::initialize()
{
    if (!mVisaInited) return false;

    QLibrary lib("visa32");

    if (!lib.load()){
        mVisaInited = false;
        return false;
    }

    ViSession rmSession;
    typedef ViStatus _VI_FUNC (*lviOpenDefaultRM) (ViPSession);
    typedef ViStatus _VI_FUNC (*lviOpen) (ViSession, ViRsrc, ViAccessMode, ViUInt32, ViPSession);
    typedef ViStatus _VI_FUNC (*lviSetAttribute) (ViObject, ViAttr, ViAttrState);
    lviOpenDefaultRM openDefaultRM = reinterpret_cast<lviOpenDefaultRM> (lib.resolve("viOpenDefaultRM"));
    lviOpen open = reinterpret_cast<lviOpen> (lib.resolve("viOpen"));
    lviSetAttribute setAttribute = reinterpret_cast<lviSetAttribute> (lib.resolve("viSetAttribute"));

    ViStatus status = openDefaultRM(&rmSession);
    if (status > VI_SUCCESS) {
        qDebug()<<"error at opening default RM";
        return false;
    }

    status = open(rmSession, mVisaDeviceName.toLocal8Bit().data(), VI_NULL, VI_NULL, &visaSession);
    if (status > VI_SUCCESS) {
        qDebug()<<"error at connecting to instrument";
        return false;
    }


    /* set visa Format */
    status = setAttribute(visaSession, VI_ATTR_TMO_VALUE, DEFAULT_TMO);
    status = setAttribute(visaSession, VI_ATTR_SUPPRESS_END_EN, VI_FALSE);
    status = setAttribute(visaSession, VI_ATTR_SEND_END_EN, VI_FALSE);

    return cAgilent34401A::initialize();
}

bool cAgilent34401aVisaInterface::isVisaInited() const
{
    return mVisaInited;
}
