#ifndef WSSPDDRIVER_H
#define WSSPDDRIVER_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/commondriver.h"
#include "calibratedialog.h"

namespace Ui {
class wSspdDriver;
}

class wSspdDriver : public DriverWidget
{
    Q_OBJECT

public:
    explicit wSspdDriver(QWidget *parent = nullptr);
    ~wSspdDriver() override;

private:
    Ui::wSspdDriver *ui;
    CU4DriverType mDeviceType;

    // DriverWidget interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual void enableGUI(bool enable = true) override;
    virtual void getEeprom() override;
    virtual void setEeprom() override;
    virtual QJsonObject eepromToJson() override;
    virtual void eepromFromJson(QJsonObject data) override;
};

#endif // WSSPDDRIVER_H
