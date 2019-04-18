#include "cagilent34401avisainterface.h"
#include <QDebug>

cAgilent34401aVisaInterface::cAgilent34401aVisaInterface():
    cAgilent34401A(),
    mVisaInited(false)
{
    // инитим прямо тут
    QLibrary lib("visa32");
    if (lib.load())
        mVisaInited = true;
   qDebug()<<"mVisaInited"<<mVisaInited;
}

cAgilent34401aVisaInterface::~cAgilent34401aVisaInterface()
{

}

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

    qDebug()<<"Try to write Agilent";

    typedef ViStatus (*lviWrite) (ViSession, ViBuf, ViUInt32, ViPUInt32);
    lviWrite write = reinterpret_cast<lviWrite> (lib.resolve("viWrite"));

    ViStatus status = write(visaSession,
                              reinterpret_cast<unsigned char*>(str.toLocal8Bit().data()),
                              static_cast<ViUInt32>(str.length()),
                              &retCount);
    if (status > VI_SUCCESS){
        qDebug()<<"error at write some data to agilent";
        return 0;
    }
    return retCount;
}

qreal cAgilent34401aVisaInterface::readAgilent()
{
    ViByte rdBuff[MAX_SCPI_LEN];
    long unsigned int retCount = 0;
    QLibrary lib("visa32");

    if (!lib.load()){
        mVisaInited = false;
        return false;
    }
    typedef ViStatus (*lviRead) (ViSession, ViBuf, ViUInt32, ViPUInt32);
    lviRead read = reinterpret_cast<lviRead> (lib.resolve("viRead"));

    qDebug()<<"Try to read Agilent";
    ViStatus status = read(visaSession, rdBuff, MAX_SCPI_LEN, &retCount);
    if (status> VI_SUCCESS){
        qDebug()<<"error at read some data to agilent";
        return 0;
    }
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

    qDebug()<<"Try to Open session";
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


    qDebug()<<"Try to set attributes";
    /* set visa Format */
    status = setAttribute(visaSession, VI_ATTR_TMO_VALUE, DEFAULT_TMO);
    qDebug()<<"status"<<status;
    status = setAttribute(visaSession, VI_ATTR_SUPPRESS_END_EN, VI_FALSE);
    qDebug()<<"Try to set attributes";
    status = setAttribute(visaSession, VI_ATTR_SEND_END_EN, VI_FALSE);
    qDebug()<<"Try to set attributes";

    return cAgilent34401A::initialize();
}

bool cAgilent34401aVisaInterface::isVisaInited() const
{
    return mVisaInited;
}