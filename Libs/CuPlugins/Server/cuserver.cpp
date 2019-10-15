#include "cuserver.h"
#include "star_prc_commands.h"
#include <QCoreApplication>
#include "../Drivers/adriver.h"
#include "../Drivers/ccu4sdm0driver.h"
#include "../Drivers/ccu4tdm0driver.h"
#include <QSettings>
#include <QTime>
#include <QElapsedTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkProxy>

using namespace cmd;

cuServer::cuServer(QObject *parent)
    : cuIODeviceImpl(parent)
    , mTcpServer(new QTcpServer(this))
    , mDataReadyMapper(new QSignalMapper(this))
    , mDestroySocketMapper(new QSignalMapper(this))
    , mTimer(new QTimer(this))
{
    mTcpServer->setProxy(QNetworkProxy::NoProxy);
    
    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    
    connect(mDataReadyMapper, SIGNAL(mapped(QObject*)), this, SLOT(dataReady(QObject*)));
    connect(mDestroySocketMapper, SIGNAL(mapped(QObject*)), this, SLOT(destroySocket(QObject*)));
    
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(timeOut()));
}

cuServer::~cuServer()
{
    foreach (SocketData data, mSocketsData) {
        data.first->disconnect();
    }
    mSocketsData.clear();
}

void cuServer::initialize()
{
    if (!mTcpServer->isListening()){
        mTcpServer->listen(QHostAddress::Any, SERVER_TCPIP_PORT);
    }
}

void cuServer::initializeDeviceList()
{
    readDeviceInformation();
    // Проверка предустановленных устройств:
    // пытаемся подключить все устройства и проверить идентификационные данные
    checkDevicesInformation();
    
    qSort(mAvailableDevices.begin(), mAvailableDevices.end(), cuServer::sortInfo);
}

void cuServer::clearDeviceList()
{
    mAvailableDevices.clear();
}

bool cuServer::pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    //Обработчик ответов по внутреннему интерфейсу
    //    qDebug()<<"pMsgReceived: "<<QTime::currentTime().toString("hh:mm:ss.zzz");
    
    switch (command){
    // для ряда команд НИЧЕГО НЕ ДЕЛАЕМ
    // в качестве таких команд оставлены - получение идентификационных данных и описание устройства
    // Эти команды сервер вызывает при инициализации или при настройке, для этого используется временная
    // переменная Абстрактного Драйвера, во время работы устройства, здесь ничего не делаем
    case G_GetDeviceId:
    case G_GetDeviceType:
    case G_GetModVersion:
    case G_GetHwVersion:
    case G_GetFwVersion:
    case G_GetDeviceDescription:
        break;
    default:
        // получили ответ от устройства
        mTimer->stop();
        mWaitingForAnswer = false;
        if (!isSCPICommand) sendAnswer(address, command, dataLength, data);
    }
    return true;
}
void cuServer::newConnection()
{
    qDebug()<<"New Connection. Total connections:"<<(mSocketsData.size()+1);
    if (!mTcpServer->hasPendingConnections()) return;
    
    QTcpSocket *socket = mTcpServer->nextPendingConnection();
    mSocketsData.append(QPair<QTcpSocket *, QByteArray>(socket, QByteArray()));
    
    mDataReadyMapper->setMapping(socket, socket);
    mDestroySocketMapper->setMapping(socket, socket);
    
    connect(socket, SIGNAL(readyRead()), mDataReadyMapper, SLOT(map()));
    connect(socket, SIGNAL(disconnected()), mDestroySocketMapper, SLOT(map()));
}

void cuServer::dataReady(QObject *sender)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender);
    if (!socket) return;
    
    for (int i = 0; i<mSocketsData.count(); ++i ){
        if (mSocketsData[i].first == socket){
            mSocketsData[i].second.append(socket->readAll());
            break;
        }
    }
    sendSocketsDataToDevices();
}

void cuServer::destroySocket(QObject *sender)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender);
    if (!socket) return;
    
    foreach (SocketData data, mSocketsData) {
        if (data.first == socket){
            if (mLastSocket == data.first)
                mLastSocket = nullptr;
            mSocketsData.removeOne(data);
            socket->deleteLater();
        }
    }
    qDebug()<<"Destroy Socket. Total connections:"<<(mSocketsData.size());
    
}

void cuServer::timeOut()
{
    mWaitingForAnswer = false;
    sendSocketsDataToDevices();
}

/// SCPI commands
void cuServer::systemRequest(QString str, QString params)
{
    /** формат строки
     * SYSTem:DEViceList? - device list
     * SYSTem:DEViceList:ADD <N> - добавить новый, N - адрес
     * SYSTem:DEViceList:FIND - поиск новых устройств. Очень долгая команда
     * SYSTem:DEViceList:SAVE - сохранить список устройств
     */
    QStringList strList = str.split(":");
    if (strList[1].indexOf("DEVL")!=0) {
        sendString("ERROR\r\n");
        return;
    }
    
    if (strList[1] == "DEVL?"){
        QString tmpStr;
        tmpStr.clear();
        tmpStr.append(QString("DevCount: %1\r\n").arg(mAvailableDevices.count()));
        for (int i = 0; i < mAvailableDevices.count(); ++i){
            tmpStr.append(";<br>");
            tmpStr.append(QString("Dev%1: address=%2: type=%3\r\n")
                          .arg(i)
                          .arg(mAvailableDevices[i].devAddress)
                          .arg(mAvailableDevices[i].devType));
        }
        sendString(tmpStr);
        return;
    }
    
    if (strList.size()!=3){
        sendString("ERROR\r\n");
        return;
    }
    
    if (strList[2] == "SAVE"){
        sendString("OK\r\n");
        saveDeviceInformation();
        return;
    }
    
    if (strList[2] == "FIND"){
        sendString("Please waiting! This operation way take several minutes!!!/r/n");
        globalDeviceSearch();
        sendString(QString("Was founded %1 devices/r/n").arg(mAvailableDevices.count()));
        return;
    }
    
    if (strList[2].indexOf("ADD") == 0){
        quint8 address = static_cast<quint8>(params.toInt());
        sendString(addDevice(address)? "OK\r\n" : "ERROR\r\n");
        return;
    }
    
    sendString("UNKNOWN COMMAND\r\n");
}

void cuServer::generalRequest(QString str, QString params)
{
    Q_UNUSED(params);
    
    /** формат строки
     * GENeral:DEVice<N>:INIT - инициализация устройства
     * GENeral:DEVice<N>:PARKing - парковка устройства перед его отключением
     * GENeral:DEVice<N>:ListeningOFF - устройство перестает откликаться на команды, далее работает после перезагрузки
     * GENeral:DEVice<N>:STATus? - получение статуса устройства
     * GENeral:DEVice<N>:ERRor? - получение последней ошибки
     * GENeral:DEVice<N>:DeviceID? - получение уникального идентификатора устройства
     * GENeral:DEVice<N>:DeviceTYPe? - получение типа устройства
     * GENeral:DEVice<N>:DeviceDEScription? - получение полного описания устройства
     * GENeral:DEVice<N>:MODificationVersion? - получение номера модификации устройства
     * GENeral:DEVice<N>:HardWareVersion? - получение версии железа
     * GENeral:DEVice<N>:FirmWareVersion? - получение версии программного обеспечения
     * GENeral:DEVice<N>:reBOOT - перезагрузка устройства
     * GENeral:DEVice<N>:WriteEEProm - запись основных констант в память
     */
    QStringList strList = str.split(':');
    if (strList[1].indexOf("DEV")!=0){
        sendString("ERROR\r\n");
        return;
    }
    
    bool ok;
    
    quint8 address = static_cast<quint8>(strList[1].remove(0,3).toInt(&ok));
    
    if (!ok){
        sendString("ERROR\r\n");
        return;
    }
    
    deviceInfo* dInfo = currentDevice(address);
    if (!dInfo){
        sendString("Error: wrong address\r\n");
        return;
    }
    
    AbstractDriver driver;
    
    driver.setIOInterface(iOInterface());
    driver.setDevAddress(address);
    
    
    if (strList[2] == "INIT"){
        driver.init();
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "PARK"){
        driver.goToPark();
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "LOFF"){
        driver.listeningOff();
        sendString("OK\r\n");
        return;
    }
    if (strList[2] == "STAT?"){ //пока пустышка
        driver.getStatus();
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "ERR?"){ //пока пустышка
        driver.getLastError();
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "DID?"){
        if (dInfo)
            sendString(QString("%1\r\n").arg(dInfo->devUDID.toString()));
        else
            sendString("Error: wrong address\r\n");
        return;
    }
    if (strList[2] == "DTYP?"){
        sendString(QString("%1\r\n").arg(dInfo->devType));
        return;
    }
    if (strList[2] == "DDES?"){
        sendString(QString("%1\r\n").arg(dInfo->devDescription));
        return;
    }
    if (strList[2] == "MODV?"){
        sendString(QString("%1\r\n").arg(dInfo->devModVersion));
        return;
    }
    if (strList[2] == "HWV?"){
        sendString(QString("%1\r\n").arg(dInfo->devHwVersion));
        return;
    }
    if (strList[2] == "FWV?"){
        sendString(QString("%1\r\n").arg(dInfo->devFwVersion));
        return;
    }
    if (strList[2] == "BOOT"){
        driver.rebootDevice();
        sendString("OK\r\n");
        return;
    }
    if (strList[2] == "WEEP"){
        driver.writeEeprom();
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    sendString("UNKNOWN COMMAND\r\n");
}

void cuServer::sspdRequest(QString str, QString params)
{
    /** формат строки
     * SSPD:DEVice<N>:DATA? - получение всех данных
     * SSPD:DEVice<N>:CURRent? - получение тока
     * SSPD:DEVice<N>:CURRent <Value/Float> - установка тока
     * SSPD:DEVice<N>:VOLTage? - получение напряжения
     * SSPD:DEVice<N>:COUNts? - получение числа отсчетов
     * SSPD:DEVice<N>:STATus? - получение статуса устройства
     * SSPD:DEVice<N>:STATus <Value|Byte> - установка статуса устройства
     *
     * Атомарные функции чтения статуса
     * SSPD:DEVice<N>:SHORt? - закоротка
     * SSPD:DEVice<N>:AMPlifierEnable? - включен ли усилитель
     * SSPD:DEVice<N>:RFKeytoComparatorenable? - переключение усилителя на компаратор
     * SSPD:DEVice<N>:ComparatorLatchEnable? - включен ли компаратор
     * SSPD:DEVice<N>:COUnterEnable? - включен ли счетчик
     * SSPD:DEVice<N>:AutoRestoreEnable? - включена ли функция автовосстановления тока
     *
     * Атомарные функции записи статуса
     * SSPD:DEVice<N>:SHORt <Value|Boolean> - установка закоротки
     * SSPD:DEVice<N>:AMPlifierEnable <Value|Boolean> - включение усилителя
     * SSPD:DEVice<N>:RFKeytoComparatorenable <Value|Boolean> - переключение выхода усилителя на компаратор
     * SSPD:DEVice<N>:ComparatorLatchEnable <Value|Boolean> - включение компаратора
     * SSPD:DEVice<N>:COUnterEnable <Value|Boolean> - включение счетчика
     * SSPD:DEVice<N>:AutoRestoreEnable <Value|Boolean> - включение авто восстановления рабочей точки
     *
     * SSPD:DEVice<N>:PARAms? - получение параметров устройства
     * SSPD:DEVice<N>:PARAms <Value|JSON> - установка параметров устройства
     * SSPD:DEVice<N>:CoMParatorReference? - получение уровня компарации устройства
     * SSPD:DEVice<N>:CoMParatorReference <Value|Float> - установка уровня компарации устройства
     * SSPD:DEVice<N>:TIMeConst? - получение постоянной времени счетчика
     * SSPD:DEVice<N>:TIMeConst <Value|Float> - установка постоянной времени счетчика
     * SSPD:DEVice<N>:AutoRestoreTHreshold? - получение напряжения срабатывания системы автовосстановления
     * SSPD:DEVice<N>:AutoRestoreTHreshold <Value|Float> - установка напряжения срабатывания системы автовосстановления
     * SSPD:DEVice<N>:AutoRestoreTimeOut? - получение постоянной времени системы автовосстановления
     * SSPD:DEVice<N>:AutoRestoreTimeOut <Value|Float> - установка постоянной времени системы автовосстановления
     * SSPD:DEVice<N>:AutoRestoreCOunts? - получение числа срабатываний системы автовосстановления
     * SSPD:DEVice<N>:AutoRestoreCOunts <Value|Float> - сброс числа срабатываний системы автовосстановления
     * SSPD:DEVice<N>:EEPRomconst? - получение коэффициентов из постоянной памяти
     * SSPD:DEVice<N>:EEPRomconst <Value|JSON> - установка коэффициентов из постоянной памяти
     * SSPD:DEVice<N>:CurrentADCcoeff? - получение коэффициентов
     * SSPD:DEVice<N>:CurrentADCcoeff <Value|JSON> - установка коэффициентов
     * SSPD:DEVice<N>:VoltageADCcoeff? - получение коэффициентов
     * SSPD:DEVice<N>:VoltageADCcoeff <Value|JSON> - установка коэффициентов
     * SSPD:DEVice<N>:CurrentDACcoeff? - получение коэффициентов
     * SSPD:DEVice<N>:CurrentDACcoeff <Value|JSON> - установка коэффициентов
     * SSPD:DEVice<N>:CoMParatorCoeff? - получение коэффициентов
     * SSPD:DEVice<N>:CoMParatorCoeff <Value|JSON> - установка коэффициентов
     */
    
    QStringList strList = str.split(':');
    if (strList[1].indexOf("DEV")!=0){
        sendString("ERROR\r\n");
        return;
    }
    
    bool ok;
    
    quint8 address = static_cast<quint8>(strList[1].remove(0,3).toInt(&ok));
    
    if (!ok){
        sendString("ERROR\r\n");
        return;
    }
    
    deviceInfo* dInfo = currentDevice(address);
    if (!dInfo){
        sendString("Error: wrong address\r\n");
        return;
    }
    
    cCu4SdM0Driver driver;
    
    driver.setIOInterface(iOInterface());
    driver.setDevAddress(address);
    
    if (strList[2]=="DATA?"){
        CU4SDM0V1_Data_t data = driver.deviceData()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Current"]  = static_cast<double>(data.Current);
            jsonObj["Voltage"]  = static_cast<double>(data.Voltage);
            jsonObj["Counts"]   = static_cast<double>(data.Counts);
            jsonObj["Status"]   = static_cast<double>(data.Status.Data);
            QJsonDocument jsonDoc(jsonObj);
            sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2]=="CURR?"){
        double data = static_cast<double>(driver.current()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CURR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.current()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2]=="VOLT?"){
        double data = static_cast<double>(driver.voltage()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2]=="COUN?"){
        double data = static_cast<double>(driver.counts()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2]=="STAT?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).Data;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "STAT"){
        CU4SDM0_Status_t Status;
        Status.Data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.deviceStatus()->setValue(Status);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    
    if (strList[2] == "SHOR"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setShortEnable(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    else if (strList[2]  == "SHOR?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stShorted;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    if (strList[2] == "AMPE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setAmpEnable(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    else if (strList[2] == "AMPE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stAmplifierOn;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    if (strList[2] == "RFKC"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setRfKeyEnable(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    else if (strList[2] == "RFKC?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stRfKeyToCmp;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    if (strList[2] == "CLE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setCmpLatchEnable(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    else if (strList[2] == "CLE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stComparatorOn;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    if (strList[2] == "COUE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setCounterEnable(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    else if (strList[2] == "COUE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stCounterOn;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    
    if (strList[2] == "ARE"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.setAutoResetEnable(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else sendString("ERROR\r\n");
        return;
    }
    else if (strList[2] == "ARE?"){
        quint8 data = driver.deviceStatus()->getValueSequence(&ok,5).stAutoResetOn;
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    if (strList[2] == "PARA?"){
        CU4SDM0V1_Param_t data = driver.deviceParams()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["AutoResetCounts"]      = static_cast<int>(data.AutoResetCounts);
            jsonObj["AutoResetThreshold"]   = static_cast<double>(data.AutoResetThreshold);
            jsonObj["AutoResetTimeOut"]     = static_cast<double>(data.AutoResetTimeOut);
            jsonObj["Cmp_Ref_Level"]        = static_cast<double>(data.Cmp_Ref_Level);
            jsonObj["Time_Const"]           = static_cast<double>(data.Time_Const);
            QJsonDocument jsonDoc(jsonObj);
            sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "PARA"){
        //For example SSPD:DEV0:PARA\s{"AutoResetCounts":1,"AutoResetThreshold":0.2,"AutoResetTimeOut":0,"Cmp_Ref_Level":2,"Time_Const":0}\r\n
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM0V1_Param_t data;
        data.AutoResetCounts    = static_cast<quint32>(jsonObj["AutoResetCounts"].toInt());
        data.AutoResetThreshold = static_cast<float>(jsonObj["AutoResetThreshold"].toDouble());
        data.AutoResetTimeOut   = static_cast<float>(jsonObj["AutoResetTimeOut"].toDouble());
        data.Cmp_Ref_Level      = static_cast<float>(jsonObj["Cmp_Ref_Level"].toDouble());
        data.Time_Const         = static_cast<float>(jsonObj["Time_Const"].toDouble());
        driver.deviceParams()->setValue(data);
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "CMPR?"){
        double data = static_cast<double>(driver.cmpReferenceLevel()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CMPR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.cmpReferenceLevel()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2] == "TIMC?"){
        double data = static_cast<double>(driver.timeConst()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "TIMC"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.timeConst()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2] == "ARTH?"){
        double data = static_cast<double>(driver.autoResetThreshold()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "ARTH"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetThreshold()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2] == "ARTO?"){
        double data = static_cast<double>(driver.autoResetTimeOut()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "ARTO"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.autoResetTimeOut()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2] == "ARCO?"){
        double data = static_cast<double>(driver.autoResetAlarmsCounts()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "ARCO"){
        unsigned int data = params.toUInt(&ok);
        if (ok) {
            driver.autoResetAlarmsCounts()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2] == "EEPR?"){
        CU4SDM0V1_EEPROM_Const_t data = driver.eepromConst()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            QJsonArray value;
            value.append(static_cast<double>(data.Current_ADC.first));
            value.append(static_cast<double>(data.Current_ADC.second));
            jsonObj["Current_ADC"]      = value;
            value[0] = static_cast<double>(data.Voltage_ADC.first);
            value[1] = static_cast<double>(data.Voltage_ADC.second);
            jsonObj["Voltage_ADC"]      = value;
            value[0] = static_cast<double>(data.Current_DAC.first);
            value[1] = static_cast<double>(data.Current_DAC.second);
            jsonObj["Current_DAC"]      = value;
            value[0] = static_cast<double>(data.Cmp_Ref_DAC.first);
            value[1] = static_cast<double>(data.Cmp_Ref_DAC.second);
            jsonObj["Cmp_Ref_DAC"]      = value;
            QJsonDocument jsonDoc(jsonObj);
            sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4SDM0V1_EEPROM_Const_t data;
        
        QJsonArray value = jsonObj["Current_ADC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Current_ADC value");
            return;
        }
        data.Current_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Current_ADC.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Voltage_ADC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Voltage_ADC value");
            return;
        }
        data.Voltage_ADC.first  = static_cast<float>(value[0].toDouble());
        data.Voltage_ADC.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Current_DAC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Current_DAC value");
            return;
        }
        data.Current_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Current_DAC.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Cmp_Ref_DAC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Cmp_Ref_DAC value");
            return;
        }
        data.Cmp_Ref_DAC.first  = static_cast<float>(value[0].toDouble());
        data.Cmp_Ref_DAC.second = static_cast<float>(value[1].toDouble());
        
        driver.eepromConst()->setValue(data);
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "CADC?"){
        pair_t<float> data = driver.currentAdcCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CADC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.currentAdcCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "VADC?"){
        pair_t<float> data = driver.voltageAdcCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "VADC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.voltageAdcCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "CDAC?"){
        pair_t<float> data = driver.currentDacCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CDAC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.currentDacCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "CMPC?"){
        pair_t<float> data = driver.cmpReferenceCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CMPC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.cmpReferenceCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    sendString("UNKNOWN COMMAND\r\n");
}

void cuServer::temperatureRequest(QString str, QString params)
{
    /** формат команды
     * TEMperatureDriver:DEVice<N>:DATA? - получение всех данных
     * TEMperatureDriver:DEVice<N>:TEMPerature? - получение температуры
     * TEMperatureDriver:DEVice<N>:PRESsure? - получение давления
     * TEMperatureDriver:DEVice<N>:CURRent? - получение тока смещения температурного сенсора
     * TEMperatureDriver:DEVice<N>:CURRent <Value/Float> - установка тока смещения температурного сенсора
     * TEMperatureDriver:DEVice<N>:VOLTage? - получение напряжения смещения температурного сенсора
     * TEMperatureDriver:DEVice<N>:PRessVoltagePositive? - получение напряжения с положительного контакта датчика давления
     * TEMperatureDriver:DEVice<N>:PRessVoltageNegative? - получение напряжения с негативного контакта датчика давления
     * TEMperatureDriver:DEVice<N>:THermometerON? - получение статуса термометра: включен/выключен
     * TEMperatureDriver:DEVice<N>:THermometerON <Value|Bool> - включение/выключение термометра
     * TEMperatureDriver:DEVice<N>:EEPRomconst? - получение констант
     * TEMperatureDriver:DEVice<N>:EEPRomconst <Value|JSON> - установка констант
     * TEMperatureDriver:DEVice<N>:CurrentADCcoeff? - получение констант для измерения величины тока смещения термометра
     * TEMperatureDriver:DEVice<N>:CurrentADCcoeff <Value|JSON> - установка констант для измерения величины тока смещения термометра
     * TEMperatureDriver:DEVice<N>:CurrentDACcoeff? - получение констант для установки величины тока смещения термометра
     * TEMperatureDriver:DEVice<N>:CurrentDACcoeff <Value|JSON> - установка констант для установки величины тока смещения термометра
     * TEMperatureDriver:DEVice<N>:VoltageADCcoeff? - получение констант для измерения напряжения смещения термометра
     * TEMperatureDriver:DEVice<N>:VoltageADCcoeff <Value|JSON> - установка констант для измерения напряжения смещения термометра
     * TEMperatureDriver:DEVice<N>:VoltagePressPositiveCoeff? - получение констант для измерения напряжения позитивного контакта датчика давления
     * TEMperatureDriver:DEVice<N>:VoltagePressPositiveCoeff <Value|JSON> - уствновка констант для измерения напряжения позитивного контакта датчика давления
     * TEMperatureDriver:DEVice<N>:VoltagePressNegativeCoeff? - получение констант для измерения напряжения негативного контакта датчика давления
     * TEMperatureDriver:DEVice<N>:VoltagePressNegativeCoeff <Value|JSON> - уствновка констант для измерения напряжения негативного контакта датчика давления
     * TEMperatureDriver:DEVice<N>:PRessSensorCoeff? - получение констант для измерения давления
     * TEMperatureDriver:DEVice<N>:PRessSensorCoeff <Value|JSON> - установка констант для измерения давления
     * TEMperatureDriver:DEVice<N>:TEMperatureTable? - получение температурной таблицы
     * TEMperatureDriver:DEVice<N>:TEMperatureTable <Value|JSON> - установка температурной таблицы
     */
    
    QStringList strList = str.split(':');
    if (strList[1].indexOf("DEV")!=0){
        sendString("ERROR\r\n");
        return;
    }
    
    bool ok;
    
    quint8 address = static_cast<quint8>(strList[1].remove(0,3).toInt(&ok));
    
    if (!ok){
        sendString("ERROR\r\n");
        return;
    }
    
    deviceInfo* dInfo = currentDevice(address);
    if (!dInfo){
        sendString("Error: wrong address\r\n");
        return;
    }
    
    cCu4TdM0Driver driver;
    
    driver.setIOInterface(iOInterface());
    driver.setDevAddress(address);
    
    if (strList[2] == "DATA?"){
        CU4TDM0V1_Data_t data = driver.deviceData()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            jsonObj["Temperature"]          = static_cast<double>(data.Temperature);
            jsonObj["Pressure"]             = static_cast<double>(data.Pressure);
            jsonObj["TempSensorCurrent"]    = static_cast<double>(data.TempSensorCurrent);
            jsonObj["TempSensorVoltage"]    = static_cast<double>(data.TempSensorVoltage);
            jsonObj["PressSensorVoltageP"]  = static_cast<double>(data.PressSensorVoltageP);
            jsonObj["PressSensorVoltageN"]  = static_cast<double>(data.PressSensorVoltageN);
            jsonObj["CommutatorOn"]         = static_cast<quint8>(data.CommutatorOn);
            QJsonDocument jsonDoc(jsonObj);
            sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "TEMP?"){
        double data = static_cast<double>(driver.temperature()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "PRES?"){
        double data = static_cast<double>(driver.pressure()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "CURR?"){
        double data = static_cast<double>(driver.tempSensorCurrent()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CURR"){
        float data = params.toFloat(&ok);
        if (ok) {
            driver.tempSensorCurrent()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    if (strList[2] == "VOLT?"){
        double data = static_cast<double>(driver.tempSensorVoltage()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "PRVP?"){
        double data = static_cast<double>(driver.pressSensorVoltageP()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "PRVN?"){
        double data = static_cast<double>(driver.pressSensorVoltageN()->getValueSequence(&ok));
        if (ok) sendString(QString("%1\r\n").arg(data));
        else sendString("Error: Timeout\r\n");
        return;
    }
    if (strList[2] == "THON?"){
        bool data = driver.commutatorOn()->getValueSequence(&ok);
        qDebug()<<data;
        if (ok) sendString(QString("%1\r\n").arg(data?1:0));
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "THON"){
        quint8 data = static_cast<quint8>(params.toInt(&ok));
        if (ok) {
            driver.commutatorOn()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");;
        }
        else sendString("ERROR\r\n");
        return;
    }
    
    if (strList[2] == "EEPR?"){
        CU4TDM0V1_EEPROM_Const_t data = driver.eepromConst()->getValueSequence(&ok);
        if (ok){
            QJsonObject jsonObj;
            QJsonArray value;
            value.append(static_cast<double>(data.pressSensorCoeffs.first));
            value.append(static_cast<double>(data.pressSensorCoeffs.second));
            jsonObj["Pressure"] = value;
            
            value[0] = static_cast<double>(data.tempSensorCurrentAdc.first);
            value[1] = static_cast<double>(data.tempSensorCurrentAdc.second);
            jsonObj["Current_ADC"] = value;
            
            value[0] = static_cast<double>(static_cast<double>(data.tempSensorCurrentDac.first));
            value[1] = static_cast<double>(static_cast<double>(data.tempSensorCurrentDac.second));
            jsonObj["Current_DAC"] = value;
            
            value[0] = static_cast<double>(data.tempSensorVoltage.first);
            value[1] = static_cast<double>(data.tempSensorVoltage.second);
            jsonObj["Voltage_ADC"] = value;
            
            value[0] = static_cast<double>(data.pressSensorVoltageP.first);
            value[1] = static_cast<double>(data.pressSensorVoltageP.second);
            jsonObj["Press_VoltageP_ADC"] = value;
            
            value[0] = static_cast<double>(data.pressSensorVoltageN.first);
            value[1] = static_cast<double>(data.pressSensorVoltageN.second);
            jsonObj["Press_VoltageN_ADC"] = value;
            
            QJsonDocument jsonDoc(jsonObj);
            sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "EEPR"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        CU4TDM0V1_EEPROM_Const_t data;
        
        QJsonArray value = jsonObj["Pressure"].toArray();
        if (value.size()!=2) {
            sendString("Error at Pressure value");
            return;
        }
        data.pressSensorCoeffs.first  = static_cast<float>(value[0].toDouble());
        data.pressSensorCoeffs.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Current_ADC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Current_ADC value");
            return;
        }
        data.tempSensorCurrentAdc.first  = static_cast<float>(value[0].toDouble());
        data.tempSensorCurrentAdc.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Current_DAC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Current_DAC value");
            return;
        }
        data.tempSensorCurrentDac.first  = static_cast<float>(value[0].toDouble());
        data.tempSensorCurrentDac.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Voltage_ADC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Voltage_ADC value");
            return;
        }
        data.tempSensorVoltage.first    = static_cast<float>(value[0].toDouble());
        data.tempSensorVoltage.second   = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Press_VoltageP_ADC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Press_VoltageP_ADC value");
            return;
        }
        data.pressSensorVoltageP.first  = static_cast<float>(value[0].toDouble());
        data.pressSensorVoltageP.second = static_cast<float>(value[1].toDouble());
        
        value = jsonObj["Press_VoltageN_ADC"].toArray();
        if (value.size()!=2) {
            sendString("Error at Press_VoltageN_ADC value");
            return;
        }
        data.pressSensorVoltageN.first  = static_cast<float>(value[0].toDouble());
        data.pressSensorVoltageN.second = static_cast<float>(value[1].toDouble());
        
        driver.eepromConst()->setValue(data);
        if (driver.waitingAnswer()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    
    if (strList[2] == "CADC?"){
        pair_t<float> data = driver.tempSensorCurrentAdcCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CADC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.tempSensorCurrentAdcCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2]=="CDAC?"){
        pair_t<float> data = driver.tempSensorCurrentDacCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "CDAC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.tempSensorCurrentDacCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "VADC?"){
        pair_t<float> data = driver.tempSensorVoltageCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "VADC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.tempSensorVoltageCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "VPPC?"){
        pair_t<float> data = driver.pressSensorVoltagePCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "VPPC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.pressSensorVoltagePCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "VPNC?"){
        pair_t<float> data = driver.pressSensorVoltageNCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "VPNC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.pressSensorVoltageNCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "PRSC?"){
        pair_t<float> data = driver.pressSensorCoeff()->getValueSequence(&ok);
        if (ok){
            sendPairFloat(data);
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "PRSC"){
        pair_t<float> data;
        bool ok = getPairFromJsonString(params, &data);
        if (ok){
            driver.pressSensorCoeff()->setValue(data);
            if (driver.waitingAnswer()) sendString("OK\r\n");
            else sendString("Error: Timeout\r\n");
        }
        else {
            sendString("Error: Format data\r\n");
        }
        return;
    }
    if (strList[2] == "TEMT?"){
        ok = driver.receiveTempTable();
        if (ok){
            CU4TDM0V1_Temp_Table_Item_t* tempTable = driver.tempTable();
            QJsonObject jsonObj;
            QJsonArray table;
            for (int i = 0; i<TEMP_TABLE_SIZE; ++i){
                QJsonArray value;
                value.append(static_cast<double>(tempTable[i].Temperature));
                value.append(static_cast<double>(tempTable[i].Voltage));
                table.append(value);
            }
            jsonObj["TempTable"] = table;
            QJsonDocument jsonDoc(jsonObj);
            sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
        }
        else sendString("Error: Timeout\r\n");
        return;
    }
    else if (strList[2] == "TEMT"){
        QJsonDocument jsonDoc(QJsonDocument::fromJson(params.toUtf8()));
        QJsonObject jsonObj = jsonDoc.object();
        QJsonArray table = jsonObj["TempTable"].toArray();
        CU4TDM0V1_Temp_Table_Item_t* tempTable = driver.tempTable();
        for (int i = 0; i<TEMP_TABLE_SIZE; ++i){
            QJsonArray value = table[i].toArray();
            tempTable[i].Temperature    = static_cast<float>(value[0].toDouble());
            tempTable[i].Voltage        = static_cast<float>(value[1].toDouble());
        }
        if (driver.sendTempTable()) sendString("OK\r\n");
        else sendString("Error: Timeout\r\n");
        return;
    }
    sendString("UNKNOWN COMMAND\r\n");
}

/**
 * @brief cuServer::serverRequest
 * Обработка команд серверу
 * @param packet
 */
void cuServer::serverRequest(const QByteArray &packet)
{
    // команда серверу
    // здесь будет обработка его команд
    //    qDebug()<<"Server request";
    quint8 command = static_cast<quint8>(packet[1]);
    const char* data = packet.data() + 3;
    
    switch (command) {
    // полный список всех доступных устройств
    case CMD_SERVER_GET_DEVICE_LIST:
    {
        QString tmpStr;
        tmpStr.clear();
        tmpStr.append(QString("DevCount: %1").arg(mAvailableDevices.count()));
        for (int i = 0; i < mAvailableDevices.count(); ++i){
            tmpStr.append(";<br>");
            tmpStr.append(QString("Dev%1: address=%2: type=%3")
                          .arg(i)
                          .arg(mAvailableDevices[i].devAddress)
                          .arg(mAvailableDevices[i].devType));
        }
        sendAnswer(SERVER_ADDRESS, command, static_cast<quint8>(tmpStr.count()), (quint8*) tmpStr.toLocal8Bit().data());
        break;
    }
    case CMD_SERVER_SEARCH_DEVICES:
    {
        globalDeviceSearch();
        quint8 tmpUint8 = static_cast<quint8>(mAvailableDevices.count());
        sendAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    case CMD_SERVER_ADD_DEVICE:
    {
        quint8 tmpUint8 = static_cast<quint8>(addDevice(*data));
        sendAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        break;
    }
    case CMD_SERVER_SAVE_DEVICE_LIST:
    {
        quint8 tmpUint8 = 1;
        sendAnswer(SERVER_ADDRESS, command, 1, &tmpUint8);
        saveDeviceInformation();
        break;
    }
    default:
        break;
    }
}

void cuServer::deviceRequest(deviceInfo *info, QByteArray &packet)
{
    quint8 address = packet[0];
    quint8 command = packet[1];
    quint8 dataLength = packet[2];
    quint8* data = ((quint8*)packet.data()) + 3;
    switch (command) {
    case G_GetDeviceId:
        sendAnswer(address, command,
                   sizeof(cUDID),
                   info->devUDID.UDID());
        break;
    case G_GetDeviceType:
        sendAnswer(address, command,
                   info->devType.length(),
                   (quint8*) info->devType.toLocal8Bit().data());
        break;
    case G_GetModVersion:
        sendAnswer(address, command,
                   info->devModVersion.length(),
                   (quint8*) info->devModVersion.toLocal8Bit().data());
        break;
    case G_GetHwVersion:
        sendAnswer(address, command,
                   info->devHwVersion.length(),
                   (quint8*) info->devHwVersion.toLocal8Bit().data());
        break;
    case G_GetFwVersion:
        sendAnswer(address, command,
                   info->devFwVersion.length(),
                   (quint8*) info->devFwVersion.toLocal8Bit().data());
        break;
    case G_GetDeviceDescription:
        sendAnswer(address, command,
                   info->devDescription.length(),
                   (quint8*) info->devDescription.toLocal8Bit().data());
        break;
        
    default:
        // А вот тут отправляем данные по внутреннему интерфейсу
        mWaitingForAnswer = true;
        mTimer->start(mServerTimeOut);
        iOInterface()->sendMsg(address, command, dataLength, data);
        break;
    }
    
}

// Отсылаем данные по TcpIp
void cuServer::sendAnswer(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    if (mLastSocket && mLastSocket->isValid())
        if (mLastSocket->isOpen() && (mLastSocket->state() == QAbstractSocket::ConnectedState) && (!isSCPICommand)){
            qDebug()<<"sendAnswer: "<<QTime::currentTime().toString("hh:mm:ss.zzz");
            qDebug()<<"TcpSocket: "<<mLastSocket->peerAddress().toString();
            qDebug()<<"TcpSocket SocketState: "<<mLastSocket->state();
            QByteArray ba;
            ba.append(address);
            ba.append(command);
            ba.append(dataLength);
            ba.append((char*) data, dataLength);
            mLastSocket->write(ba);
        }
}

void cuServer::sendString(QString str)
{
    if (mLastSocket && mLastSocket->isValid())
        if (mLastSocket->isOpen()){
            mLastSocket->write(str.toLocal8Bit());
            mLastSocket->flush();
        }
}

int cuServer::serverTimeOut() const
{
    return mServerTimeOut;
}

void cuServer::setServerTimeOut(int serverTimeOut)
{
    mServerTimeOut = serverTimeOut;
}

bool cuServer::sortInfo(const deviceInfo &dInfo1, const deviceInfo &dInfo2)
{
    return dInfo1.devAddress < dInfo2.devAddress;
}

bool cuServer::isDataReady(const QByteArray &ba)
{
    if (ba.size() >= 3)
        if (ba.size() >= ba[2] + 3)
            return true;
    return false;
}

deviceInfo *cuServer::currentDevice(quint8 address)
{
    for (int i = 0; i< mAvailableDevices.count(); ++i){
        if (mAvailableDevices[i].devAddress == address)
            return &mAvailableDevices[i];
    }
    return nullptr;
}

/**
 * @brief cuServer::addDevice
 * Добавление нового устройства в список устройств
 * @param address
 * @return
 */
bool cuServer::addDevice(quint8 address)
{
    qDebug()<<"add device:"<< address;
    while (mWaitingForAnswer) qApp->processEvents();
    
    // проверяем есть ли в availableDevices
    // удаляем устройства с таким же адресом
    int i = 0;
    while (i < mAvailableDevices.count()){
        if (mAvailableDevices[i].devAddress == address) {
            mAvailableDevices.removeAt(i);
        }
        else ++i;
    }
    
    AbstractDriver driver;
    
    driver.setIOInterface(iOInterface());
    driver.setDevAddress(address);
    
    deviceInfo newDevice;
    newDevice.devAddress = address;
    
    bool ok = false;
    i = 0;
    QString tmp = driver.getDeviceType()->getValueSequence(&ok, 5);
    qDebug()<<"getDeviceType:"<< ok;

    if (!ok) return false;
    newDevice.devType = tmp;
    qDebug()<<tmp;
    
    newDevice.devUDID = driver.getUDID()->getValueSequence(&ok, 5);
    if (!ok) return false;
    
    newDevice.devModVersion = driver.getModificationVersion()->getValueSequence(&ok, 5);
    if (!ok) return false;
    
    newDevice.devHwVersion = driver.getHardwareVersion()->getValueSequence(&ok, 5);
    if (!ok) return false;
    
    newDevice.devFwVersion = driver.getFirmwareVersion()->getValueSequence(&ok, 5);
    if (!ok) return false;
    
    newDevice.devDescription = driver.getDeviceDescription()->getValueSequence(&ok, 5);
    if (!ok) return false;
    
    qDebug()<<"success";
    mAvailableDevices.append(newDevice);
    
    qSort(mAvailableDevices.begin(), mAvailableDevices.end(), cuServer::sortInfo);
    return true;
}

void cuServer::globalDeviceSearch()
{
    for (int i = 0; i < MAX_COUNT_DEVICES; ++i)
        addDevice(i);
}

void cuServer::saveDeviceInformation()
{
    // пишем данные в файла настроек
    QSettings settings("Scontel", "RaspPi Server");
    settings.beginWriteArray("devices");
    for (int i = 0; i < mAvailableDevices.count(); ++i){
        settings.setArrayIndex(i);
        settings.setValue("devAddress", mAvailableDevices[i].devAddress);
        settings.setValue("devType", mAvailableDevices[i].devType);
        settings.setValue("devUDID",QByteArray((const char *)mAvailableDevices[i].devUDID.UDID(),12));
        settings.setValue("devFwVersion",mAvailableDevices[i].devFwVersion);
        settings.setValue("devHwVersion",mAvailableDevices[i].devHwVersion);
        settings.setValue("devModVersion",mAvailableDevices[i].devModVersion);
        settings.setValue("devDescription",mAvailableDevices[i].devDescription);
    }
    settings.endArray();
}

void cuServer::readDeviceInformation()
{
    // читаем данные из файла настроек
    QSettings settings("Scontel", "RaspPi Server");
    int size = settings.beginReadArray("devices");
    mAvailableDevices.clear();
    for (int i = 0; i < size; ++i){
        deviceInfo device;
        settings.setArrayIndex(i);
        device.devAddress = settings.value("devAddress", 255).toInt();
        device.devType = settings.value("devType","None").toString();
        device.devDescription = settings.value("devDescription", "None").toString();
        device.devUDID.setUDID((quint8*)settings.value("devUDID","0").toByteArray().data());
        device.devFwVersion = settings.value("devFwVersion", "None").toString();
        device.devHwVersion = settings.value("devHwVersion", "None").toString();
        device.devModVersion = settings.value("devModVersion", "None").toString();
        mAvailableDevices.append(device);
    }
    settings.endArray();
    qDebug()<<"available devices:"<<mAvailableDevices.size();
}

void cuServer::checkDevicesInformation()
{
    int i = 0;
    while (i < mAvailableDevices.size()){
        if (!isDeviceInformationCorrect(mAvailableDevices[i])) mAvailableDevices.removeAt(i);
        else ++i;
    }
    qDebug()<<"available devices (after check device information):"<<mAvailableDevices.size();
}

bool cuServer::isDeviceInformationCorrect(deviceInfo info)
{
    AbstractDriver driver;
    driver.setIOInterface(iOInterface());
    driver.setDevAddress(info.devAddress);
    
    bool ok;
    cUDID UDID = driver.getUDID()->getValueSequence(&ok, 10); // крайне важная операция будем пробовать аж до 10 раз
    if (!ok) return false;

    // Проверяем Id устройства,
    // Более ничего не проверяем, считаем что UDID уникален и 2 устройства с одинаковым UDID не может быть
    // как следствие UDID точно соответствует его Type и т.д.

    if (info.devUDID.toString() == UDID.toString()) return true;
    
    return false;
}

void cuServer::sendPairFloat(pair_t<float> data)
{
    QJsonArray value;
    value.append(static_cast<double>(data.first));
    value.append(static_cast<double>(data.second));
    QJsonDocument jsonDoc(value);
    sendString(QString("%1\r\n").arg(QString(jsonDoc.toJson())));
}

bool cuServer::getPairFromJsonString(QString str, pair_t<float> * data)
{
    QJsonDocument jsonDoc(QJsonDocument::fromJson(str.toUtf8()));
    if (!jsonDoc.isArray()){
        return false;
    }
    QJsonArray value = jsonDoc.array();
    if (value.size()!=2){
        return false;
    }
    data->first  = static_cast<float>(value[0].toDouble());
    data->second = static_cast<float>(value[1].toDouble());
    return true;
}

void cuServer::sendSocketsDataToDevices()
{
    // Берем номер сокета в нашем списке, который подготовил данные для пересылки устройствам
    foreach (SocketData data, mSocketsData){
        if (!data.second.isEmpty()){
            
            mLastSocket = data.first;
            int idx = mSocketsData.indexOf(data);
            
            if (isDataReady(data.second)){
                /// проверка на несоответствие протоколу SCPI
                /// основная тонкость заключается в том, что в SCPI используется только текстовая информация
                /// а в обычном пакете данных первый символ - не соответствует текстовому символу, на этом и производим сверку протоколов
                /// делаем расшифровку команд
                
                QByteArray ba = mSocketsData[idx].second;
                mSocketsData[idx].second.remove(0, data.second[2] + 3);
                
                // смотрим адрес устройства
                if (((quint8)data.second[0]) == SERVER_ADDRESS){
                    // это запрос к серверу
                    isSCPICommand = false;
                    serverRequest(ba);
                }
                else {
                    // а это запрос к Устройству
                    deviceInfo *info = currentDevice(data.second[0]);
                    // если такого устройства совсем нет, то и не будем обрабатывать это устройство
                    if (info != nullptr){
                        isSCPICommand = false;
                        deviceRequest(info, ba);
                    }
                }
            }
            else{
                isSCPICommand = true;
                //копируем данные и вычищаем их из mSocketsData
                QString str = mSocketsData[idx].second;
                mSocketsData[idx].second.clear();
                
                //проверка на соответствие протоколу SCPI
                
                QStringList strCommands = str.split(';');
                foreach (QString cmd, strCommands){
                    //разбиваем команду по пробелам
                    QStringList tmpList = cmd.split(" ");
                    //удаляем все прописные символы из команды
                    cmd = tmpList[0].simplified();
                    int i = 0;
                    while (i<cmd.size()){
                        if (cmd[i].isLower()) cmd.remove(i,1);
                        else i++;
                    }
                    qDebug()<<"cmd: "<<cmd;
                    QString params;
                    tmpList.removeFirst();
                    params = tmpList.join(" ");
                    qDebug()<<"params: "<<params;
                    
                    if (cmd == "*IDN?"){
                        sendString("Scontel ControlUnit,00001,0.01.02\r\n");
                        continue;
                    }
                    if (cmd.indexOf("SYST") == 0){
                        //системные команды
                        systemRequest(cmd, params);
                        continue;
                    }
                    if (cmd.indexOf("GEN") == 0){
                        //базовые команды
                        generalRequest(cmd, params);
                        continue;
                    }
                    if (cmd.indexOf("SSPD") == 0){
                        //команды SSPD драйвера
                        sspdRequest(cmd, params);
                        continue;
                    }
                    if (cmd.indexOf("TEMD") == 0){
                        temperatureRequest(cmd, params);
                        continue;
                    }
                }
            }
        }
    }
}
