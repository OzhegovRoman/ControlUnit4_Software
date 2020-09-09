#ifndef COMMONCALIBRATOR_H
#define COMMONCALIBRATOR_H

#include <QObject>
#include <QThread>
#include "Drivers/commondriver.h"
#include "cagilent34401avisainterface.h"
#include "lineregression.h"

struct sData{
    double dac;
    double adc;
    double agilent;
};

// Блин хотел все сделать на QThread, было бы красивее, но драйвер не работает, интерфейс туда же

class CommonCalibrator: public QObject
{
    Q_OBJECT
public:
    explicit CommonCalibrator(QObject* parent = nullptr);

    virtual void setDriver(CommonDriver* driver) {Q_UNUSED(driver)}
    virtual QStringList modeList() {return QStringList();}

    int modeIndex() const;
    void setModeIndex(int modeIndex);

    cAgilent34401aVisaInterface *agilent() const;
    void setAgilent(cAgilent34401aVisaInterface *agilent);

    int pointCount() const;
    void setPointCount(int pointCount);

    int checkingPointCount() const;
    void setCheckingPointCount(int checkingPointCount);

    void msleep(uint ms);
    void terminate();
    void start();

    virtual QString driverType() {return QString();}

private:
    int mModeIndex;
    cAgilent34401aVisaInterface *mAgilent;
    int mPointCount;
    int mCheckingPointCount;
    QVector<sData> mData;
    bool stopFlag;

    void performEepromConsts();
    void run();

protected:
    virtual void performAgilent(){agilent()->setMode(cAgilent34401A::Mode_U_DC);}
    virtual void performDriver() {}
    virtual void saveEepromConsts(){}
    virtual void restoreEepromConsts(){}
    virtual void performAdcConsts(){}
    virtual void performDacConsts(){}
    virtual void setDefaultValue();
    virtual double defaultValue(){return 0.0;}
    virtual void setDacValue(double value){Q_UNUSED(value)};
    virtual double readAdcValue(){return 0.0;}
    virtual void setNewDacEepromCoeffs(lineRegressionCoeff coeffs){Q_UNUSED(coeffs)}
    virtual void setNewAdcEepromCoeffs(lineRegressionCoeff coeffs){Q_UNUSED(coeffs)}
    virtual void finish();

signals:
    void changeProgress(int);
    void addPoint(int graphIndex, double x, double y, bool update);
    void message(QString);
    void finished();
    void performPlot();

};

#endif // COMMONCALIBRATOR_H
