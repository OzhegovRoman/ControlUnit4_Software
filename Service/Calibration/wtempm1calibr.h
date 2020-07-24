#ifndef WTEMPM1CALIBR_H
#define WTEMPM1CALIBR_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/tempdriverm1.h"
#include "wtempm1supportwidget.h"


namespace Ui {
class wTempM1Calibr;
}

class wTempM1Calibr : public DriverWidget
{
    Q_OBJECT

public:
    explicit wTempM1Calibr(QWidget *parent = nullptr);
    ~wTempM1Calibr();

private:
    Ui::wTempM1Calibr *ui;
    TempDriverM1 *mTempDriver;
    wTempM1SupportWidget* mSupportWidget[4];
    // DriverWidget interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual void enableGUI(bool enable) override;
    virtual void getEeprom() override;
    virtual void setEeprom() override;
    virtual QJsonObject eepromToJson() override;
    virtual void eepromFromJson(QJsonObject data) override;
};

#endif // WTEMPM1CALIBR_H
