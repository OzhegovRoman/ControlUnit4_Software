#ifndef DRIVERWIDGET_H
#define DRIVERWIDGET_H

#include <QWidget>
#include "Drivers/commondriver.h"
#include <QJsonObject>

class DriverWidget: public QWidget
{
public:
    explicit DriverWidget(QWidget *parent = nullptr);

    CommonDriver *driver() const;
    virtual void setDriver(CommonDriver *driver);

    virtual void enableGUI(bool enable = true) = 0;
    void disableGUI(){enableGUI(false);}

    virtual void getEeprom() = 0;
    virtual void setEeprom() = 0;
    virtual QJsonObject eepromToJson() = 0;
    virtual void eepromFromJson(QJsonObject data) = 0;

private:
    CommonDriver* mDriver;

};

#endif // DRIVERWIDGET_H
