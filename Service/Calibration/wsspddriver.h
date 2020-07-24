#ifndef WSSPDDRIVER_H
#define WSSPDDRIVER_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/sspddriverm0.h"

namespace Ui {
class wSspdDriver;
}

class wSspdDriver : public DriverWidget
{
    Q_OBJECT

public:
    explicit wSspdDriver(QWidget *parent = nullptr);
    ~wSspdDriver() override;

    CU4SDM0V1_EEPROM_Const_t getEepromConst();

public slots:
    void onEepromConstReceived(CU4SDM0V1_EEPROM_Const_t eepromConst);

private:
    Ui::wSspdDriver *ui;
    SspdDriverM0* mSspdDriver;

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
