#ifndef WTEMPCALIBR_H
#define WTEMPCALIBR_H

#include <QWidget>
#include "Drivers/ccu4tdm0driver.h"

namespace Ui {
class wTempCalibr;
}

class wTempCalibr : public QWidget
{
    Q_OBJECT

public:
    explicit wTempCalibr(QWidget *parent = 0);
    ~wTempCalibr();

    CU4TDM0V1_EEPROM_Const_t getEepromConst();
    CU4TDM0V1_Temp_Table_Item_t *getTempTable();


public slots:
    void onEepromConstReceived(CU4TDM0V1_EEPROM_Const_t eepromConst);
    void setTempTable(CU4TDM0V1_Temp_Table_Item_t* tempTable);

private slots:
    void on_pbLoadFromFile_clicked();

private:
    Ui::wTempCalibr *ui;
    CU4TDM0V1_Temp_Table_Item_t mTempTable[TEMP_TABLE_SIZE];
};

#endif // WTEMPCALIBR_H