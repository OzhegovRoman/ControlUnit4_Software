#ifndef WTEMPCALIBR_H
#define WTEMPCALIBR_H

#include <QWidget>
#include "driverwidget.h"
#include "Drivers/tempdriverm0.h"

namespace Ui {
class wTempCalibr;
}

class wTempCalibr : public DriverWidget
{
    Q_OBJECT

public:
    explicit wTempCalibr(QWidget *parent = nullptr);
    ~wTempCalibr() override;

    CU4TDM0V1_EEPROM_Const_t getEepromConst();
    CU4TDM0V1_Temp_Table_Item_t *getTempTable();


public slots:
    void onEepromConstReceived(CU4TDM0V1_EEPROM_Const_t eepromConst);
    void setTempTable(CU4TDM0V1_Temp_Table_Item_t* tempTable);

private slots:
    void on_pbLoadFromFile_clicked();

private:
    Ui::wTempCalibr *ui;
    CU4TDM0V1_Temp_Table_Item_t mTempTable[TEMP_TABLE_SIZE]{};
    TempDriverM0 *mTempDriver;

    // DriverWidget interface
public:
    virtual void setDriver(CommonDriver *driver) override;
    virtual void enableGUI(bool enable = true) override;
    virtual void getEeprom() override;
    virtual void setEeprom() override;
    virtual QJsonObject eepromToJson() override;
    virtual void eepromFromJson(QJsonObject data) override;
};

#endif // WTEMPCALIBR_H
