#ifndef WSSPDDRIVER_H
#define WSSPDDRIVER_H

#include <QWidget>
#include "Drivers/ccu4sdm0driver.h"

namespace Ui {
class wSspdDriver;
}

class wSspdDriver : public QWidget
{
    Q_OBJECT

public:
    explicit wSspdDriver(QWidget *parent = nullptr);
    ~wSspdDriver() override;

    CU4SDM0V1_EEPROM_Const_t getEepromConst();
    CU4SDM1_EEPROM_Const_t getEepromM1Const();

public slots:
    void onEepromConstReceived(CU4SDM0V1_EEPROM_Const_t eepromConst);
    void onEepromConstReceived(CU4SDM1_EEPROM_Const_t eepromConst);

private:
    Ui::wSspdDriver *ui;

};

#endif // WSSPDDRIVER_H
