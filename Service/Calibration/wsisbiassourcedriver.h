#ifndef WSISBIASSOURCEDRIVER_H
#define WSISBIASSOURCEDRIVER_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/commondriver.h"
#include "calibratedialog.h"

namespace Ui {
class wSisBiasSourceDriver;
}

class wSisBiasSourceDriver : public DriverWidget
{
    Q_OBJECT

public:
    explicit wSisBiasSourceDriver(QWidget *parent = nullptr);
    ~wSisBiasSourceDriver();

private:
    Ui::wSisBiasSourceDriver *ui;

    // DriverWidget interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual void enableGUI(bool enable) override;
    virtual void getEeprom() override;
    virtual void setEeprom() override;
    virtual QJsonObject eepromToJson() override;
    virtual void eepromFromJson(QJsonObject data) override;
};

#endif // WSISBIASSOURCEDRIVER_H
