#ifndef WSISCONTROLLINEDRIVER_H
#define WSISCONTROLLINEDRIVER_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/commondriver.h"
#include "calibratedialog.h"

namespace Ui {
class wSisControlLineDriver;
}

class wSisControlLineDriver : public DriverWidget
{
    Q_OBJECT

public:
    explicit wSisControlLineDriver(QWidget *parent = nullptr);
    ~wSisControlLineDriver();

private:
    Ui::wSisControlLineDriver *ui;

    // DriverWidget interface
public:
    virtual void enableGUI(bool enable) override;
    virtual void getEeprom() override;
    virtual void setEeprom() override;
    virtual QJsonObject eepromToJson() override;
    virtual void eepromFromJson(QJsonObject data) override;
    virtual void setDriver(CommonDriver *driver) override;
};

#endif // WSISCONTROLLINEDRIVER_H
