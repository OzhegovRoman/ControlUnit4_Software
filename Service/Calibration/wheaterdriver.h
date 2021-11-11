#ifndef WHEATERDRIVER_H
#define WHEATERDRIVER_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/commondriver.h"
#include "calibratedialog.h"

namespace Ui {
class wHeaterDriver;
}

class wHeaterDriver : public DriverWidget
{
    Q_OBJECT

public:
    explicit wHeaterDriver(QWidget *parent = nullptr);
    ~wHeaterDriver();

private:
    Ui::wHeaterDriver *ui;

    // DriverWidget interface
public:
    virtual void enableGUI(bool enable) override;
    virtual void getEeprom() override;
    virtual void setEeprom() override;
    virtual QJsonObject eepromToJson() override;
    virtual void eepromFromJson(QJsonObject data) override;

    // DriverWidget interface
public:
    virtual void setDriver(CommonDriver *driver) override;
};

#endif // WHEATERDRIVER_H
