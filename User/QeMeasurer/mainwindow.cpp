#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"

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
    mTimer->setSingleShot(true);

    ui->wCounterPlot->addGraph();
    ui->wCounterPlot->graph(0)->setPen(QPen(QApplication::palette().highlight(), 3));

    ui->wCounterPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->wCounterPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->wCounterPlot->xAxis->setAutoTickStep(false);
    ui->wCounterPlot->xAxis->setTickStep(10);
    ui->wCounterPlot->setBackground(QApplication::palette().base().color());

    ui->wCounterPlot->xAxis->setTickLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->xAxis->setBasePen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->xAxis->setLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->xAxis->setTickPen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->xAxis->setSubTickPen(QPen(QApplication::palette().text().color()));

    ui->wCounterPlot->xAxis2->setTickLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->xAxis2->setBasePen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->xAxis2->setLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->xAxis2->setTickPen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->xAxis2->setSubTickPen(QPen(QApplication::palette().text().color()));

    ui->wCounterPlot->yAxis->setTickLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->yAxis->setBasePen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->yAxis->setLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->yAxis->setTickPen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->yAxis->setSubTickPen(QPen(QApplication::palette().text().color()));

    ui->wCounterPlot->yAxis2->setTickLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->yAxis2->setBasePen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->yAxis2->setLabelColor(QApplication::palette().text().color());
    ui->wCounterPlot->yAxis2->setTickPen(QPen(QApplication::palette().text().color()));
    ui->wCounterPlot->yAxis2->setSubTickPen(QPen(QApplication::palette().text().color()));

    ui->wCounterPlot->axisRect()->setupFullAxesBox();

    ui->wCounterPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->wCounterPlot, &QCustomPlot::customContextMenuRequested,
            this, &MainWindow::counterContextMenuRequest);

    ui->wMeasurerPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    // setup policy and connect slot for context menu popup:
    ui->wMeasurerPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->wMeasurerPlot, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(measurerContextMenuRequest(QPoint)));

    setAutoScaleCounterPlot(true);


    ui->wMeasurerPlot->setBackground(QApplication::palette().base().color());
    ui->wMeasurerPlot->xAxis->setTickLabelColor(QApplication::palette().text().color());
    ui->wMeasurerPlot->xAxis->setBasePen(QPen(QApplication::palette().text().color()));
    ui->wMeasurerPlot->xAxis->setLabelColor(QApplication::palette().text().color());
    ui->wMeasurerPlot->xAxis->setTickPen(QPen(QApplication::palette().text().color()));
    ui->wMeasurerPlot->xAxis->setSubTickPen(QPen(QApplication::palette().text().color()));

    ui->wMeasurerPlot->yAxis->setTickLabelColor(QApplication::palette().text().color());
    ui->wMeasurerPlot->yAxis->setBasePen(QPen(QApplication::palette().text().color()));
    ui->wMeasurerPlot->yAxis->setLabelColor(QApplication::palette().text().color());
    ui->wMeasurerPlot->yAxis->setTickPen(QPen(QApplication::palette().text().color()));
    ui->wMeasurerPlot->yAxis->setSubTickPen(QPen(QApplication::palette().text().color()));

    connect(ui->pbInitialize, &QPushButton::clicked, this, &MainWindow::initialize);
}

MainWindow::~MainWindow()
{
    qDebug()<<"MainWindow::~MainWindow";
    delete ui;
}

void MainWindow::measurerContextMenuRequest(QPoint pos)
{
    qDebug()<<"MainWindow::measurerContextMenuRequest";

    if (ui->wMeasurerPlot->graphCount() == 0) return;

    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction("Clear", this, SLOT(deleteMeasureData()));
    menu->addAction("Save data", this, SLOT(saveData()));

    menu->popup(ui->wMeasurerPlot->mapToGlobal(pos));
}

void MainWindow::counterContextMenuRequest(QPoint pos)
{
    qDebug()<<"MainWindow::counterContextMenuRequest";

    //   if (ui->wCounterPlot->graphCount() == 0) return;

    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    QAction *autoScale = new QAction("Auto scale",menu);
    autoScale->setCheckable(true);
    autoScale->setChecked(isEnabledCounterAutoscale);

    QAction *rangesVisibility = new QAction("Show Y range",menu);
    rangesVisibility->setCheckable(true);
    rangesVisibility->setChecked(isRangesVisible);

    connect(autoScale,&QAction::triggered,this,&MainWindow::setAutoScaleCounterPlot);
    connect(rangesVisibility, &QAction::triggered, this, &MainWindow::setRangesVisible);

    menu->addAction(autoScale);
    menu->addAction(rangesVisibility);

    menu->addAction("Clear", this, [=](){
        ui->wCounterPlot->graph()->removeDataAfter(0);
        ui->wCounterPlot->replot();
    });

    menu->popup(ui->wCounterPlot->mapToGlobal(pos));
}

void MainWindow::setAutoScaleCounterPlot(bool isAuto)
{
    qDebug()<<"MainWindow::setAutoScaleCounterPlot";

    isEnabledCounterAutoscale = isAuto;
    setRangesVisible(!isAuto);
    ui->SB_YMin->setDisabled(isAuto);
    ui->SB_YMax->setDisabled(isAuto);
}

void MainWindow::controlAutoScaleCounter()
{
    if (isEnabledCounterAutoscale){
        auto range = ui->wCounterPlot->yAxis->range();
        ui->SB_YMin->setValue(range.lower);
        ui->SB_YMax->setValue(range.upper);
    }
    else
    {
        if (ui->SB_YMin->value() >= ui->SB_YMax->value())
            ui->SB_YMin->setValue(ui->SB_YMax->value() - 1);
        if (ui->SB_YMax->value() <= ui->SB_YMin->value())
            ui->SB_YMax->setValue(ui->SB_YMin->value() + 1);
        ui->wCounterPlot->yAxis->setRange(ui->SB_YMin->value(),ui->SB_YMax->value());
    }
}

void MainWindow::setRangesVisible(bool isVisible)
{
    isRangesVisible = isVisible;
    ui->SB_YMin->setVisible(isRangesVisible);
    ui->SB_YMax->setVisible(isRangesVisible);
    ui->L_YMin-> setVisible(isRangesVisible);
    ui->L_YMax-> setVisible(isRangesVisible);
}

void MainWindow::initialize()
{
    qDebug()<<"MainWindow::on_pbInitialize_clicked";
    // сначала нужно получить информацию от модуля и выбрать подходящий тип
    CommonDriver driver;
    driver.setIOInterface(mInterface);
    bool secretMode = QGuiApplication::queryKeyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier);

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

    if (driver.deviceType()->currentValue().contains("CU4SDM0")){
        deviceType = dtSspdM0;
        mDriver = new SspdDriverM0(this);
        ui->cbHFMode->setVisible(false);
    }
    if (driver.deviceType()->currentValue().contains("CU4SDM1")){
        deviceType = dtSspdM1;
        mDriver = new SspdDriverM1(this);
        ui->cbHFMode->setVisible(true);
    }

    if (mDriver == nullptr){
        deviceType = dtUnknown;
        mDriver = new CommonDriver(this);
    }


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

    if (ui->lwActions->count()>4)
        ui->lwActions->takeItem(4);

    for (int i = 0; i<4; ++i) {
        ui->lwActions->item(i)->setFlags(deviceType == dtUnknown ? Qt::NoItemFlags : Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    if (deviceType == dtUnknown)
        return;

    if (secretMode){
        tmpStr.append("<br>Secret Mode Activated");
        ui->lwActions->addItem("Secret Params");
        ui->lwActions->item(4)->setFont(ui->lwActions->item(3)->font());
    }

    on_pbReadParams_clicked();

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
    {
        bool ok = false;
        auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
        auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
        if (tmpSspdM0){
            auto params = tmpSspdM0->params()->getValueSync(&ok, 2);
            mTimerCount_Interval = static_cast<int>(static_cast<double>(params.Time_Const)*1000.0);
            auto status = tmpSspdM0->status()->getValueSync(&ok, 2);
            status.stCounterOn = true;
            status.stRfKeyToCmp = true;
            status.stComparatorOn = true;
            status.stCounterOn = true;
            tmpSspdM0->status()->setValueSync(status);
        }
        if (tmpSspdM1){
            auto params = tmpSspdM1->params()->getValueSync(&ok, 2);
            mTimerCount_Interval = static_cast<int>(static_cast<double>(params.Time_Const)*1000.0);
            auto status = tmpSspdM1->status()->getValueSync(&ok, 2);
            status.stCounterOn = true;
            status.stComparatorOn = true;
            status.stCounterOn = true;
            tmpSspdM1->status()->setValueSync(status);
        }

        mTimer->setInterval(qMax(100, mTimerCount_Interval));

        connect(mTimer, SIGNAL(timeout()), SLOT(updateCountsGraph()));
        updateCountsGraph();
        break;
    }
    case 3: // Measure
        ui->cbType->setCurrentIndex(0);
        break;
    default:
        break;
    }
}

void MainWindow::updateData()
{
    bool ok = false;
    if (deviceType == dtSspdM0){
        auto data = qobject_cast<SspdDriverM0*>(mDriver)->data()->getValueSync(&ok, 5);
        if (ok){
            ui->lbData->setText(QString("I: %1 uA<br>U: %2 mV")
                                .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                                .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2));
            ui->cbAmplifier->setChecked(data.Status.stAmplifierOn);
            ui->cbShort->setChecked(data.Status.stShorted);
            ui->cbComparator->setChecked(data.Status.stComparatorOn);
        }
    }
    else if (deviceType == dtSspdM1){
        auto data = qobject_cast<SspdDriverM1*>(mDriver)->data()->getValueSync(&ok, 5);
        if (ok){
            ui->lbData->setText(QString("I: %1 uA<br>Imon: %2 uA<br>U: %3 mV")
                                .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                                .arg(static_cast<double>(data.CurrentMonitor)*1e6, 6,'f', 1)
                                .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2));
            ui->cbAmplifier->setChecked(data.Status.stAmplifierOn);
            ui->cbShort->setChecked(data.Status.stShorted);
            ui->cbComparator->setChecked(data.Status.stComparatorOn);
            ui->cbHFMode->setChecked(data.Status.stHFModeOn);
        }
    }
    if (!ok)
        qDebug()<<"error at getting data";
    mTimer->start();
}

void MainWindow::updateCountsGraph()
{
    qDebug()<<"MainWindow::updateCountsGraph";
    bool ok = false;
    double counts = 0;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0)
        counts = tmpSspdM0->counts()->getValueSync(&ok, 5);
    if (tmpSspdM1)
        counts = tmpSspdM1->counts()->getValueSync(&ok, 5);

    if (!ok) {
        qDebug()<<"can't read counts data";
        mTimer->start();
        return;
    }
    counts *= 1000.0/static_cast<double>(mTimerCount_Interval);
    qreal key = QDateTime::currentDateTime().currentMSecsSinceEpoch()/1000.0;

    ui->wCounterPlot->graph(0)->addData(key, counts);
    // remove data of lines that's outside visible range:
    ui->wCounterPlot->rescaleAxes(true);

    // make key axis range scroll with the data (at a constant range size of 8):
    //    ui->wCustomPlot->rescaleAxes();
    ui->wCounterPlot->xAxis->setRange(key+2, 30, Qt::AlignRight);
    controlAutoScaleCounter();
    ui->wCounterPlot->graph()->removeDataBefore(key-29);
    ui->wCounterPlot->replot();

    ui->lbCounts->setText(QString("Counts per second: %1").arg(counts));

    mTimer->start();
}

void MainWindow::setCurrentValue()
{
    qDebug()<<"MainWindow::setCurrentValue";
    if (mCurrentValue > ui->sbStop->value()) {
        on_pbStop_clicked();
        return;
    }

    float value = mCurrentValue;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);

    switch (ui->cbType->currentIndex()) {
    case 0:
        //Bias
        value = mCurrentValue*1E-6;
        if (tmpSspdM0)
            tmpSspdM0->current()->setValueSync(value, nullptr, 5);
        if (tmpSspdM1)
            tmpSspdM1->current()->setValueSync(value, nullptr, 5);
        break;
    case 1:
        //Cmp  ref level
        if (tmpSspdM0)
            tmpSspdM0->cmpReferenceLevel()->setValueSync(value, nullptr, 5);
        if (tmpSspdM1)
            tmpSspdM1->cmpReferenceLevel()->setValueSync(value, nullptr, 5);
        break;
    default:
        return;
    }
    mTimer->start();
}

void MainWindow::addPoint2MeasureGraphs()
{
    qDebug()<<"MainWindow::addPoint2MeasureGraphs";
    double key = mCurrentValue;
    float value = 0;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);

    if (tmpSspdM0){
        auto data = tmpSspdM0->data()->getValueSync(nullptr, 5);
        value =  static_cast<double>(data.Counts/tmpSspdM0->params()->currentValue().Time_Const);
        if (static_cast<double>(data.Voltage) > ui->sbTrigger->value()){ //триггер сработал
            on_pbStop_clicked();
            return;
        }
        if (ui->cbType->currentIndex() == 0 )
            key = static_cast<double>(data.Current)*1E6;
    }
    if (tmpSspdM1){
        auto data = tmpSspdM1->data()->getValueSync(nullptr, 5);
        value =  static_cast<double>(data.Counts/tmpSspdM1->params()->currentValue().Time_Const);
        if (static_cast<double>(data.Voltage) > ui->sbTrigger->value()){ //триггер сработал
            on_pbStop_clicked();
            return;
        }
        if (ui->cbType->currentIndex() == 0 )
            key = static_cast<double>(data.Current)*1E6;
    }

    ui->wMeasurerPlot->graph(ui->wMeasurerPlot->graphCount() - 1)->addData(key, value);
    ui->wMeasurerPlot->rescaleAxes(true);
    ui->wMeasurerPlot->replot();

    ui->lbMeasurerStatus->setText(QString("Status: X = %1; Y = %2")
                                  .arg(key)
                                  .arg(value));
    qDebug()<<QString("Status: X = %1; Y = %2")
              .arg(key)
              .arg(value);



    mCurrentValue += ui->sbStep->value();
    setCurrentValue();
}

void MainWindow::deleteMeasureData()
{
    ui->wMeasurerPlot->clearGraphs();
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
    out<<"X\tY\r\n";
    out<<";In case Current_vs_Count trace it mean X == I (uA)\r\n";
    out<<";In case CmpLevel_vs_Count trace it mean X == U_cmp (V)\r\n";
    for (int j = 0; j < ui->wMeasurerPlot->graphCount(); ++j){
        out<<"Graph "<<j<<"\r\n";
        QList<QCPData> data =  ui->wMeasurerPlot->graph(j)->data()->values();

        for (int i = 0; i<data.count(); i++){
            out<<(data[i].key)<<'\t'<<data[i].value<<"\r\n";
        }
    }
    m_File.close();

}

void MainWindow::enableControlsAtMeasure(bool value)
{
    ui->sbStart->setEnabled(value);
    ui->sbStep->setEnabled(value);
    ui->sbStop->setEnabled(value);
    ui->sbTrigger->setEnabled(value);
    ui->cbType->setEnabled(value);
    ui->pbStart->setEnabled(value);
    ui->lwActions->setEnabled(value);
}

void MainWindow::on_pbSetI_clicked()
{
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    float value = ui->sbI->value()*1e-6;
    if (tmpSspdM0)
        tmpSspdM0->current()->setValueSync(value, nullptr, 5);
    if (tmpSspdM1)
        tmpSspdM1->current()->setValueSync(value, nullptr, 5);
}

void MainWindow::on_pbSetCmp_clicked()
{
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    float value = ui->sbCmp->value();
    if (tmpSspdM0)
        tmpSspdM0->cmpReferenceLevel()->setValueSync(value, nullptr, 5);
    if (tmpSspdM1)
        tmpSspdM1->cmpReferenceLevel()->setValueSync(value, nullptr, 5);
}

void MainWindow::on_cbShort_clicked(bool checked)
{
    bool ok = false;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0)
        tmpSspdM0->shortEnable()->setValueSync(checked, &ok);
    if (tmpSspdM1)
        tmpSspdM1->shortEnable()->setValueSync(checked, &ok);
    if (!ok) qDebug()<<"can't set short";
}

void MainWindow::on_cbAmplifier_clicked(bool checked)
{
    bool ok = false;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0)
        tmpSspdM0->amplifierEnable()->setValueSync(checked, &ok);
    if (tmpSspdM1)
        tmpSspdM1->amplifierEnable()->setValueSync(checked, &ok);
    if (!ok) qDebug()<<"can't set amplifier";
}

void MainWindow::on_cbComparator_clicked(bool checked)
{
    bool ok = false;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0){
        auto status = tmpSspdM0->status()->getValueSync(&ok);
        if  (!ok){
            qDebug()<<"can't get Driver Status";
            return;
        }
        if (checked) {
            status.stComparatorOn = true;
            status.stRfKeyToCmp = true;
            status.stCounterOn = true;
        }
        else{
            status.stComparatorOn = false;
            status.stRfKeyToCmp = false;
            status.stCounterOn = false;
        }
        tmpSspdM0->status()->setValueSync(status, &ok, 5);
    }
    if (tmpSspdM1){
        auto status = tmpSspdM1->status()->getValueSync(&ok);
        if  (!ok){
            qDebug()<<"can't get Driver Status";
            return;
        }
        if (checked) {
            status.stComparatorOn = true;
            status.stCounterOn = true;
        }
        else{
            status.stComparatorOn = false;
            status.stCounterOn = false;
        }
        tmpSspdM1->status()->setValueSync(status, &ok, 5);
    }
    if (!ok) qDebug()<<"can't set device status";
}

void MainWindow::on_pbReadParams_clicked()
{
    qDebug()<<"MainWindow::on_pbReadParams_clicked";
    bool ok = false;
    float value = 0;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0){
        auto params = tmpSspdM0->params()->getValueSync(&ok, 5);
        if (!ok){
            qDebug()<<"can't read params";
            return;
        }
        value = params.Time_Const;
    }
    if (tmpSspdM1){
        auto params = tmpSspdM1->params()->getValueSync(&ok, 5);
        if (!ok){
            qDebug()<<"can't read params";
            return;
        }
        value = params.Time_Const;
    }

    ui->sbTimeConst->setValue(value);
}

void MainWindow::on_pbSetParams_clicked()
{
    qDebug()<<"MainWindow::on_pbSetParams_clicked";
    mTimerCount_Interval = static_cast<int>(ui->sbTimeConst->value()*1000.0);
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0)
        tmpSspdM0->timeConst()->setValueSync(static_cast<float>(mTimerCount_Interval)/1000.0f, nullptr, 5);
    if (tmpSspdM1)
        tmpSspdM1->timeConst()->setValueSync(static_cast<float>(mTimerCount_Interval)/1000.0f, nullptr, 5);

    mTimer->setInterval(qMax(100, mTimerCount_Interval));
}

void MainWindow::on_pbStart_clicked()
{
    qDebug()<<"MainWindow::on_pbStart_clicked";
    // Запуск процесса измерения
    ui->wMeasurerPlot->addGraph();
    QCPGraph *graph = ui->wMeasurerPlot->graph(ui->wMeasurerPlot->graphCount() - 1);
    graph->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle myScatter;
    myScatter.setShape(QCPScatterStyle::ssDiamond);
    myScatter.setPen(QPen(QApplication::palette().highlight(), 3));
    myScatter.setBrush(QApplication::palette().base());
    myScatter.setSize(9);
    graph->setScatterStyle(myScatter);

    enableControlsAtMeasure(false);

    mCurrentValue = ui->sbStart->value();

    // включаем закоротку
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM0){
        tmpSspdM0->shortEnable()->setValueSync(true, nullptr, 5);
    }
    if (tmpSspdM1){
        tmpSspdM1->shortEnable()->setValueSync(true, nullptr, 5);
    }

    // устанавливаем начальное значение
    switch (ui->cbType->currentIndex()) {
    case 0:
        //Bias
        if (tmpSspdM0){
            tmpSspdM0->current()->setValueSync(static_cast<float>(mCurrentValue*1E-6), nullptr, 5);
        }
        if (tmpSspdM1){
            tmpSspdM1->current()->setValueSync(static_cast<float>(mCurrentValue*1E-6), nullptr, 5);
        }
        break;
    case 1:
        //Cmp  ref level
        if (tmpSspdM0){
            tmpSspdM0->cmpReferenceLevel()->setValueSync(static_cast<float>(mCurrentValue), nullptr, 5);
        }
        if (tmpSspdM1){
            tmpSspdM1->cmpReferenceLevel()->setValueSync(static_cast<float>(mCurrentValue), nullptr, 5);
        }
        break;
    default:
        return;
    }

    // включаем задержку
    QElapsedTimer elapsTimer;
    elapsTimer.start();
    while (elapsTimer.elapsed()<2000)
        qApp->processEvents();

    // раскорачиваем и далее уже как обычно.
    if (tmpSspdM0){
        tmpSspdM0->shortEnable()->setValueSync(false, nullptr, 5);
        auto params = tmpSspdM0->params()->getValueSync();
        mTimer->setInterval(static_cast<int>(static_cast<double>(params.Time_Const)*2000.0));

        auto status = tmpSspdM0->status()->getValueSync();
        status.stAmplifierOn = true;
        status.stComparatorOn = true;
        status.stRfKeyToCmp = true;
        status.stCounterOn = true;

        tmpSspdM0->status()->setValueSync(status);
    }
    if (tmpSspdM1){
        tmpSspdM1->shortEnable()->setValueSync(false, nullptr, 5);
        auto params = tmpSspdM1->params()->getValueSync();
        mTimer->setInterval(static_cast<int>(static_cast<double>(params.Time_Const)*2000.0));

        auto status = tmpSspdM1->status()->getValueSync();
        status.stAmplifierOn = true;
        status.stComparatorOn = true;
        status.stCounterOn = true;

        tmpSspdM1->status()->setValueSync(status);
    }

    disconnect(mTimer, nullptr, nullptr, nullptr);

    connect(mTimer, SIGNAL(timeout()), SLOT(addPoint2MeasureGraphs()));
    setCurrentValue();
}

void MainWindow::on_pbStop_clicked()
{
    // Остановка процесса измерения
    mTimer->stop();
    enableControlsAtMeasure(true);
}

void MainWindow::on_cbType_currentIndexChanged(int index)
{
    switch (index){
    case 0:
        ui->lbStart->setText("Start, uA");
        ui->lbStop->setText("Stop, uA");
        ui->lbStep->setText("Step, uA");
        ui->lbTrigger->setText("Trigger, V");
        break;
    case 1:
        ui->lbStart->setText("Start, V");
        ui->lbStop->setText("Stop, V");
        ui->lbStep->setText("Step, V");
        ui->lbTrigger->setText("Trigger, V");
        break;
    default:
        break;
    }
}

void MainWindow::on_pbGetSecretParams_clicked()
{
    qDebug()<<"MainWindow::on_pbGetSecretParams_clicked";
    bool ok = false;
    bool tmpBool = false;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);

    if (tmpSspdM0)
        tmpBool = tmpSspdM0->PIDEnableStatus()->getValueSync(&ok, 5);
    if (tmpSspdM1)
        tmpBool = tmpSspdM1->PIDEnableStatus()->getValueSync(&ok, 5);

    if (!ok){
        ui->lbSecretStatus->setText("Error at PIDEnableStatus()->getValueSequence");
        return;
    }
    ui->cbPID->setChecked(tmpBool);

    ui->lbSecretStatus->setText("Success");
}

void MainWindow::on_pbSetSecretParams_clicked()
{
    bool ok = false;
    auto tmpSspdM0 = qobject_cast<SspdDriverM0*>(mDriver);
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);

    if (tmpSspdM0)
        tmpSspdM0->PIDEnableStatus()->setValueSync(ui->cbPID->isChecked(), &ok, 5);
    if (tmpSspdM1)
        tmpSspdM1->PIDEnableStatus()->setValueSync(ui->cbPID->isChecked(), &ok, 5);

    if (!ok){
        ui->lbSecretStatus->setText("Error at PIDEnableStatus()->setValueSequence");
        return;
    }
    ui->lbSecretStatus->setText("Success");
}

void MainWindow::on_pbReading_clicked()
{
    qDebug()<<"MainWindow::on_pbReading_clicked";
    mTimer->stop();
    disconnect(mTimer, nullptr, nullptr, nullptr);
    if (ui->pbReading->isChecked()){
        mTimer->setInterval(500);
        connect(mTimer, SIGNAL(timeout()), SLOT(updateSecureData()));
        updateSecureData();
        mTimer->start();
    }
}

void MainWindow::on_pbRecording_clicked()
{
    if (ui->pbRecording->isChecked()){
        mFileName = QFileDialog::getSaveFileName(this,
                                                 tr("Save File..."),
                                                 "output.txt",
                                                 tr("Text files (*.txt)"));
        if (mFileName.isEmpty())
            ui->pbRecording->setChecked(false);
    }
}

void MainWindow::updateSecureData()
{
    static int i = 0;
    qDebug()<<i++;
    bool ok = false;
    QString labelString, fileString;
    if (deviceType == dtSspdM0){
        auto data = qobject_cast<SspdDriverM0*>(mDriver)->data()->getValueSync(&ok, 5);
        if (ok){
            labelString = QString("I: %1 uA<br>U: %2 mV<br>Count: %3")
                    .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                    .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2)
                    .arg(static_cast<double>(data.Counts), 6, 'f', 2);
            fileString = QString("%1\t%2\t%3\r\n")
                    .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                    .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2)
                    .arg(static_cast<double>(data.Counts), 6, 'f', 2);
        }
    }
    else if (deviceType == dtSspdM1){
        auto data = qobject_cast<SspdDriverM1*>(mDriver)->data()->getValueSync(&ok, 5);
        if (ok){
            labelString = QString("I: %1 uA<br>Imon: %2 uA<br>U: %3 mV<br>Count: %4")
                    .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                    .arg(static_cast<double>(data.CurrentMonitor)*1e6, 6,'f', 1)
                    .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2)
                    .arg(static_cast<double>(data.Counts), 6, 'f', 2);
            fileString = QString("%1\t%2\t%3\t%4\r\n")
                    .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                    .arg(static_cast<double>(data.CurrentMonitor)*1e6, 6,'f', 1)
                    .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2)
                    .arg(static_cast<double>(data.Counts), 6, 'f', 2);
        }
    }

    if (ok){
        ui->lbSecretStatus->setText(labelString);
        if (ui->pbRecording->isChecked()){
            QFile m_File(mFileName);
            m_File.open(QIODevice::WriteOnly | QIODevice::Append);
            QTextStream out(&m_File);
            out<<fileString;
            m_File.close();
        }
    }
    mTimer->start();
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

void MainWindow::on_cbHFMode_clicked(bool checked)
{
    bool ok = false;
    auto tmpSspdM1 = qobject_cast<SspdDriverM1*>(mDriver);
    if (tmpSspdM1)
        tmpSspdM1->highFrequencyModeEnable()->setValueSync(checked, &ok);
    if (!ok) qDebug()<<"can't set HF Mode";
}


