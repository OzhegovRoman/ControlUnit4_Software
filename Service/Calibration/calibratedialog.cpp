#include "calibratedialog.h"
#include "ui_calibratedialog.h"
#include "qcustomplot.h"
#include "lineregression.h"
#include "QElapsedTimer"
#include <QSettings>
#include "Calibrator/cu4sdm0calibrator.h"
#include "Calibrator/cu4sdm1calibrator.h"
#include "Calibrator/cu4tdm0calibrator.h"
#include "Calibrator/cu4tdm1calibrator.h"
#include "Calibrator/cu4htm0calibrator.h"

CalibrateDialog::CalibrateDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CalibrateDialog)
    , agilent(new cAgilent34401aVisaInterface())
    , mCalibrator(nullptr)
    , mChannel(0)

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

    mCalibrator->setModeIndex(ui->cbMode->currentIndex());
    agilent->setVisaDeviceName(ui->leVisaName->text());
    mCalibrator->setAgilent(agilent);

    // Все спрячем
    enableComponents(false);
    mCalibrator->start();
}

void CalibrateDialog::messagePerform(QString str)
{
    qDebug()<<"message:"<<str;
    ui->lStatus->setText(QString("Status: %1").arg(str));
}

void CalibrateDialog::changeProgress(int progress)
{
    ui->progressBar->setValue(progress);
}

void CalibrateDialog::appendPoints(int graphIndex, double x, double y, bool update)
{
    if (graphIndex >= ui->dataPlot->graphCount())
        return;
    ui->dataPlot->graph(graphIndex)->addData(x, y);
    if (update) {
        ui->dataPlot->rescaleAxes(true);
        ui->dataPlot->replot();
    }
}

void CalibrateDialog::stopCalibration()
{
    mCalibrator->terminate();
}

int CalibrateDialog::channel() const
{
    return mChannel;
}

void CalibrateDialog::setChannel(int channel)
{
    mChannel = channel;
}

CU4DriverType CalibrateDialog::driverType() const
{
    return mDriverType;
}

void CalibrateDialog::setDriverType(const CU4DriverType &driverType)
{
    mDriverType = driverType;
}

void CalibrateDialog::enableComponents(bool state)
{
    ui->cbMode->setEnabled(state);
    ui->leVisaName->setEnabled(state);
    ui->pbStart->setEnabled(state);
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

CommonDriver *CalibrateDialog::driver() const
{
    return mDriver;
}

void CalibrateDialog::setDriver(CommonDriver *driver)
{
    mDriver = driver;
}

int CalibrateDialog::exec()
{
    if (!agilent->isVisaInited()){
        return -1;
    }

    enableComponents(true);

    if (mCalibrator!= nullptr){
        mCalibrator->disconnect();
        mCalibrator->deleteLater();
    }

    switch (mDriverType) {
    case dtSspdDriverM0:
        mCalibrator = new CU4SDM0Calibrator(this);
        break;
    case dtSspdDriverM1:
        mCalibrator = new CU4SDM1Calibrator(this);
        break;
    case dtTempDriverM0:
        mCalibrator = new CU4TDM0Calibrator(this);
        break;
    case dtTempDriverM1:
        mCalibrator = new CU4TDM1Calibrator(this);
        qobject_cast<CU4TDM1Calibrator * >(mCalibrator)->setChannel(mChannel);
        break;
    case dtHeaterDriver:
        mCalibrator = new CU4HTM0Calibrator(this);
        break;
    default:
        mCalibrator = new CommonCalibrator(this);
    }

    connect(mCalibrator, &CommonCalibrator::changeProgress, this, &CalibrateDialog::changeProgress);
    connect(mCalibrator, &CommonCalibrator::finished, this, [=](){enableComponents(true);});
    connect(mCalibrator, &CommonCalibrator::addPoint, this, &CalibrateDialog::appendPoints);
    connect(mCalibrator, &CommonCalibrator::message, this, &CalibrateDialog::messagePerform);
    connect(mCalibrator, &CommonCalibrator::performPlot, this, &CalibrateDialog::plotPerform);

    mCalibrator->setDriver(mDriver);

    QSettings settings("Scontel", "ControlUnit4_Calibration");
    if (settings.value("DriverType",QString()).toString() == mCalibrator->driverType()){
        mCalibrator->setModeIndex(settings.value(mCalibrator->driverType()+"_Mode", 0).toInt());
    }

    ui->cbMode->clear();
    ui->cbMode->addItems(mCalibrator->modeList());

    ui->cbMode->setCurrentIndex(mCalibrator->modeIndex());
    ui->leVisaName->setText(settings.value("VisaName",QString()).toString());

    mDriver->init();
    return QDialog::exec();
}
