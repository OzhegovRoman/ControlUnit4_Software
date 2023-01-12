#include "mainwindow.h"
#include "qmenu.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"
#include "Drivers/sisbiassourcedriverm0.h"
#include "servercommands.h"

#include <QValueAxis>
#include <QtCharts/QScatterSeries>

#include <QElapsedTimer>
#include <QInputDialog>

#include <QMenu>
#include <QFileDialog>

#include "nicescale.h"


#ifdef RASPBERRY_PI
#include <QApplication>
#include <QDesktopWidget>
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

    updateControlUnitList();

    for (int i = 0; i < ui->cbTcpIpAddress->count(); i++){
        if (ui->cbTcpIpAddress->itemText(i) == mLastTcpIpAddress){
            ui->cbTcpIpAddress->setCurrentIndex(i);
            break;
        }
    }
    mTimer->setSingleShot(true);

    ui->wChartView->setRenderHint(QPainter::Antialiasing);
    ui->wChartView->chart()->setDropShadowEnabled(false);
    ui->wChartView->chart()->legend()->hide();

    ui->leStartSweep->setValidator(new QDoubleValidator(this));
    ui->leStepSweep->setValidator(new QDoubleValidator(this));
    ui->leStopSweep->setValidator(new QDoubleValidator(this));
    ui->leTimeStepSweep->setValidator(new QIntValidator(this));

    ui->wChartView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->wChartView, &QChartView::customContextMenuRequested, this, &MainWindow::measurerContextMenuRequest);
}

MainWindow::~MainWindow()
{
    qDebug()<<"MainWindow::~MainWindow";
    delete ui;
}

//TODO: переделать контекстное меню
void MainWindow::measurerContextMenuRequest(QPoint pos)
{
    qDebug()<<"MainWindow::measurerContextMenuRequest";

    if (ui->wChartView->chart()->series().count() == 0) return;

    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction("Clear", this, SLOT(deleteMeasureData()));
    menu->addAction("Save data", this, SLOT(saveData()));

    menu->popup(ui->wChartView->mapToGlobal(pos));
}

void MainWindow::on_pbInitialize_clicked()
{
    qDebug()<<"MainWindow::on_pbInitialize_clicked";
    // сначала нужно получить информацию от модуля и выбрать подходящий тип
    CommonDriver driver;
    driver.setIOInterface(mInterface);

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
    driver.setDevAddress(static_cast<const quint8>(ui->sbDeviceAddress->value()));
#endif

    if (!driver.getDeviceInfo()) {
        ui->lbStatus->setText("Status: Error at getDeviceInfo function");
        return;
    }
    // а вот тут уже создаем новый
    if (mDriver!= nullptr)
        mDriver->deleteLater();
    mDriver = nullptr;


    deviceType = dtUnknown;
    if (driver.deviceType()->currentValue().contains("CU4BS"))
        deviceType = dtSisBiasSource;

    mDriver = new SisBiasSourceDriverM0(this);
    mDriver->setIOInterface(mInterface);
    mDriver->setDevAddress(driver.devAddress());

    bool ok = false;
    mDriver->init()->executeSync(&ok);
    if (!ok) {
        ui->lbStatus->setText("Status: Error at init function");
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

    for (int i = 0; i<3; ++i) {
        ui->lwActions->item(i)->setFlags(deviceType == dtUnknown ? Qt::NoItemFlags : Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    if (deviceType == dtUnknown)
        return;

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
    case 1: // Инициализация
        mTimer->setInterval(500);
        connect(mTimer, SIGNAL(timeout()), SLOT(updateData()));
        updateData();
        break;
    case 2:
        // Measure
        //        ui->cbSweepType->setCurrentIndex(0);
        break;
    default:
        break;
    }
}

void MainWindow::updateData()
{
    bool ok = false;
    if (deviceType ==  dtUnknown) return;

    auto data = mDriver->data()->getValueSync(&ok, 5);
    if (ok){
        ui->lbData->setText(QString("U: %1 mV<br>I: %2 uA<br>")
                            .arg(static_cast<double>(data.voltage)*1e3, 6, 'f', 2)
                            .arg(static_cast<double>(data.current)*1e6, 6, 'f', 1));
        ui->cbShort ->setChecked(data.shortCircuit);
        ui->rbImode ->setChecked(data.mode==CU4BSM0_IMODE);
        ui->rbUmode ->setChecked(data.mode==CU4BSM0_UMODE);
        ui->pbSetI  ->setEnabled(data.mode==CU4BSM0_IMODE);
        ui->pbSetU  ->setEnabled(data.mode==CU4BSM0_UMODE);
        ui->sbI     ->setEnabled(data.mode==CU4BSM0_IMODE);
        ui->sbU     ->setEnabled(data.mode==CU4BSM0_UMODE);
    }
    else
        qDebug()<<"error at getting data";
    mTimer->start();
}

void MainWindow::setCurrentValue()
{
    qDebug()<<"MainWindow::setCurrentValue";

    if (mVoltageMode)
        mDriver->voltage()->setValueSync(mCurrentValue*1e-3, nullptr, 5);
    else
        mDriver->current()->setValueSync(mCurrentValue*1e-6, nullptr, 5);

    mTimer->start();
}

void MainWindow::deleteMeasureData()
{
    qDebug()<<"clear";
    foreach (auto series, ui->wChartView->chart()->series()) {
        series->deleteLater();
    }
    ui->wChartView->chart()->removeAllSeries();
    foreach (auto axis, ui->wChartView->chart()->axes()){
        ui->wChartView->chart()->removeAxis(axis);
        axis->deleteLater();
    }
    ui->wChartView->repaint();
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
    out<<"U\tI\r\n";
    out<<"; X == U (mV)\r\n";
    out<<"; Y == I (uA)\r\n";
    int index = 0;
    foreach (auto series, ui->wChartView->chart()->series()) {
        out<<"Graph "<<(index++)<<"\r\n";
        auto points = reinterpret_cast<QScatterSeries *>(series)->points();

        for (int i = 0; i<points.count(); i++){
            out<<(points[i].x())<<'\t'<<points[i].y()<<"\r\n";
        }
    }
    m_File.close();

}

void MainWindow::enableControlsAtMeasure(bool value)
{
    ui->leStartSweep->setEnabled(value);
    ui->leStepSweep->setEnabled(value);
    ui->leStopSweep->setEnabled(value);
    ui->leTimeStepSweep->setEnabled(value);
    ui->cbSweepType->setEnabled(value);
    ui->pbStart->setEnabled(value);
    ui->lwActions->setEnabled(value);
}

void MainWindow::on_cbShort_clicked(bool checked)
{
    bool ok = false;
    mDriver->shortEnable()->setValueSync(checked, &ok);
    if (!ok) qDebug()<<"can't set short";
}

void MainWindow::on_pbStart_clicked()
{

    qDebug()<<"MainWindow::on_pbStart_clicked";

    // Запуск процесса измерения
    QScatterSeries *series = new QScatterSeries(ui->wChartView->chart());
    series->setUseOpenGL();

    connect(series, &QScatterSeries::pointAdded, this, [=](int index){
        auto * xAxis = reinterpret_cast<QValueAxis *>(ui->wChartView->chart()->axes(Qt::Horizontal)[0]);
                auto * yAxis = reinterpret_cast<QValueAxis *>(ui->wChartView->chart()->axes(Qt::Vertical)[0]);
//        yAxis->setTickCount(10);

        auto point = series->at(index);

        if ((chartRange.width() == 0) && (chartRange.height() == 0) && (chartRange.bottomLeft() == QPointF(0,0))){
            chartRange = QRectF(QPointF(point.x(), point.y()),QSizeF(0,0));
        }

        if (point.x() < chartRange.left()){
            chartRange.setLeft(point.x());
            NiceScale scale (chartRange.left(), chartRange.right());
            xAxis->setRange(scale.niceMin(), scale.niceMax());
            xAxis->setTickCount(scale.tickCount());
        }
        if (point.x() > chartRange.right()){
            chartRange.setRight(point.x());
            NiceScale scale (chartRange.left(), chartRange.right());
            xAxis->setRange(scale.niceMin(), scale.niceMax());
            xAxis->setTickCount(scale.tickCount());
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

    });

    series->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    series->setBorderColor(series->pen().color());
    series->setMarkerSize(10.0);
    series->setPen(QColor(Qt::transparent));
    ui->wChartView->chart()->addSeries(series);
    if (ui->wChartView->chart()->series().count() == 1){
        ui->wChartView->chart()->createDefaultAxes();
        chartRange = QRectF(0,0,0,0);
        auto * xAxis = reinterpret_cast<QValueAxis *>(ui->wChartView->chart()->axes(Qt::Horizontal)[0]);
        auto * yAxis = reinterpret_cast<QValueAxis *>(ui->wChartView->chart()->axes(Qt::Vertical)[0]);
//        xAxis->setTickCount(10);
//        yAxis->setTickCount(10);
        xAxis->setTitleText("Voltage, [mV]");
        yAxis->setTitleText("Current, [uA]");
    }
    else {
        series->attachAxis(ui->wChartView->chart()->axes(Qt::Horizontal)[0]);
        series->attachAxis(ui->wChartView->chart()->axes(Qt::Vertical)[0]);
    }

    enableControlsAtMeasure(false);

    //TODO: поправить в зависимости от режима
    CU4BSM0_SWEEP_PARAMS_t params;
    if (ui->cbSweepType->currentIndex() == 0){
        params.mode         = CU4BSM0_UMODE;
        params.start        = ui->leStartSweep->text().toDouble() * 1e-3;
        params.stop         = ui->leStopSweep->text().toDouble()  * 1e-3;
        params.step         = ui->leStepSweep->text().toDouble()  * 1e-3;
    }
    else {
        params.mode         = CU4BSM0_IMODE;
        params.start        = ui->leStartSweep->text().toDouble() * 1e-6;
        params.stop         = ui->leStopSweep->text().toDouble()  * 1e-6;
        params.step         = ui->leStepSweep->text().toDouble()  * 1e-6;
    }
    params.timeStep_ms  = ui->leTimeStepSweep->text().toDouble();

    bool ok;

    mDriver->sweepParams()->setValueSync(params, &ok, 5);
    if (!ok){
        enableControlsAtMeasure(true);
        return;
    }

    // устанавливаем режим смещения
    mDriver->mode()->setValueSync(params.mode, &ok, 5);
    if (!ok){
        enableControlsAtMeasure(true);
        return;
    }

    // устанавливаем начальное значение тока
    if (params.mode == CU4BSM0_UMODE){
        mDriver->voltage()->setValueSync(params.start, &ok, 5);
    }
    else
        mDriver->current()->setValueSync(params.start, &ok, 5);

    if (!ok){
        enableControlsAtMeasure(true);
        return;
    }

    // убираем закоротку
    mDriver->shortEnable()->setValueSync(false, &ok, 5);
    if (!ok){
        enableControlsAtMeasure(true);
        return;
    }

    // задержка 500 ms
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed()<500)
        qApp->processEvents();

    // включаем pipe mode
    connect(mInterface, &cuIOInterface::msgReceived, this, &MainWindow::serverMessageReceive);
    serverMessageReceived = false;
    mInterface->sendMsg(SERVER_ADDRESS, CMD_SERVER_PIPE_MODE_ON, 0, nullptr);
    while (!serverMessageReceived){
        qApp->processEvents();
    }

    // включаем sweep
    sweepStoped = false;

    mDriver->sweepStart()->executeSync(&ok, 5);
    if (!ok){
        enableControlsAtMeasure(true);
        return;
    }

    // ждем окончания посылки
    while (!sweepStoped){
        qApp->processEvents();
    }

    // выключаем pipe mode
    serverMessageReceived = false;
    mInterface->sendMsg(SERVER_ADDRESS, CMD_SERVER_PIPE_MODE_OFF, 0, nullptr);
    while (!serverMessageReceived){
        qApp->processEvents();
    }
    disconnect(mInterface, &cuIOInterface::msgReceived, this, &MainWindow::serverMessageReceive);

    enableControlsAtMeasure(true);
}

void MainWindow::on_pbStop_clicked()
{
    //    mStopFlag = true;
}

void MainWindow::updateControlUnitList()
{
    setDisabled(true);
    qApp->processEvents();
    QStringList list = availableControlUnits();
    ui->cbTcpIpAddress->clear();
    if (!list.isEmpty())
        ui->cbTcpIpAddress->addItems(list);
    ui->cbTcpIpAddress->addItems(QStringList()<<"Update..."<<"Manual...");

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
        QString LastTcpIpAddress = settings.value("TcpAddress","127.000.000.001").toString();

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

void MainWindow::on_rbUmode_toggled(bool checked)
{
    mDriver->mode()->setValueSync(checked ? CU4BSM0_UMODE : CU4BSM0_IMODE, nullptr, 5);
}


void MainWindow::on_pbSetU_clicked()
{
    mDriver->voltage()->setValueSync(ui->sbU->value()*1e-3, nullptr, 5);
}

void MainWindow::serverMessageReceive(quint8 address, quint8 command, quint8 dataLength, quint8 *data)
{
    Q_UNUSED(address);

    serverMessageReceived = true;
    if (command == cmd::BS_SweepData){
        if (dataLength == 12){

            auto* tmp = reinterpret_cast<CU4BSM0_SWEEP_DATA_t*>(data);

            double key = tmp->voltage*1000.;
            double value = tmp->current*1e6;

            auto * series = reinterpret_cast<QScatterSeries *>(ui->wChartView->chart()->series().last());
            series->append(key, value);
        }
        if (dataLength == 4){
            sweepStoped = true;
        }
    }
}

void MainWindow::on_pbSetI_clicked()
{
    mDriver->current()->setValueSync(ui->sbI->value()*1e-6, nullptr, 5);
}

void MainWindow::on_cbSweepType_currentIndexChanged(int index)
{
    if (index == 0){
        ui->lbStep->setText("Step, mV");
        ui->lbStart->setText("Start, mV");
        ui->lbStop->setText("Stop, mV");
    }
    else {
        ui->lbStep->setText("Step, uA");
        ui->lbStart->setText("Start, uA");
        ui->lbStop->setText("Stop, uA");
    }
}

