#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"
#include "Drivers/sisbiassourcedriverm0.h"

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
    mInterface->setPortName("COM3");
#endif

    updateControlUnitList();

    for (int i = 0; i < ui->cbTcpIpAddress->count(); i++){
        if (ui->cbTcpIpAddress->itemText(i) == mLastTcpIpAddress){
            ui->cbTcpIpAddress->setCurrentIndex(i);
            break;
        }
    }
    mTimer->setSingleShot(true);

    ui->wMeasurerPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->wMeasurerPlot, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(measurerContextMenuRequest(QPoint)));

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

    ui->leStartSweep->setValidator(new QDoubleValidator(this));
    ui->leStepSweep->setValidator(new QDoubleValidator(this));
    ui->leStopSweep->setValidator(new QDoubleValidator(this));
    ui->leTimeStepSweep->setValidator(new QIntValidator(this));
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

void MainWindow::addPoint2MeasureGraphs()
{
    qDebug()<<"MainWindow::addPoint2MeasureGraphs";
    if (mStopFlag) {
        enableControlsAtMeasure(true);
        return;
    }

    auto data = mDriver->data()->getValueSync(nullptr, 5);

    double key = data.voltage;
    double value = data.current;

    ui->wMeasurerPlot->graph(ui->wMeasurerPlot->graphCount() - 1)->addData(key, value);
    ui->wMeasurerPlot->rescaleAxes(true);
    ui->wMeasurerPlot->replot();

    ui->lbMeasurerStatus->setText(QString("Status: X = %1; Y = %2")
                                  .arg(key)
                                  .arg(value));
    qDebug()<<QString("Status: X = %1; Y = %2")
              .arg(key)
              .arg(value);


    mCurrentValue += mCurrentStep;

    if ((mCurrentMax == mCurrentMin) && (
                ((mCurrentStep > 0) && (mCurrentValue > mCurrentMax)) ||
                ((mCurrentStep < 0) && (mCurrentValue < mCurrentMin))
                )){
        enableControlsAtMeasure(true);
        return;
    }

    if ((mCurrentValue > mCurrentMax) && (mCurrentStep > 0)){
        mCurrentStep = - mCurrentStep;
        mCurrentValue += 2*mCurrentStep;
        mCurrentMax = ui->leStartSweep->text().toDouble();
    }

    if ((mCurrentValue < mCurrentMin) && (mCurrentStep < 0)){
        mCurrentStep = - mCurrentStep;
        mCurrentValue += 2*mCurrentStep;
        mCurrentMin = mCurrentMax;
    }

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
    out<<"; X == U (mV)\r\n";
    out<<"; Y == I (V)\r\n";
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
    ui->wMeasurerPlot->addGraph();
    QCPGraph *graph = ui->wMeasurerPlot->graph(ui->wMeasurerPlot->graphCount() - 1);
    graph->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle myScatter;
    int colorindex = ((ui->wMeasurerPlot->graphCount() - 1) % 10);
    QColor colors[10] = {QColor(0xc0392b),
                         QColor(0x2980b9),
                         QColor(0xd35400),
                         QColor(0x27ae60),
                         QColor(0xf39c12),
                         QColor(0x8e44ad),
                         QColor(0x16a085),
                         QColor(0xe74c3c),
                         QColor(0x3498db),
                         QColor(0xe67e22)
                        };
    myScatter.setShape(QCPScatterStyle::ssDiamond);
    myScatter.setPen(QPen(colors[colorindex], 3));
    myScatter.setBrush(QBrush(colors[colorindex]));
    myScatter.setSize(9);
    graph->setScatterStyle(myScatter);

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

    //    mCurrentValue   = ui->leStartSweep->text().toDouble();
    //    mCurrentStep    = ui->leStepSweep->text().toDouble();
    //    mCurrentMin     = ui->leMinSweep->text().toDouble();
    //    mCurrentMax     = ui->leMaxSweep->text().toDouble();
    //    if ((mCurrentValue < mCurrentMin) || (mCurrentValue > mCurrentMax))
    //        return;

    //    enableControlsAtMeasure(false);
    //    // если вниз, то переворачиваем знак.

    //    switch (ui->cbType->currentIndex()) {
    //    case 0: // Up
    //    case 3:
    //        mCurrentMin = mCurrentMax;
    //        break;
    //    case 1: //Down
    //    case 4:
    //        mCurrentMax = mCurrentMin;
    //        mCurrentStep *= -1;
    //        break;
    //    }

    //    mVoltageMode = ui->cbType->currentIndex() < 3;

    //    // устанавливаем 0
    //    mDriver->current()->setValueSync(0, nullptr, 5);
    //    mDriver->voltage()->setValueSync(0, nullptr, 5);

    //    // включаем закоротку
    //    mDriver->shortEnable()->setValueSync(true, nullptr, 5);

    //    // устанавливаем правильный режим смещения
    //    mDriver->mode()->setValueSync((ui->cbType->currentIndex()<3) ? CU4BSM0_UMODE : CU4BSM0_IMODE,
    //                                  nullptr, 5);

    //    // снимаем закоротку
    //    mDriver->shortEnable()->setValueSync(false, nullptr, 5);

    //    // устанавливаем начальное значение
    //    if (mVoltageMode) {
    //        mDriver->voltage()->setValueSync(static_cast<float>(mCurrentValue * 1E-3), nullptr, 5);
    //    }
    //    else{
    //        mDriver->current()->setValueSync(static_cast<float>(mCurrentValue * 1E-6), nullptr, 5);
    //    }

    //    // включаем задержку
    //    QElapsedTimer elapsTimer;
    //    elapsTimer.start();
    //    while (elapsTimer.elapsed()<100)
    //        qApp->processEvents();

    //    disconnect(mTimer, nullptr, nullptr, nullptr);

    //    mTimer->setInterval(10);

    //    connect(mTimer, SIGNAL(timeout()), SLOT(addPoint2MeasureGraphs()));
    //    setCurrentValue();
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

        QSettings settings("Scontel", "ControlUnit4_Calibration");
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

            double key = tmp->voltage;
            double value = tmp->current;

            ui->wMeasurerPlot->graph(ui->wMeasurerPlot->graphCount() - 1)->addData(key, value);
            ui->wMeasurerPlot->rescaleAxes(true);
            ui->wMeasurerPlot->replot();

            //qDebug()<<tmp->current<<tmp->voltage<<tmp->time;
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

