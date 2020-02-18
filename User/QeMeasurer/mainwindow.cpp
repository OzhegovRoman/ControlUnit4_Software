#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSettings>
#include "../qCustomLib/qCustomLib.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    mDriver(nullptr),
    mInterface(new IOInterface_t(this)),
    mTimer(new QTimer(this))
{
    ui->setupUi(this);

    QSettings settings("Scontel","QeMeasurer");
    mLastTcpIpAddress = settings.value("TcpAddress",QString()).toString();
    ui->sbDeviceAddress->setValue(settings.value("DeviceAddress",0).toInt());

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
#elif defined(RASPBERRY_PI)
    mInterface->setPortName("ttyAMA0");
#endif

    qDebug()<<"test_01";
    updateControlUnitList();
    qDebug()<<"test_02";

    for (int i = 0; i < ui->cbTcpIpAddress->count(); i++){
        if (ui->cbTcpIpAddress->itemText(i) == mLastTcpIpAddress){
            ui->cbTcpIpAddress->setCurrentIndex(i);
            break;
        }
    }
    mTimer->setSingleShot(true);

    ui->wCounterPlot->addGraph();
    ui->wCounterPlot->graph(0)->setPen(QPen(Qt::blue));
    ui->wCounterPlot->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->wCounterPlot->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->wCounterPlot->xAxis->setAutoTickStep(false);
    ui->wCounterPlot->xAxis->setTickStep(10);
    ui->wCounterPlot->axisRect()->setupFullAxesBox();

    ui->wMeasurerPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    // setup policy and connect slot for context menu popup:
    ui->wMeasurerPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->wMeasurerPlot, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(contextMenuRequest(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::contextMenuRequest(QPoint pos)
{

    if (ui->wMeasurerPlot->graphCount() == 0) return;

    auto *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    menu->addAction("Clear", this, SLOT(deleteMeasureData()));
    menu->addAction("Save data", this, SLOT(saveData()));

    menu->popup(ui->wMeasurerPlot->mapToGlobal(pos));
}

void MainWindow::on_pbInitialize_clicked()
{
    // сначала нужно получить информацию от модуля и выбрать подходящий тип
    CommonDriver driver;
    driver.setIOInterface(mInterface);
    bool secretMode = QGuiApplication::queryKeyboardModifiers() == (Qt::ControlModifier | Qt::ShiftModifier);

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

    mDriver = new SspdDriverM0(this);
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

    if (secretMode){
        tmpStr.append("<br>Secret Mode Activated");
        ui->lwActions->addItem("Secret Params");
        ui->lwActions->item(4)->setFont(ui->lwActions->item(3)->font());
    }
    ui->lbStatus->setText(tmpStr);
    for (int i = 0; i<4; ++i) {
        ui->lwActions->item(i)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    on_pbReadParams_clicked();

    QSettings settings("Scontel","QeMeasurer");
    mLastTcpIpAddress = ui->cbTcpIpAddress->currentText();
    settings.setValue("TcpAddress", mLastTcpIpAddress);
    settings.setValue("DeviceAddress", ui->sbDeviceAddress->value());
}

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    bool ok;

    CU4SDM0V1_Param_t params;
    CU4SDM0_Status_t status;
    mMode = arg1;
    mTimer->stop();

    disconnect(mTimer, nullptr, nullptr, nullptr);

    switch (arg1) {
    case 1: // Инициализация
        mTimer->setInterval(500);
        connect(mTimer, SIGNAL(timeout()), SLOT(updateData()));
        updateData();
        break;
    case 2:
        params = mDriver->params()->getValueSync(&ok);
        mTimerCount_Interval = static_cast<int>(static_cast<double>(params.Time_Const)*1000.0);
        mTimer->setInterval(qMax(100, mTimerCount_Interval));
        qDebug()<<"Time_Const:"<<params.Time_Const;

        status = mDriver->status()->getValueSync(&ok);
        status.Data = status.Data | CU4SDM0V1_STATUS_COUNTER_WORKED;
        mDriver->status()->setValueSync(status);

        connect(mTimer, SIGNAL(timeout()), SLOT(updateCountsGraph()));
        updateCountsGraph();
        break;
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
    auto data = mDriver->data()->getValueSync(&ok);
    if (ok){
        ui->lbData->setText(QString("I: %1 uA<br>U: %2 mV")
                            .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                            .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2));
        ui->cbAmplifier->setChecked(data.Status.stAmplifierOn);
        ui->cbShort->setChecked(data.Status.stShorted);
        ui->cbComparator->setChecked(data.Status.stComparatorOn);
        qDebug()<<"new data. Status:" << data.Status.Data;
        qDebug()<<"new data. Comparator:" << data.Status.stComparatorOn;
    }
    else qDebug()<<"error at getting data";
    mTimer->start();
}

void MainWindow::updateCountsGraph()
{
    bool ok;
    auto counts = static_cast<double>(mDriver->counts()->getValueSync(&ok, 5));
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
    ui->wCounterPlot->replot();

    ui->lbCounts->setText(QString("Counts per second: %1").arg(counts));

    mTimer->start();
}

void MainWindow::setCurrentValue()
{
    if (mCurrentValue > ui->sbStop->value()) {
        on_pbStop_clicked();
        return;
    }

    switch (ui->cbType->currentIndex()) {
    case 0:
        //Bias
        mDriver->current()->setValueSync(static_cast<float>(mCurrentValue*1E-6), nullptr, 5);
        break;
    case 1:
        //Cmp  ref level
        mDriver->cmpReferenceLevel()->setValueSync(static_cast<float>(mCurrentValue), nullptr, 5);
        break;
    default:
        return;
    }
    mTimer->start();
}

void MainWindow::addPoint2MeasureGraphs()
{
    double key = 0;
    double value = 0;
    auto data = mDriver->data()->getValueSync(nullptr, 5);
    value =  static_cast<double>(data.Counts/mDriver->params()->currentValue().Time_Const);
    if (static_cast<double>(data.Voltage) > ui->sbTrigger->value()){ //триггер сработал
        on_pbStop_clicked();
        return;
    }
    key = static_cast<double>(data.Current)*1E6;

    if (ui->cbType->currentIndex() !=0 )
        key = mCurrentValue;


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
    mDriver->current()->setValueSync(static_cast<float>(ui->sbI->value()*1E-6), nullptr, 5);
}

void MainWindow::on_pbSetCmp_clicked()
{
    mDriver->cmpReferenceLevel()->setValueSync(static_cast<float>(ui->sbCmp->value()), nullptr, 5);
}

void MainWindow::on_cbShort_clicked(bool checked)
{
    bool ok = false;
    mDriver->shortEnable()->setValueSync(checked, &ok);
    if (!ok) qDebug()<<"can't set short";
}

void MainWindow::on_cbAmplifier_clicked(bool checked)
{
    bool ok = false;
    mDriver->amplifierEnable()->setValueSync(checked, &ok);
    if (!ok) qDebug()<<"can't set amplifier";
}

void MainWindow::on_cbComparator_clicked(bool checked)
{
    bool ok;
    CU4SDM0_Status_t status = mDriver->status()->getValueSync(&ok);
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
    qDebug()<<"setted status:"<<status.Data;
    mDriver->status()->setValueSync(status, &ok, 5);
    if (!ok) qDebug()<<"can't set device status";
}

void MainWindow::on_pbReadParams_clicked()
{
    bool ok = false;
    auto params = mDriver->params()->getValueSync(&ok, 5);
    if (!ok){
        qDebug()<<"can't read params";
        return;
    }
    ui->sbTimeConst->setValue(static_cast<double>(params.Time_Const));
}

void MainWindow::on_pbSetParams_clicked()
{
    mTimerCount_Interval = static_cast<int>(ui->sbTimeConst->value()*1000.0);
    mDriver->timeConst()->setValueSync(static_cast<float>(mTimerCount_Interval)/1000.0f, nullptr, 5);
    mTimer->setInterval(qMax(100, mTimerCount_Interval));
}

void MainWindow::on_pbStart_clicked()
{
    // Запуск процесса измерения
    ui->wMeasurerPlot->addGraph();
    QCPGraph *graph = ui->wMeasurerPlot->graph(ui->wMeasurerPlot->graphCount() - 1);
    graph->setLineStyle(QCPGraph::lsNone);
    QCPScatterStyle myScatter;
    myScatter.setShape(QCPScatterStyle::ssDiamond);
    myScatter.setPen(QPen(Qt::blue));
    myScatter.setBrush(Qt::white);
    myScatter.setSize(5);
    graph->setScatterStyle(myScatter);

    enableControlsAtMeasure(false);

    mCurrentValue = ui->sbStart->value();

    // включаем закоротку

    mDriver->shortEnable()->setValueSync(true, nullptr, 5);

    // устанавливаем начальное значение
    switch (ui->cbType->currentIndex()) {
    case 0:
        //Bias
        mDriver->current()->setValueSync(static_cast<float>(mCurrentValue*1E-6), nullptr, 5);
        break;
    case 1:
        //Cmp  ref level
        mDriver->cmpReferenceLevel()->setValueSync(static_cast<float>(mCurrentValue), nullptr, 5);
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
    mDriver->shortEnable()->setValueSync(false, nullptr, 5);

    // настроим постоянную времени
    auto params = mDriver->params()->getValueSync();
    mTimer->setInterval(static_cast<int>(static_cast<double>(params.Time_Const)*2000.0));

    auto status = mDriver->status()->getValueSync();
    status.Data |= CU4SDM0V1_STATUS_COUNTER_WORKED;
    mDriver->status()->setValueSync(status);

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
    bool ok;
    bool tmpBool = mDriver->PIDEnableStatus()->getValueSync(&ok, 5);
    if (!ok){
        ui->lbSecretStatus->setText("Error at PIDEnableStatus()->getValueSequence");
        return;
    }
    ui->cbPID->setChecked(tmpBool);

    ui->lbSecretStatus->setText("Success");
}

void MainWindow::on_pbSetSecretParams_clicked()
{
    bool ok;
    mDriver->PIDEnableStatus()->setValueSync(ui->cbPID->isChecked(), &ok, 5);
    if (!ok){
        ui->lbSecretStatus->setText("Error at PIDEnableStatus()->setValueSequence");
        return;
    }
    ui->lbSecretStatus->setText("Success");
}

void MainWindow::on_pbReading_clicked()
{
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
        qDebug()<<mFileName;
        if (mFileName.isEmpty())
            ui->pbRecording->setChecked(false);
    }
}

void MainWindow::updateSecureData()
{
    static int i = 0;
    qDebug()<<i++;
    bool ok;
    auto data = mDriver->data()->getValueSync(&ok);
    if (ok){
        ui->lbSecretStatus->setText(QString("I: %1 uA<br>U: %2 mV<br>Count: %3")
                                    .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                                    .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2)
                                    .arg(static_cast<double>(data.Counts), 6, 'f', 2));
        if (ui->pbRecording->isChecked()){
            QFile m_File(mFileName);
            m_File.open(QIODevice::WriteOnly | QIODevice::Append);
            QTextStream out(&m_File);
            out<<QString("%1\t%2\t%3\r\n")
                 .arg(static_cast<double>(data.Current)*1e6, 6,'f', 1)
                 .arg(static_cast<double>(data.Voltage)*1e3, 6, 'f', 2)
                 .arg(static_cast<double>(data.Counts), 6, 'f', 2);
            m_File.close();
        }
    }
    else qDebug()<<"error at getting data";

    mTimer->start();
}

void MainWindow::updateControlUnitList()
{
    setDisabled(true);
    qApp->processEvents();
    QStringList list = availableControlUnits();
    qDebug()<<"test1";
    ui->cbTcpIpAddress->clear();
    if (!list.isEmpty())
        ui->cbTcpIpAddress->addItems(list);
    ui->cbTcpIpAddress->addItems(QStringList()<<"Update..."<<"Manual...");
    qDebug()<<"test2";

    setEnabled(true);
}

void MainWindow::on_cbTcpIpAddress_activated(const QString &arg1)
{
    qDebug()<<"activated";
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
