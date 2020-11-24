#include "commoncalibrator.h"
#include <QDebug>
#include <QSettings>
#include <QElapsedTimer>
#include <QApplication>

CommonCalibrator::CommonCalibrator(QObject *parent)
    : QObject(parent)
    , mModeIndex (0)
    , mPointCount (200)
    , mCheckingPointCount(20)
    , stopFlag(false)
{

}

int CommonCalibrator::modeIndex() const
{
    return mModeIndex;
}

void CommonCalibrator::setModeIndex(int modeIndex)
{
    mModeIndex = modeIndex;
}

cAgilent34401aVisaInterface *CommonCalibrator::agilent() const
{
    return mAgilent;
}

void CommonCalibrator::setAgilent(cAgilent34401aVisaInterface *agilent)
{
    mAgilent = agilent;
}

int CommonCalibrator::pointCount() const
{
    return mPointCount;
}

void CommonCalibrator::setPointCount(int pointCount)
{
    mPointCount = pointCount;
}

int CommonCalibrator::checkingPointCount() const
{
    return mCheckingPointCount;
}

void CommonCalibrator::setCheckingPointCount(int checkingPointCount)
{
    mCheckingPointCount = checkingPointCount;
}

void CommonCalibrator::terminate()
{
    stopFlag = true;
}

void CommonCalibrator::start()
{
    stopFlag = false;
    run();
    emit finished();
}

void CommonCalibrator::performEepromConsts()
{
    saveEepromConsts();
    performAdcConsts();
    performDacConsts();
    restoreEepromConsts();
}

void CommonCalibrator::setDefaultValue()
{
    setDacValue(defaultValue());
    msleep(2000);
}

void CommonCalibrator::finish()
{
    emit finished();
}

void CommonCalibrator::msleep(uint ms)
{
    QElapsedTimer timer;
    timer.start();
    while ((timer.elapsed()<ms) && !stopFlag)
        qApp->processEvents();
}

void CommonCalibrator::run()
{
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    settings.setValue("DriverType", driverType());
    settings.setValue(driverType()+"_Mode", modeIndex());

    emit changeProgress(0);
    emit performPlot();
    msleep(10);

    mData.clear();

    emit message("Performing Agilent...");
    msleep(10);
    performAgilent();

    emit message("Performing Driver...");
    msleep(10);
    performDriver();

    emit message("Performing calibration constants...");
    msleep(10);
    performEepromConsts();

    emit message("Set 0 value...");
    msleep(10);
    setDacValue(0);
    msleep(2000);

    emit message("Start calibration progress");
    msleep(10);
    for (int i = 0; i < mPointCount; ++i){
        if (stopFlag) return;

        sData tmpData;
        tmpData.dac = 65535.0 / mPointCount * i;
        // ждем 0.5 секунды пока установится значение
        setDacValue(tmpData.dac);
        msleep(500);

        tmpData.agilent = agilent()->singleRead();
        tmpData.adc = readAdcValue();
        if (stopFlag) return;
        msleep(20);

        if (stopFlag) return;

        mData.append(tmpData);

        emit message(QString("DAC: %1; ADC: %2; Agilent: %3")
                     .arg(mData.last().dac)
                     .arg(mData.last().adc)
                     .arg(mData.last().agilent));
        emit changeProgress(100.0*(i+1)/mPointCount);

        emit addPoint(0, mData.last().dac, mData.last().agilent, false);
        emit addPoint(1, mData.last().adc, mData.last().agilent, true);
        msleep(10);
    }

    if (stopFlag) return;

    // Подготавливаем массивы точек
    QVector<QPointF> points[2];
    for (int i = 0; i< mData.size(); ++i){
        points[0].append(QPointF(mData[i].agilent, mData[i].dac));
        points[1].append(QPointF(mData[i].adc, mData[i].agilent));
    }

    // удаляем плохие точки
    emit message("Delete 10% of \"bad\" points");
    msleep(10);
    for (int i = 0; i<mPointCount / 10; ++i){
        for (int j = 0; j<2; ++j){
            int index = findBadestPoint(points[j]);
            points[j].removeAt(index);
        }
    }

    // перерисовываем графики
    emit performPlot();
    for (int i = 0; i< points[0].size(); ++i){
        emit addPoint(0, points[0][i].y(), points[0][i].x(), false);
        emit addPoint(1, points[1][i].x(), points[1][i].y(), false);
        msleep(1);
    }
    emit addPoint(1, points[1].last().x(), points[1].last().y(), true);
    msleep(1);

    if (stopFlag) return;

    emit message("Checking default value...");
    emit changeProgress(0);
    msleep(10);

    if (stopFlag) return;
    lineRegressionCoeff tmpDacCoeff = lineRegression(points[0]);

    qDebug()<<"tmpDacCoeff:"<<tmpDacCoeff.slope<<tmpDacCoeff.intercept;
    setNewDacEepromCoeffs(tmpDacCoeff);
    performAdcConsts();
    restoreEepromConsts();

    if (stopFlag) return;
    setDefaultValue();

    emit message("Collect data...");
    double agData = 0;
    double value = 0;

    for (int i = 0; i < mCheckingPointCount; ++i){

        value += readAdcValue();
        agData = agData + agilent()->singleRead();
        emit changeProgress(100.0*i/mCheckingPointCount);
        msleep(10);
    }
    agData = agData / mCheckingPointCount;
    value = value / mCheckingPointCount;

    // Считаем поправку к ЦАП
    setNewDacEepromCoeffs(lineRegressionViaPoint(points[0], QPointF(agData, tmpDacCoeff.slope * defaultValue() + tmpDacCoeff.intercept)));
    // Считаем поправку к АЦП
    setNewAdcEepromCoeffs(lineRegressionViaPoint(points[1], QPointF(value, agData)));
    restoreEepromConsts();

    setDacValue(defaultValue());

    emit changeProgress(100);

    finish();
}
