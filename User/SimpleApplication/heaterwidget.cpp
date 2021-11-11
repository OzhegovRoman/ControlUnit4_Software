#include "heaterwidget.h"
#include "ui_heaterwidget.h"
#include <QDoubleValidator>
#include <QMouseEvent>
#include <QDebug>

HeaterWidget::HeaterWidget(QWidget *parent)
    : CommonWidget(parent)
    , ui(new Ui::HeaterWidget)
{
    ui->setupUi(this);

    ui->leMaxCurrent    -> setValidator(new QDoubleValidator());
    ui->leFrontEdge     -> setValidator(new QDoubleValidator());
    ui->leHoldTime      -> setValidator(new QDoubleValidator());
    ui->leRearEdge      -> setValidator(new QDoubleValidator());

    ProgressBarBaseStyle = QString("QProgressBar {"
                                   "border: 2px solid grey;"
                                   "border-radius: 5px;"
                                   "background-color: %1;"
                                   "}"
                                   "QProgressBar::chunk {"
                                   "background-color: %2;"
                                   "width: 1px;"
                                   "}")
            .arg(QApplication::palette().base().color().name())
            .arg("#39BFEF");

    ProgressBarPressedStyle = QString("QProgressBar {"
                                      "border: 2px solid grey;"
                                      "border-radius: 5px;"
                                      "background-color: %1;"
                                      "}"
                                      "QProgressBar::chunk {"
                                      "background-color: %2;"
                                      "width: 1px;"
                                      "}")
            .arg("#FF4A11")
            .arg("#FF8411");

    ui->progressBar->setStyleSheet(ProgressBarBaseStyle);
}

HeaterWidget::~HeaterWidget()
{
    delete ui;
}

HeaterDriverM0 *HeaterWidget::driver() const
{
    return mDriver;
}

void HeaterWidget::setDriver(HeaterDriverM0 *newDriver)
{
    mDriver = newDriver;
}

void HeaterWidget::startHeating()
{
    heatingProgress = 0;

    auto eepromConst = mDriver->eepromConst()->currentValue();
    eepromConst.maximumCurrent = ui->leMaxCurrent   -> text().toDouble();
    eepromConst.frontEdgeTime  = ui->leFrontEdge    -> text().toDouble();
    eepromConst.holdTime       = ui->leHoldTime     -> text().toDouble();
    eepromConst.rearEdgeTime   = ui->leRearEdge     -> text().toDouble();

    bool ok = false;
    mDriver->eepromConst()->setValueSync(eepromConst, &ok, 5);
    mDriver->eepromConst()->setCurrentValue(eepromConst);

    if (!ok) return;
    mDriver->startHeating()->executeSync(&ok, 5);

    if (!ok) return;

    waitingTime = (eepromConst.frontEdgeTime + eepromConst.holdTime + eepromConst.rearEdgeTime) * 1000.0;
    ui->progressBar->setMaximum(waitingTime);
    ui->progressBar->setFormat("Heaiting. Hold to stop...");
    heatingProgress = 0;
    QTimer::singleShot(10, this, &HeaterWidget::updateHeatingProgress);
}

void HeaterWidget::updateProgressBar()
{
    if (mMousePressed) {
        currentProgress +=10;
        if (currentProgress <= ui->progressBar->maximum())
            QTimer::singleShot(10, this, &HeaterWidget::updateProgressBar);
        else {
            mMousePressed = false;
            ui->progressBar->setStyleSheet(ProgressBarBaseStyle);
            if ((heatingProgress > 0 )  && (heatingProgress < waitingTime - mDriver->eepromConst()->currentValue().rearEdgeTime * 1000.0)){
                //ToDo: imediately stop
                bool ok;
                mDriver->emergencyStop()->executeSync(&ok, 10);
                heatingProgress = waitingTime - mDriver->eepromConst()->currentValue().rearEdgeTime * 1000.0;
                qDebug()<<"imediately stop";
                ui->progressBar->setFormat("Wait...");
                qDebug()<<heatingProgress;
                qDebug()<<waitingTime;
                ui->progressBar->setMaximum(waitingTime);
                ui->progressBar->setValue(heatingProgress);
            }
            else startHeating();
        }
    }
    else
        currentProgress = 0;
    ui->progressBar->setValue(currentProgress);
}

void HeaterWidget::updateHeatingProgress()
{
    heatingProgress += 10;
    if (heatingProgress <= waitingTime)
        QTimer::singleShot(10, this, &HeaterWidget::updateHeatingProgress);
    else {
        heatingProgress = 0;
        ui->progressBar->setFormat("Hold to Start Heating...");
    }
    if (!mMousePressed){
        ui->progressBar->setMaximum(waitingTime);
        ui->progressBar->setValue(heatingProgress);
    }
}

void HeaterWidget::closeWidget()
{

}

void HeaterWidget::openWidget()
{
    bool ok = false;
    mDriver->eepromConst()->getValueSync(&ok, 5);
    if (ok){
        ui->leMaxCurrent    -> setText(QString::number(mDriver->eepromConst()->currentValue().maximumCurrent));
        ui->leFrontEdge     -> setText(QString::number(mDriver->eepromConst()->currentValue().frontEdgeTime ));
        ui->leHoldTime      -> setText(QString::number(mDriver->eepromConst()->currentValue().holdTime      ));
        ui->leRearEdge      -> setText(QString::number(mDriver->eepromConst()->currentValue().rearEdgeTime  ));
    }
}

void HeaterWidget::updateWidget()
{
}

void HeaterWidget::mousePressEvent(QMouseEvent *event)
{
    if (this->childAt(event->pos()) == ui->progressBar){
        qDebug()<<"Press event....";
        ui->progressBar->setStyleSheet(ProgressBarPressedStyle);
        currentProgress = 0;
        ui->progressBar->setMaximum(500);
        mMousePressed = true;
        QTimer::singleShot(10, this, &HeaterWidget::updateProgressBar);
    }
}

void HeaterWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMousePressed = false;
    QPalette p = QApplication::palette();
    ui->progressBar->setPalette(p);
    ui->progressBar->setStyleSheet(ProgressBarBaseStyle);
}
