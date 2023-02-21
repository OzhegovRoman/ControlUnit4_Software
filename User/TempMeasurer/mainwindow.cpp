#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"
#include "Drivers/tempdriverm0.h"
#include <QFileDialog>
#include <QInputDialog>

#include <QtCharts/QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QDateTime>
#include <QMenu>

#include "../NiceScale/nicescale.h"

#ifdef RASPBERRY_PI
#include <QApplication>
#include <QDesktopWidget>
#endif

#ifdef RS485
#include <QSerialPortInfo>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    deviceType(dtUnknown),
    mDriver(nullptr),
    mInterface(new IOInterface_t(this)),
    mTimer(new QTimer(this))
{
    qDebug()<<"MainWindow::MainWindow";
    ui->setupUi(this);

    QSettings settings("Scontel","QeMeasurer");
    mLastTcpIpAddress = settings.value("TcpAddress",QString()).toString();
    ui->sbDeviceAddress->setValue(settings.value("DeviceAddress",0).toInt());

#ifdef RASPBERRY_PI
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Plain);
    QLabel *title = new QLabel(QString("Quantum efficiency measurer").toUpper());
    title->setAlignment(Qt::AlignHCenter);
    QFont font = title->font();
    font.setBold(true);
    title->setFont(font);

    QPushButton *pPB = new QPushButton ("x");
    pPB->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    pPB->setFlat(true);
    pPB->setFixedHeight(QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight));
    pPB->setFixedWidth(QApplication::style()->pixelMetric(QStyle::PM_TitleBarHeight));

    QVBoxLayout *vlayout = new QVBoxLayout(ui->wTitle);
    QHBoxLayout *hlayout = new QHBoxLayout(ui->wTitle);
    hlayout->addWidget(title);
    hlayout->addWidget(pPB);

    vlayout->addLayout(hlayout);
    vlayout->addWidget(frame);
    vlayout->setMargin(0);
    ui->wTitle->setFixedHeight(ui->wTitle->minimumSizeHint().height());

    connect(pPB,SIGNAL(clicked()), this, SLOT(close()));
    this->setWindowState(Qt::WindowFullScreen);
#else
    ui->wTitle->hide();
#endif

#if  defined(FAKE)
    FakeCuSdM0 *device = new FakeCuSdM0();
    device->setAddress(0);
    device->setFakeInterface(mInterface);

    cUDID UDID;
    quint8 mUDID[12];
    for (int i = 0; i < 12; ++i)
        mUDID[i] = 0x11*i;
    UDID.setUDID(mUDID);

    device->setUDID(UDID);

#elif defined(RS485)
    ui->lAddress->setText("COM Port:");
    //    mInterface->setPortName("COM3");
#endif

    updateControlUnitList();

    for (int i = 0; i < ui->cbTcpIpAddress->count(); i++){
        if (ui->cbTcpIpAddress->itemText(i) == mLastTcpIpAddress){
            ui->cbTcpIpAddress->setCurrentIndex(i);
            break;
        }
    }

    ui->stackedWidget->setCurrentIndex(0);

    mTimer->setSingleShot(true);
    mTimer->setInterval(1000);

    ui->chTemperature->setRenderHint(QPainter::Antialiasing);
    auto * chart = ui->chTemperature->chart();
    chart->setDropShadowEnabled(false);
    chart->legend()->hide();

    ui->chTemperature->setContextMenuPolicy(Qt::CustomContextMenu);
    auto *series = new QLineSeries();
    series->setUseOpenGL();

    chart->addSeries(series);
    //ui->chTemperature->chart()->createDefaultAxes();
    {
        QDateTimeAxis *axisX = new QDateTimeAxis;
        axisX->setTickCount(10);
        axisX->setFormat("hh:mm:ss");
        axisX->setTitleText("Time");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis;
        axisY->setLabelFormat("%g");
        axisY->setTitleText("Temperature, [K]");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
    }

    chartRange = QRectF(0, 0, 0, 0);

    connect(ui->pbInitialize,   &QPushButton::clicked,                      this, &MainWindow::initialize);
    connect(ui->cbConnection,   &QCheckBox::clicked,                        this, &MainWindow::setConnectedState);
    connect(ui->chTemperature,  &QChartView::customContextMenuRequested,    this, &MainWindow::chartContextMenuRequest);

    connect(series,             &QLineSeries::pointAdded,                   this, [=](int index){
        auto * xAxis = reinterpret_cast<QDateTimeAxis *>(ui->chTemperature->chart()->axes(Qt::Horizontal)[0]);
        auto * yAxis = reinterpret_cast<QValueAxis *>(ui->chTemperature->chart()->axes(Qt::Vertical)[0]);

        auto point = series->at(index);

        if ((chartRange.width() == 0) && (chartRange.height() == 0) && (chartRange.bottomLeft() == QPointF(0,0))){
            chartRange = QRectF(QPointF(point.x(), point.y()),QSizeF(0,0));
        }
        if (point.x() < chartRange.left()){
            chartRange.setLeft(point.x());
            xAxis->setRange(QDateTime::fromMSecsSinceEpoch(chartRange.left()), QDateTime::fromMSecsSinceEpoch(chartRange.right()));
        }
        if (point.x() > chartRange.right()){
            chartRange.setRight(point.x());
            xAxis->setRange(QDateTime::fromMSecsSinceEpoch(chartRange.left()), QDateTime::fromMSecsSinceEpoch(chartRange.right()));
        }
        if (point.y() < chartRange.bottom()){
            chartRange.setBottom(point.y());
            NiceScale scale (chartRange.bottom(), chartRange.top());
            yAxis->setRange(scale.niceMin(), scale.niceMax());
            yAxis->setTickCount(scale.tickCount());
        }
        if (point.y() > chartRange.top()){
            chartRange.setTop(point.y());
            NiceScale scale (chartRange.bottom(), chartRange.top());
            yAxis->setRange(scale.niceMin(), scale.niceMax());
            yAxis->setTickCount(scale.tickCount());
        }

        ui->chTemperature->update();
    });

}

MainWindow::~MainWindow()
{
    qDebug()<<"MainWindow::~MainWindow";
    delete ui;
}

void MainWindow::chartContextMenuRequest(QPoint pos)
{
    qDebug()<<"MainWindow::chartContextMenuRequest";

    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction("Clear",        this, &MainWindow::deleteMeasureData);
    menu->addAction("Save data",    this, &MainWindow::saveData);

    menu->popup(ui->chTemperature->mapToGlobal(pos));
}

void MainWindow::initialize()
{
    qDebug()<<"MainWindow::on_pbInitialize_clicked";
    // сначала нужно получить информацию от модуля и выбрать подходящий тип
    CommonDriver driver;
    driver.setIOInterface(mInterface);

#ifdef RS485
    mInterface->setPortName(ui->cbTcpIpAddress->currentText());
    driver.setDevAddress(static_cast<const quint8>(ui->sbDeviceAddress->value()));
#endif

#ifdef TCPIP_SOCKET_INTERFACE
    QString str = ui->cbTcpIpAddress->currentText();
    while (str.contains(".0")){
        str.replace(".0",".");
    }

    while (str.contains("..")){
        str.replace("..",".0.");
    }

    qDebug()<<str;
    mInterface->setAddress(convertToHostAddress(str));
    qDebug()<<mInterface->address();
    mInterface->setPort(SERVER_TCPIP_PORT);
    driver.setDevAddress(ui->sbDeviceAddress->value());
#endif

    if (!driver.getDeviceInfo()) {
        ui->lbStatus->setText("Status: Error at getDeviceInfo function");
        showActions(false);
        return;
    }

    QString tmpStr = QString("Status: Device %1 inited<br>"
                             "Modification: %2<br>"
                             "Hardware: %3<br>"
                             "Firmware: %4<br>"
                             "Description: %5<br>"
                             "DeviceId: %6")
            .arg(driver.deviceType()->currentValue())
            .arg(driver.modificationVersion()->currentValue())
            .arg(driver.hardwareVersion()->currentValue())
            .arg(driver.firmwareVersion()->currentValue())
            .arg(driver.deviceDescription()->currentValue())
            .arg(driver.UDID()->currentValue().toString());

    ui->lbStatus->setText(tmpStr);

    // проверяем правильность драйвера
    if (!driver.deviceType()->currentValue().contains("CU4TDM0")){
        // что-то пошло не так, блочим все функции.
        showActions(false);
        return;
    }

    // а вот тут уже создаем новый
    if (mDriver!= nullptr)
        mDriver->deleteLater();
    mDriver = nullptr;

    showActions(true);

    deviceType = dtTempM0;
    mDriver = new TempDriverM0(this);

    mDriver->setIOInterface(mInterface);
    mDriver->setDevAddress(driver.devAddress());

    bool ok = false;
    mDriver->init()->executeSync(&ok);
    if (!ok) {
        ui->lbStatus->setText("Status: Error at init function");
        return;
    }

    QSettings settings("Scontel","QeMeasurer");
    mLastTcpIpAddress = ui->cbTcpIpAddress->currentText();
    settings.setValue("TcpAddress", mLastTcpIpAddress);
    settings.setValue("DeviceAddress", ui->sbDeviceAddress->value());
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    mTimer->stop();

    disconnect(mTimer, nullptr, nullptr, nullptr);

    switch (arg1) {
    case 1: // Temp Driver
        disconnect(mTimer, nullptr, nullptr, nullptr);
        connect(mTimer, &QTimer::timeout, this, &MainWindow::updateData);
        updateData();
        break;
    case 2: //Graphs
    {
        disconnect(mTimer, nullptr, nullptr, nullptr);
        connect(mTimer, &QTimer::timeout, this, &MainWindow::updateTemperatureGraph);
        updateTemperatureGraph();
        break;
    }
    default:
        break;
    }
}

void MainWindow::updateData()
{
    bool ok = false;
    auto data  = qobject_cast<TempDriverM0*>(mDriver)->data()->getValueSync(&ok, 5);
    if (ok){
        ui->lbData->setText(QString("<h2>T: %1 K</h2>I sensor: %2 uA<br>U sensor: %3 V")
                            .arg(static_cast<double>(data.Temperature),6,'f',2)
                            .arg(static_cast<double>(data.TempSensorCurrent*1e6),6,'f',2)
                            .arg(static_cast<double>(data.TempSensorVoltage),6,'f',3)
                            );
        ui->cbConnection->setChecked(data.CommutatorOn);
    }
    if (!ok)
        qDebug()<<"error at getting data";
    mTimer->start();
}

void MainWindow::updateTemperatureGraph()
{
    bool ok = false;
    double temperature = 0;
    auto tmpTempM0 = qobject_cast<TempDriverM0*>(mDriver);
    if (tmpTempM0)
        temperature = tmpTempM0->temperature()->getValueSync(&ok, 5);

    if (!ok) {
        qDebug()<<"can't read counts data";
        mTimer->start();
        return;
    }

    auto key = QDateTime::currentDateTime().toMSecsSinceEpoch();

    auto * series = reinterpret_cast<QLineSeries *>(ui->chTemperature->chart()->series().last());
    series->append(key, temperature);

    ui->lbTempStatus->setText(QString("Temperature: %1 [K]").arg(temperature));

    mTimer->start();
}

void MainWindow::deleteMeasureData()
{
    auto series = qobject_cast<QLineSeries*>(ui->chTemperature->chart()->series()[0]);
    series->clear();
    chartRange = QRectF(0,0,0,0);
}

void MainWindow::saveData()
{
    QSettings settings("Scontel","QeMeasurer");
    QString defFileName = settings.value("LastFileName", "/untitled.txt").toString();

    QString fileName = QFileDialog::getSaveFileName(this,tr("Save data..."),
                                                    defFileName,
                                                    tr("data (*.txt)"));
    if (fileName.isEmpty()) return;

    settings.setValue("LastFileName", fileName);

    QFile m_File(fileName);
    m_File.open(QIODevice::WriteOnly);
    QTextStream out(&m_File);
    out<<"Time\tTemperature\r\n";
    out<<"hh:mm:ss\tK\r\n";
    auto *series = qobject_cast<QLineSeries*>(ui->chTemperature->chart()->series()[0]);
    foreach (auto point, series->points()) {
        out<<QDateTime::fromMSecsSinceEpoch(point.x()).toString("hh:mm:ss")<<'\t'<<point.y()<<"\r\n";
    }
    m_File.close();
}

void MainWindow::updateControlUnitList()
{
    setDisabled(true);
    qApp->processEvents();
    ui->cbTcpIpAddress->clear();
#ifdef RS485
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    if (!list.isEmpty())
        foreach (const QSerialPortInfo &info, list) {
            ui->cbTcpIpAddress->addItem(info.portName());
        }
#elif TCPIP_SOCKET_INTERFACE
    QStringList list = availableControlUnits();
    if (!list.isEmpty())
        ui->cbTcpIpAddress->addItems(list);
    ui->cbTcpIpAddress->addItems(QStringList()<<"Update..."<<"Manual...");
#endif
    setEnabled(true);
}

void MainWindow::on_cbTcpIpAddress_activated(const QString &arg1)
{
    if (arg1.contains("Update")){
        updateControlUnitList();
        ui->cbTcpIpAddress->setCurrentIndex(0);
    }
    else if (arg1.contains("Manual")){

        QSettings settings("Scontel", "QeMeasurer");
        QString LastTcpIpAddress = settings.value("TcpIpAddress","127.000.000.001").toString();

        bool ok;
        QString  str = QInputDialog::getText(this
                                             , "TcpIp Addresss..."
                                             , "Enter TcpIp Address manualy"
                                             , QLineEdit::Normal
                                             , LastTcpIpAddress
                                             , &ok);
        if (ok && !str.isEmpty()){
            updateControlUnitList();

            ok = false;
            for (int i = 0; i < ui->cbTcpIpAddress->count(); i++){
                if (ui->cbTcpIpAddress->itemText(i) == str){
                    ui->cbTcpIpAddress->setCurrentIndex(i);
                    ok = true;
                    break;
                }
            }
            if (!ok){
                ui->cbTcpIpAddress->insertItem(0, str);
                ui->cbTcpIpAddress->setCurrentIndex(0);
            }
        }
    }
}

void MainWindow::showActions(bool visible)
{
    for (int i = 0; i < ui->lwActions->count(); ++i) {
        ui->lwActions->item(i)->setFlags(visible ?  Qt::ItemIsSelectable | Qt::ItemIsEnabled : Qt::NoItemFlags);
    }
}

void MainWindow::setConnectedState(bool connected)
{
    qobject_cast<TempDriverM0*>(mDriver)->commutator()->setValueSync(connected, nullptr, 5);
}
