#include "cdevboot.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QDebug>
#include <QStandardPaths>

#include <Drivers/adriver.h>
#include <Interfaces/curs485iointerface.h>
#include "Bootloader/bootloader.h"
#ifdef RASPBERRY_PI
#include "../RaspPiMMap/rasppimmap.h"
#endif


cDevBoot::cDevBoot(QObject *parent) :
    QObject(parent),
    mPortName(QString("ttyS0")),
    mAddress(0),
    mLoadFromURL(false),
    mForce(false),
    mFileName(QString("%1/fw.bin").arg("/home/pi")),
    mUrl(QString("http://rplab.ru/~ozhegov/ControlUnit4/Bin/Firmware/")),
    mDevType(QString()),
    mFirmwareVersion(QString())
{

}

QString cDevBoot::getPortName() const
{
    return mPortName;
}

void cDevBoot::setPortName(const QString &portName)
{
    mPortName = portName;
}

int cDevBoot::address() const
{
    return mAddress;
}

void cDevBoot::setAddress(int address)
{
    mAddress = address;
}

bool cDevBoot::isLoadFromURL() const
{
    return mLoadFromURL;
}

void cDevBoot::setLoadFromURL(bool loadFromURL)
{
    mLoadFromURL = loadFromURL;
}

QString cDevBoot::fileName() const
{
    return mFileName;
}

void cDevBoot::setFileName(const QString &fileName)
{
    mFileName = fileName;
}

QString cDevBoot::getUrl() const
{
    return mUrl;
}

void cDevBoot::setUrl(const QString &url)
{
    mUrl = url;
}

bool cDevBoot::isForce() const
{
    return mForce;
}

void cDevBoot::setForce(bool force)
{
    mForce = force;
}

QString cDevBoot::devType() const
{
    return mDevType;
}

void cDevBoot::setDevType(const QString &value)
{
    mDevType = value;
}

QString cDevBoot::fwVersion() const
{
    return mFirmwareVersion;
}

void cDevBoot::setFwVersion(const QString &value)
{
    mFirmwareVersion = value;
}

void cDevBoot::startProcess()
{

    if (isLoadFromURL()){
        prepareVersionTypeMaps();
        prepareOptions();
    }

    disableAllDEvices();

    bootloaderStart();
}

QStringList cDevBoot::getFileList(QUrl url)
{
    QNetworkAccessManager manager;
    manager.setProxy(QNetworkProxy::NoProxy);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    while (!reply->isFinished())
        qApp->processEvents();
    QString str = QString(reply->readAll());

    QStringList list;
    int i = 0;
    while (i<str.size()){
        i =  str.indexOf(QRegularExpression(QString("a href=.*</a>")), i+1);
        if (i == -1) break;
        i+=8;
        int j = str.indexOf("</a>",i);
        QStringList tmpList = str.mid(i,j-i).split("\">");
        if ((tmpList.size() == 2) && (tmpList[0] == tmpList[1]))
            list<<tmpList[0];
    }
    return list;
}

void cDevBoot::downloadFile(QUrl url)
{
    qDebug()<<"downloadFile";
    QNetworkAccessManager manager;
    manager.setProxy(QNetworkProxy::NoProxy);

    QNetworkRequest request(url);
    QNetworkReply *reply = manager.get(request);
    while (!reply->isFinished())
        qApp->processEvents();
    QByteArray data = reply->readAll();
    QFile saveFile(mFileName);
    if (saveFile.open(QIODevice::WriteOnly)){
        saveFile.write(data);
    }
    saveFile.close();

}

bool cDevBoot::compareVersion(const QString &str1, const QString &str2)
{
    QStringList strL1 = str1.split(".");
    QStringList strL2 = str2.split(".");
    for (int i = 0; i < qMin(strL1.size(), strL2.size()); ++i){
        if (strL1[i]!=strL2[i])
            return strL1[i].toInt()>strL2[i].toInt();
    }
    return strL1.size()>strL2.size();

}

void cDevBoot::prepareVersionTypeMaps()
{
    qDebug()<<"prepareVersionTypeMaps";
    // Строим карту всевозможных версий-типов
    QStringList list = getFileList(mUrl);
    foreach (QString tmpStr, list){
        tmpStr.chop(1);
        QStringList tmpStrList = getFileList(QString("%1%2/").arg(mUrl).arg(tmpStr));
        for (int i = 0; i < tmpStrList.size(); ++i){
            if (tmpStrList[i].contains("_code.bin")){
                tmpStrList[i].chop(9);
                mVersionType[tmpStr]<<tmpStrList[i];
            }
        }
    }
    // преобразуем карту в другом направлении: тип-версия
    foreach (QString version, mVersionType.keys()){
        foreach (QString Type, mVersionType[version])
            mTypeVersion[Type]<<version;
    }
}

void cDevBoot::enumerateDevice()
{
    //автоопределение типа устросйтва
    AbstractDriver aDriver;
    cuRs485IOInterface ioInterface;
    ioInterface.setPortName(mPortName);
    aDriver.setIOInterface(&ioInterface);
    aDriver.setDevAddress(mAddress);
    bool ok;
    QString tmpType = aDriver.getDeviceType()->getValueSequence(&ok, 5);
    if (!ok){
        qDebug()<<"Fatal error!!! Can't connect to device!!!";
        exit(1);
    }
    if (!mDevType.isEmpty()){
        if (mDevType!=tmpType){
            qDebug()<<"WARNING!!! Device type with option -d not equal to current device type";
            if (!mForce){
                qDebug()<<"For force update device plese use options -f";
                exit(2);
            }
        }
    }
    else mDevType = tmpType;
}

bool cDevBoot::isDeviceTypeCorrect()
{
    if (!mDevType.isEmpty()){
        qDebug()<<"devType"<<mDevType;
        return mTypeVersion.keys().contains(mDevType);
    }
    return false;
}

bool cDevBoot::isFirmwareVersionCorrect()
{
    // проверяем наличие версии файла
    if (!mFirmwareVersion.isEmpty()){
        bool error = !mVersionType.keys().contains(mFirmwareVersion);
        if (error) {
            qDebug()<<"WARNING! Wrong version of firmware";
            if (mForce){
                qDebug()<<"Last available version will be used.";
                mFirmwareVersion = QString();
                return true;
            }
            else{
                qDebug()<<"For force update device plese use options -f";
                return false;
            }
        }
    }
    return true;
}

bool cDevBoot::isFirmwareVersionAndTypeCorrectTogether()
{
    if ((!mDevType.isEmpty()) && (!mFirmwareVersion.isEmpty())){
        bool error = !mTypeVersion[mDevType].contains(mFirmwareVersion);
        if (error){
            qDebug()<<"WARNING! Wrong version and type of device. There is no suitable file.";
            if (mForce){
                qDebug()<<"Last available version will be used.";
                mFirmwareVersion = QString();
                return true;
            }
            else{
                qDebug()<<"For force update device plese use options -f";
                return false;
            }

        }
    }
    return true;
}

void cDevBoot::prepareOptions()
{
    enumerateDevice();

    if (!isDeviceTypeCorrect()){
        qDebug()<<"WARNING! Wrong type of device";
        exit(3);
    }

    if (!isFirmwareVersionCorrect()){
        exit(4);
    }

    if (!isFirmwareVersionAndTypeCorrectTogether()){
        exit(5);
    }


    if (mFirmwareVersion.isEmpty()){
        // выбираем последнюю доступную версию нашего файла.
        qSort(mTypeVersion[mDevType].begin(), mTypeVersion[mDevType].end(), cDevBoot::compareVersion);
        mFirmwareVersion = mTypeVersion[mDevType][0];
    }
    qDebug()<<"mDevType"<<mDevType;
    qDebug()<<"fwVersion"<<mFirmwareVersion;

    downloadFile(QString("%1%2/%3_code.bin")
                 .arg(mUrl)
                 .arg(mFirmwareVersion)
                 .arg(mDevType));
}

void cDevBoot::disableAllDEvices()
{
    qDebug()<<"Disable all other devices";
    {
        AbstractDriver aDriver;
        cuRs485IOInterface ioInterface;
        ioInterface.setPortName(mPortName);
        aDriver.setIOInterface(&ioInterface);
        for (int j = 0; j < 2; ++j){ // Делаем на всякий случай 2 разика
            for (int i = 0; i < 32; ++i){
                if (i!=mAddress){
                    aDriver.setDevAddress(i);
                    aDriver.listeningOff();
                    fprintf(stderr, QString("\rprogress %1%%").arg((j*31+i)*100/62).toLatin1().data());
                }
            }
            QThread::msleep(100);
        }
        qDebug()<<"";
    }
}

void cDevBoot::bootloaderStart()
{
    qDebug()<<"Start Bootloader";
    Bootloader *mBootloader = new Bootloader(this);
    mBootloader->setPortName(mPortName);
    connect(mBootloader, SIGNAL(loaderProgressChanged(int)), SLOT(bootloaderProgressChanged(int)));
    connect(mBootloader, SIGNAL(loaderFinished()), SLOT(bootloaderFinished()));
    connect(mBootloader, SIGNAL(infoLoader(QString)), SLOT(bootloaderInfo(QString)));
    connect(mBootloader, SIGNAL(errorLoader(QString)), SLOT(bootloaderInfo(QString)));
    mBootloader->program(mFileName);
}

void cDevBoot::rebootAllDevices()
{
    qDebug()<<"Reboot";
#ifdef RASPBERRY_PI
    mmapGpio rpiGpio;
    rpiGpio.writePinLow(25);
    QThread::msleep(50);
    rpiGpio.writePinHigh(25);
    QThread::msleep(1000);
#endif
}

void cDevBoot::bootloaderProgressChanged(int progress)
{
    static int tp = 0;
    if (tp != progress){
        tp = progress;
        fprintf(stderr, QString("\rprogress %1%%").arg(progress).toLatin1().data());
        if (tp == 100)
            qDebug()<<"";
    }
}

void cDevBoot::bootloaderFinished()
{
    qDebug()<<"Done";
    rebootAllDevices();

    if (mLoadFromURL){
        QFile::remove(mFileName);
    }
    exit(0);
}

void cDevBoot::bootloaderInfo(QString str)
{
    qDebug()<<str;
}




