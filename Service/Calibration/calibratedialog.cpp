#include "calibratedialog.h"
#include "ui_calibratedialog.h"
#include "qcustomplot.h"
#include "lineregression.h"
#include "QElapsedTimer"
#include <QSettings>

#define NPoint 200
#define NPointCheck 20

//TODO: расширить на другие приборы и другие модули

CalibrateDialog::CalibrateDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CalibrateDialog)
    , agilent(new cAgilent34401aVisaInterface())
{
    ui->setupUi(this);
    connect(this, SIGNAL(rejected()), this, SLOT(stopCalibration()));
}

CalibrateDialog::~CalibrateDialog()
{
    delete ui;
}

// Сделан линейный алгоритм,
// Для наглядности сделан уход от системы signal-slot
void CalibrateDialog::on_pbStart_clicked()
{
    // сохраняем введенные настройки
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    settings.setValue("VisaName", ui->leVisaName->text());
    switch (mDriverType) {
    case (tDriverType::CU4SDM0):
        settings.setValue("DriverType","CU4SDM0");
        settings.setValue("SspdDriver_M0_Mode",ui->cbMode->currentIndex());
        break;
    case (tDriverType::CU4SDM1):
        settings.setValue("DriverType","CU4SDM1");
        settings.setValue("SspdDriver_M1_Mode",ui->cbMode->currentIndex());
        break;
    case (tDriverType::CU4TD):
        settings.setValue("DriverType","CU4TD");
        settings.setValue("TempDriverMode",ui->cbMode->currentIndex());
        break;
    default:
        break;
    }

    // Инициализируем плагин на калибровку.
    if (mDriver == nullptr) return; //ничего не получится

    // Все спрячем
    enableComponents(false);

    ui->progressBar->setValue(0);

    for (int i = 0; i<2; ++i){
        x[i].clear();
        y[i].clear();
    }

    // Подготовим QCustomPlot
    plotPerform();

    // настроим Agilent
    agilentPerform();

    // настроим драйвер устройства
    switch (mDriverType) {
    case(tDriverType::CU4SDM0):
    {
        cCu4SdM0Driver *tmpDriver = qobject_cast<cCu4SdM0Driver*>(mDriver);
        if (!tmpDriver){ //ничего не получится
            return;
        }
        // отключаем PID
        tmpDriver->PIDEnableStatus()->setValueSequence(false, nullptr, 5);
        tmpDriver->setShortEnable(false);
        tmpDriver->waitingAnswer();
        break;
    }

        //TODO: реализация получения драйвера
    case(tDriverType::CU4SDM1):
        break;

    case(tDriverType::CU4TD):
    {

        cCu4TdM0Driver *tmpDriver = qobject_cast<cCu4TdM0Driver*>(mDriver);
        if (!tmpDriver){ //ничего не получится
            return;
        }
        tmpDriver->commutatorTurnOn(true);
        tmpDriver->waitingAnswer();
        break;
    }
    default:
        break;
    }

    messagePerform("Start calibration progress");

    // обновим константы и проинициализируем EepromConst
    eepromConstInitialize();

    // устанавливаем 0 и ждем 2 секунды
    qDebug()<<"Set 0";
    setCurrent(0);
    sleep(2000);

    // Итак начинаем сам процесс калировки
    messagePerform("Start calibration progress");
    calibrationFirstStep();

    if (!stopFlag){

        // Удаляем 10% "плохих" точек
        deleteBadPoints();

        // делаем проверку нуля.
        agilentCheckZero();
    }

    // все
    enableComponents(true);
}

void CalibrateDialog::messagePerform(QString str)
{
    ui->lStatus->setText(QString("Status: %1").arg(str));
}

void CalibrateDialog::agilentCheckZero()
{
    messagePerform("checking Zero");
    ui->progressBar->setValue(0);

    pair_t<float> emptyCoeff;
    emptyCoeff.first = 1;
    emptyCoeff.second = 0;

    //Загружаем новые константы
    lineRegressionCoeff coeff = lineRegression(x[0], y[0]);

    // устанавливаем новые коэффициенты
    bool ok = false;
    switch (mDriverType) {
    case tDriverType::CU4SDM0 :
    {
        lastCU4SDEepromConst.Current_DAC.first = coeff.slope;
        lastCU4SDEepromConst.Current_DAC.second = coeff.intercept;
        CU4SDM0V1_EEPROM_Const_t tmpEepromConst = lastCU4SDEepromConst;
        if (ui->cbMode->currentIndex() == 0)
            tmpEepromConst.Voltage_ADC = emptyCoeff;
        else
            tmpEepromConst.Current_ADC = emptyCoeff;
        qobject_cast<cCu4SdM0Driver*>(mDriver)->eepromConst()->setValueSequence(tmpEepromConst, &ok, 5);

        setCurrent(0);

        break;
    }
        //TODO: реализовать поведение для нового драйвера
    case tDriverType::CU4SDM1 :
        break;
    case tDriverType::CU4TD :
    {
        lastCU4TDEepromConst.tempSensorCurrentDac.first = coeff.slope;
        lastCU4TDEepromConst.tempSensorCurrentDac.second = coeff.intercept;
        CU4TDM0V1_EEPROM_Const_t tmpEepromConst = lastCU4TDEepromConst;

        switch (ui->cbMode->currentIndex()) {
        case 0:
            tmpEepromConst.pressSensorVoltageP = emptyCoeff;
            break;
        case 1:
            tmpEepromConst.pressSensorVoltageN = emptyCoeff;
            break;
        case 2:
            tmpEepromConst.tempSensorVoltage = emptyCoeff;
            break;
        case 3:
            tmpEepromConst.tempSensorCurrentAdc = emptyCoeff;
            break;
        default:
            break;
        }
        qobject_cast<cCu4TdM0Driver*>(mDriver)->eepromConst()->setValueSequence(tmpEepromConst, &ok, 5);
        setCurrent(1e-5f);
        break;
    }
    default:
        break;
    }

    sleep (2000);
    // устанавливаем ток и ждем 2 секунды
    messagePerform("collect data");

    qreal agData = 0;
    qreal value = 0;

    for (int i = 0; i < NPointCheck; ++i){

        switch (mDriverType) {
        case tDriverType::CU4SDM0 :
        {
            CU4SDM0V1_Data_t data = qobject_cast<cCu4SdM0Driver*>(mDriver)->deviceData()->getValueSequence(&ok, 5);
            value += (ui->cbMode->currentIndex() == 0) ?
                        static_cast<double>(data.Voltage) :
                        static_cast<double>(data.Current);
            break;
        }
            //TODO: реадлизовать поведение дра1йвера
        case tDriverType::CU4SDM1 :
            break;
        case tDriverType::CU4TD :
        {
            CU4TDM0V1_Data_t data = qobject_cast<cCu4TdM0Driver*>(mDriver)->deviceData()->getValueSequence(&ok, 5);
            switch (ui->cbMode->currentIndex()) {
            case 0:
                value += static_cast<double>(data.PressSensorVoltageP);
                break;
            case 1:
                value += static_cast<double>(data.PressSensorVoltageN);
                break;
            case 2:
                value += static_cast<double>(data.TempSensorVoltage);
                break;
            case 3:
                value += static_cast<double>(data.TempSensorCurrent);
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }

        agData = agData + agilent->singleRead();
        ui->progressBar->setValue(static_cast<int>(100.0*i/NPointCheck));
    }
    agData = agData / NPointCheck;
    value = value / NPointCheck;

    //Делаем правки
    QPointF  point;
    switch (mDriverType) {
    case tDriverType::CU4SDM0 :
    {
        // Делаем правку на ЦАП
        point = QPointF(agData, static_cast<double>(lastCU4SDEepromConst.Current_DAC.second));
        coeff = lineRegressionViaPoint(x[0], y[0], point);
        lastCU4SDEepromConst.Current_DAC.first = coeff.slope;
        lastCU4SDEepromConst.Current_DAC.second = coeff.intercept;

        // Делаем правку только на АЦП
        point = QPointF(value, agData);
        coeff = lineRegressionViaPoint(x[1], y[1], point);
        emptyCoeff.first = coeff.slope;
        emptyCoeff.second = coeff.intercept;

        if (ui->cbMode->currentIndex() == 0) {
            lastCU4SDEepromConst.Voltage_ADC = emptyCoeff;
        }
        else
            lastCU4SDEepromConst.Current_ADC = emptyCoeff;
        qobject_cast<cCu4SdM0Driver*>(mDriver)->eepromConst()->setValueSequence(lastCU4SDEepromConst, &ok, 5);

        // включаем обратно PID регулятор
        qobject_cast<cCu4SdM0Driver*>(mDriver)->PIDEnableStatus()->setValueSequence(true, nullptr, 5);
        setCurrent(0);
        break;
    }
        //TODO: реализовать
    case tDriverType::CU4SDM1 :
        break;
    case tDriverType::CU4TD :
    {
        // Делаем правку на ЦАП
        point = QPointF(agData, static_cast<double>(lastCU4TDEepromConst.tempSensorCurrentDac.first * 1e-5f + lastCU4TDEepromConst.tempSensorCurrentDac.second));
        coeff = lineRegressionViaPoint(x[0], y[0], point);
        lastCU4TDEepromConst.tempSensorCurrentDac.first = coeff.slope;
        lastCU4TDEepromConst.tempSensorCurrentDac.second = coeff.intercept;

        // Делаем правку только на АЦП
        point = QPointF(value, agData);
        coeff = lineRegressionViaPoint(x[1], y[1], point);
        emptyCoeff.first = coeff.slope;
        emptyCoeff.second = coeff.intercept;

        switch (ui->cbMode->currentIndex()) {
        case 0:
            lastCU4TDEepromConst.pressSensorVoltageP    = emptyCoeff;
            break;
        case 1:
            lastCU4TDEepromConst.pressSensorVoltageN    = emptyCoeff;
            break;
        case 2:
            lastCU4TDEepromConst.tempSensorVoltage      = emptyCoeff;
            break;
        case 3:
            qDebug()<<"tempSensorCurrentADC";
            lastCU4TDEepromConst.tempSensorCurrentAdc   = emptyCoeff;
            break;
        default:
            break;
        }

        // Ждем ответа
        qobject_cast<cCu4TdM0Driver*>(mDriver)->eepromConst()->setValueSequence(lastCU4TDEepromConst, &ok, 5);
        setCurrent(1e-5f);
        break;
    }
    default:
        break;
    }

    // Ждем ответа
    ui->progressBar->setValue(100);
}

void CalibrateDialog::stopCalibration()
{
    stopFlag = true;
}

tDriverType CalibrateDialog::driverType() const
{
    return mDriverType;
}

void CalibrateDialog::setDriverType(const tDriverType &driverType)
{
    mDriverType = driverType;
}

void CalibrateDialog::enableComponents(bool state)
{
    ui->cbMode->setEnabled(state);
    ui->leVisaName->setEnabled(state);
    ui->pbStart->setEnabled(state);
}

void CalibrateDialog::updateGraphData()
{
    if (isVisible()){
        ui->dataPlot->graph(0)->clearData();
        ui->dataPlot->graph(1)->clearData();

        ui->dataPlot->graph(0)->addData(y[0], x[0]);
        ui->dataPlot->graph(1)->addData(x[1], y[1]);

        ui->dataPlot->rescaleAxes(true);
        ui->dataPlot->replot();
    }
}

// Подготовка графика для построения
void CalibrateDialog::plotPerform()
{
    //clear graphs
    ui->dataPlot->clearGraphs();
    ui->dataPlot->addGraph(ui->dataPlot->xAxis, ui->dataPlot->yAxis);
    ui->dataPlot->graph(0)->setPen(QPen(Qt::red));
    ui->dataPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    ui->dataPlot->addGraph(ui->dataPlot->xAxis2, ui->dataPlot->yAxis);
    ui->dataPlot->graph(1)->setPen(QPen(Qt::blue));
    ui->dataPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    // activate top and right axes, which are invisible by default:
    ui->dataPlot->xAxis2->setVisible(true);

    // set ranges appropriate to show data:
    ui->dataPlot->xAxis->setRange(0, 65535);
}

// Подготовка мультиметра
void CalibrateDialog::agilentPerform()
{
    qDebug()<<"Agilent perform";
    agilent->setVisaDeviceName(ui->leVisaName->text());
    // TODO: выбор режима для SDM1
    if (((mDriverType == tDriverType::CU4SDM0) && (ui->cbMode->currentIndex() == 0)) ||
            ((mDriverType == tDriverType::CU4TD) && (ui->cbMode->currentIndex() < 3)))
        agilent->setMode(cAgilent34401A::Mode_U_DC);
    else
        agilent->setMode(cAgilent34401A::Mode_I_DC);
}

void CalibrateDialog::eepromConstInitialize()
{
    pair_t<float> emptyCoeff;
    emptyCoeff.first = 1;
    emptyCoeff.second = 0;
    bool ok = false;

    //TODO: сделать на все случаи жизни
    switch (mDriverType){
    case (tDriverType::CU4SDM0):
    {
        cCu4SdM0Driver *driver = qobject_cast<cCu4SdM0Driver*>(mDriver);
        CU4SDM0V1_EEPROM_Const_t tmpEepromConst =
                lastCU4SDEepromConst =
                driver->eepromConst()->getValueSequence(&ok, 5);
        if (ok){
            messagePerform("EEPROM Const received");
            tmpEepromConst.Current_ADC =
                    tmpEepromConst.Current_DAC =
                    tmpEepromConst.Voltage_ADC = emptyCoeff;
            ok = false;
            driver->eepromConst()->setValueSequence(tmpEepromConst, &ok, 5);
        }
        break;
    }
        // реализовать
    case (tDriverType::CU4SDM1):
        break;
    case (tDriverType::CU4TD):
    {
        cCu4TdM0Driver *driver = qobject_cast<cCu4TdM0Driver*>(mDriver);
        CU4TDM0V1_EEPROM_Const_t tmpEepromConst =
                lastCU4TDEepromConst =
                driver->eepromConst()->getValueSequence(&ok, 5);
        if (ok){
            messagePerform("EEPROM Const received");
            tmpEepromConst.pressSensorVoltageP =
                    tmpEepromConst.pressSensorVoltageN =
                    tmpEepromConst.tempSensorCurrentAdc =
                    tmpEepromConst.tempSensorCurrentDac =
                    tmpEepromConst.tempSensorVoltage = emptyCoeff;
            ok = false;
            driver->eepromConst()->setValueSequence(tmpEepromConst, &ok, 5);
        }
        break;
    }
    default:
        break;
    }

    if (!ok) {
        messagePerform("ERROR: can't receive/set calibration data");
        stopCalibration();
    }

}

void CalibrateDialog::setCurrent(float value)
{
    // устанавливаем 0 и ждем 2 секунды
    bool ok;
    switch (mDriverType) {
    case tDriverType::CU4SDM0:
    {
        qobject_cast<cCu4SdM0Driver*>(mDriver)->current()->setValueSequence(value, &ok, 5);
        break;
    }
        //TODO: реализовать
    case tDriverType::CU4SDM1:
    {
        break;
    }
    case tDriverType::CU4TD:
    {
        qobject_cast<cCu4TdM0Driver*>(mDriver)->tempSensorCurrent()->setValueSequence(value, &ok, 5);
        break;
    }
    default:
        break;
    }
}

void CalibrateDialog::calibrationFirstStep()
{
    QElapsedTimer timer;
    qreal agData;
    bool ok;
    //    CU4SDM0V1_Data_t data;
    //    cCu4SdM0Driver* driver = qobject_cast<cCu4SdM0Driver*>(mDriver);
    timer.start();
    ui->progressBar->setValue(0);
    stopFlag = false;
    for (int i = 0; i < NPoint; ++i){
        //устанавливаем следующее значение тока
        y[0].append(65535.0/NPoint*i);
        setCurrent(y[0].last());

        // ждем 0.5 секунды для установки значения
        timer.restart();
        while (timer.elapsed()<500){
            qApp->processEvents();
        }

        if (stopFlag) return;

        // считываем значение токов-напряжений
        qDebug()<<"single read";
        agData = agilent->singleRead();
        x[0].append(agData);
        y[1].append(agData);

        switch (mDriverType) {
        case tDriverType::CU4SDM0: {
            CU4SDM0V1_Data_t data = qobject_cast<cCu4SdM0Driver*>(mDriver)->deviceData()->getValueSequence(&ok, 5);
            x[1].append((ui->cbMode->currentIndex() == 0) ? data.Voltage : data.Current);
            break;
        }
            // TODO: реализовать
        case tDriverType::CU4SDM1: {
            break;
        }
        case tDriverType::CU4TD: {
            CU4TDM0V1_Data_t data = qobject_cast<cCu4TdM0Driver*>(mDriver)->deviceData()->getValueSequence(&ok, 5);
            switch (ui->cbMode->currentIndex()) {
            case 0:
                x[1].append(data.PressSensorVoltageP);
                break;
            case 1:
                x[1].append(data.PressSensorVoltageN);
                break;
            case 2:
                x[1].append(data.TempSensorVoltage);
                break;
            case 3:
                x[1].append(data.TempSensorCurrent);
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }

        if (stopFlag) return;

        messagePerform(QString("I: %1/%2, U: %3, Agilent: %4")
                       .arg(x[0].last())
                .arg(y[0].last())
                .arg(x[1].last())
                .arg(y[1].last()));
        ui->progressBar->setValue(static_cast<int>(100.0*(i+1)/NPoint));
        updateGraphData();
    }
}

void CalibrateDialog::deleteBadPoints()
{
    // Удаляем 10% "плохих" точек
    int index;
    messagePerform("Delete 10% of \"bad\" points");
    for (int i = 0; i<NPoint/10; ++i){
        for (int j = 0; j<2; ++j){
            index = findBadestPoint(x[j], y[j]);
            x[j].removeAt(index);
            y[j].removeAt(index);
        }
    }
    updateGraphData();
}

void CalibrateDialog::sleep(int msec)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed()<msec){
        ui->progressBar->setValue(static_cast<int>(timer.elapsed()/20));
        qApp->processEvents();
    }
}

AbstractDriver *CalibrateDialog::driver() const
{
    return mDriver;
}

void CalibrateDialog::setDriver(AbstractDriver *driver)
{
    mDriver = driver;
}

int CalibrateDialog::exec()
{
    if (!agilent->isVisaInited()){
        return -1;
    }

    enableComponents(true);
    QStringList modeList;
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    QString oldDriverType = settings.value("DriverType",QString()).toString();
    int currentIndex = 0;
    switch (mDriverType) {
    case (tDriverType::CU4SDM0):
        modeList<<"U mode"<<"I mode";
        if (oldDriverType == "CU4SDM0") currentIndex = settings.value("SspdDriverMode",0).toInt();
        break;
        //TODO: реализовать
    case (tDriverType::CU4SDM1):
        break;
    case (tDriverType::CU4TD):
        modeList<<"PressSensorVoltageP"<<"PressSensorVoltageN"<<"TempSensorVoltage"<<"TempSensorCurrent";
        if (oldDriverType == "CU4TD") currentIndex = settings.value("TempDriverMode",0).toInt();
        break;
    default:
        break;
    }
    ui->cbMode->clear();
    ui->cbMode->addItems(modeList);

    ui->cbMode->setCurrentIndex(currentIndex);
    ui->leVisaName->setText(settings.value("VisaName",QString()).toString());

    plotPerform();
    mDriver->init();
    return QDialog::exec();
}
