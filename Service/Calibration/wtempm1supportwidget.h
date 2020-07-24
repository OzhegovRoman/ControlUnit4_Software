#ifndef WTEMPM1SUPPORTWIDGET_H
#define WTEMPM1SUPPORTWIDGET_H

#include <QWidget>

#include "Drivers/tempdriverm1.h"

namespace Ui {
class wTempM1SupportWidget;
}

class wTempM1SupportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit wTempM1SupportWidget(QWidget *parent = nullptr);
    ~wTempM1SupportWidget();

    TDM1_EepromCoeff eepromCoeff();
    void setEepromCoeff(TDM1_EepromCoeff coeff);

    DefaultParam getDefaultParam();
    void setDefaultParam(DefaultParam param);

    void setTemperatureTableItem(CU4TDM0V1_Temp_Table_Item_t item, uint8_t index);
    void setTemperatureTable(CU4TDM0V1_Temp_Table_Item_t* item);
    CU4TDM0V1_Temp_Table_Item_t* temperatureTable();

private slots:
    void on_pbLoadTable_clicked();

private:
    Ui::wTempM1SupportWidget *ui;
    CU4TDM0V1_Temp_Table_Item_t mTempTable[TEMP_TABLE_SIZE];
};

#endif // WTEMPM1SUPPORTWIDGET_H
