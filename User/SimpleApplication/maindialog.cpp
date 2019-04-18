#include "maindialog.h"
#include "ui_maindialog.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QSettings>
#include "Interfaces/curs485iointerface.h"
#include "../qCustomLib/qCustomLib.h"
//#include <QWindow>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog),
    mInterface(new cInterface(this)),
    lastWidgetIndex(-1),
    mTimerTimeOut(500),
    mTimer(new QTimer(this)),
    mInited(false)
{

    ui->setupUi(this);

    Qt::WindowFlags flags = Qt::Window | Qt::WindowSystemMenuHint
            | Qt::WindowMinimizeButtonHint
            | Qt::WindowMaximizeButtonHint
            | Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);

    mTimer->setSingleShot(true);

}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::on_stackedWidget_currentChanged(int arg1)
{
    if (!mInited) return;
    // закрываем старое окно
    if (lastWidgetIndex>-1){
        if (lastWidgetIndex == ui->stackedWidget->count()-1)
            aWidget->closeWidget();
        else {
            TempWidget* tWidget = qobject_cast<TempWidget*>(ui->stackedWidget->widget(lastWidgetIndex));
            SspdWidget* sWidget = qobject_cast<SspdWidget*>(ui->stackedWidget->widget(lastWidgetIndex));
            if (tWidget) tWidget->closeWidget();
            if (sWidget) sWidget->closeWidget();
        }
    }
    lastWidgetIndex = arg1;
    if (arg1 == ui->stackedWidget->count() - 1)
        aWidget->openWidget();
    else {
        TempWidget* tWidget = qobject_cast<TempWidget*>(ui->stackedWidget->widget(arg1));
        SspdWidget* sWidget = qobject_cast<SspdWidget*>(ui->stackedWidget->widget(arg1));
        if (tWidget) tWidget->openWidget();
        if (sWidget) sWidget->openWidget();
    }
}

void MainDialog::changeTimeOut(int msecs)
{
    mTimerTimeOut = msecs;
}

void MainDialog::timerTimeOut()
{
    int i = ui->stackedWidget->currentIndex();
    if (i == ui->stackedWidget->count() - 1){
        aWidget->updateWidget();
    }
    else {
        TempWidget* tWidget = qobject_cast<TempWidget*>(ui->stackedWidget->widget(i));
        SspdWidget* sWidget = qobject_cast<SspdWidget*>(ui->stackedWidget->widget(i));
        if (tWidget) tWidget->updateWidget();
        if (sWidget) sWidget->updateWidget();
    }
    mTimer->start(mTimerTimeOut);
}

void MainDialog::setAddress(const QHostAddress &address)
{
    mAddress = address;
}

bool MainDialog::initialize()
{
    // иницилизация порта и всего-всего.
    mElapsedTimer.start();
    qDebug()<<"Initialize";

    bool ok;
    QString answer;

    if (mTcpIpProtocol){
        cuTcpSocketIOInterface* interface = new cuTcpSocketIOInterface(this);

        interface->setAddress(convertToHostAddress(mTcpIpAddress));
        interface->setPort(SERVER_TCPIP_PORT);

        answer = interface->tcpIpQuery("SYST:DEVL?\r\n", 500, &ok);
        mInterface = interface;
    }
    else {
        cuRs485IOInterface* interface = new cuRs485IOInterface(this);
        interface->setPortName(mSerialPortName);

        QSettings settings("Scontel", "RaspPi Server");
        int size = settings.beginReadArray("devices");

        answer.append(QString("DevCount: %1\r\n").arg(size));

        for (int i = 0; i < size; ++i) {
            settings.setArrayIndex(i);

            answer.append(";<br>");
            answer.append(QString("Dev%1: address=%2: type=%3\r\n")
                          .arg(i)
                          .arg(settings.value("devAddress", 255).toInt())
                          .arg(settings.value("devType","None").toString()));
        }

        settings.endArray();
        mInterface = interface;
    }

    connect(mInterface, SIGNAL(msgReceived(quint8,quint8,quint8,quint8*)),
            this, SLOT(msgReceived(quint8,quint8,quint8,quint8*)));

    // получаем список доступных устройств.
    mWaiting = true;

    qDebug()<<"answer: "<<answer;
    createUI(answer);

    connect(mTimer, SIGNAL(timeout()), SLOT(timerTimeOut()));
    mTimer->start(mTimerTimeOut);
    mInited = true;
    on_stackedWidget_currentChanged(0);
    return true;
}

void MainDialog::msgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data)
{
    Q_UNUSED(address)
    mWaiting = false;
    switch (command) {
    case CMD_SERVER_GET_DEVICE_LIST:
        qDebug()<<"CMD_SERVER_GET_DEVICE_LIST";
        QString answer = QString(QByteArray((const char*)data, dataLength));
        qDebug()<<"Answer: "<<answer;
        createUI(answer);
        break;
    }
}

void MainDialog::setSerialPortName(const QString &serialPortName)
{
    mSerialPortName = serialPortName;
}

void MainDialog::setTcpIpAddress(const QString &tcpIpAddress)
{
    mTcpIpAddress = tcpIpAddress;
}

void MainDialog::setTcpIpProtocolEnable(bool tcpIpProtocol)
{
    mTcpIpProtocol = tcpIpProtocol;
}


bool MainDialog::waitingAnswer()
{
    QElapsedTimer timer;
    timer.start();
    while (mWaiting && (timer.elapsed()<1000))
        qApp->processEvents();
    return !mWaiting;
}

bool MainDialog::createUI(QString deviceList)
{
    QStringList list = deviceList.split("<br>");
    // составляем список всех устройств, каждый из них инициализируем  и т. д.
    foreach (QString str, list) {
        QStringList lList = str.split(':');
        if (lList.size() == 3){ //должно быть описание устройства
            //первое значение DevX - X порядковый номер
            int add = lList[1].split('=')[1].toInt();
            QString type = lList[2].split('=')[1];
            if (type.contains("CU4SD")){
                //данное устройство - SspdDriver
                mSdDrivers.append(new cCu4SdM0Driver(this));
                int i = mSdDrivers.size()-1;
                mSdDrivers[i]->setDevAddress(add);
                ui->listWidget->addItem(QString("Sspd Driver\nAddress: %1").arg(add));
                mSdDrivers[i]->setIOInterface(mInterface);

                SspdWidget* widget = new SspdWidget(this);
                widget->setDriver(mSdDrivers[i]);
                ui->stackedWidget->addWidget(widget);
            }
            else if (type.contains("CU4TD")){
                //данное устройство - TempDriver
                mTdDrivers.append(new cCu4TdM0Driver(this));
                int i = mTdDrivers.size()-1;
                mTdDrivers[i]->setDevAddress(add);
                ui->listWidget->addItem(QString("Temperature\nAddress: %1").arg(add));
                mTdDrivers[i]->setIOInterface(mInterface);

                TempWidget* widget = new TempWidget(this);
                widget->setDriver(mTdDrivers[i]);
                ui->stackedWidget->addWidget(widget);
            }
        }
    }

    ui->listWidget->addItem("All Channels");
    aWidget = new AllChannels(this);
    ui->stackedWidget->addWidget(aWidget);
    aWidget->initialize(deviceList);
    aWidget->setInterface(mInterface);

    connect(aWidget, SIGNAL(setTimeOut(int)), SLOT(changeTimeOut(int)));
    return true;
}
