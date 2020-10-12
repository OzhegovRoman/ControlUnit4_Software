#ifndef SSPDWIDGETM1_H
#define SSPDWIDGETM1_H

#include <QWidget>
#include "commonwidget.h"

#include "Drivers/sspddriverm1.h"

namespace Ui {
class SspdWidgetM1;
}

class SspdWidgetM1 : public CommonWidget
{
    Q_OBJECT

public:
    explicit SspdWidgetM1(QWidget *parent = nullptr);
    ~SspdWidgetM1() override;

    void setDriver(SspdDriverM1 *driver);

    void updateWidget() override;
    void openWidget() override;

signals:
    void dataUpdated(CU4SDM1_Data_t data);
private:
    Ui::SspdWidgetM1 *ui;
    SspdDriverM1 *mDriver{nullptr};
private slots:
    void updateData(CU4SDM1_Data_t data);
    void paramsUpdated(CU4SDM1_Param_t params);
    void on_cbShort_clicked(bool checked);
    void on_cbAmplifier_clicked(bool checked);
    void on_pbSetI_clicked();
    void on_pbSetCmp_clicked();
    void on_cbAutoreset_clicked(bool checked);
    void on_pbSetParams_clicked();
    void on_cbHFMode_clicked(bool checked);
    void on_cbCounter_clicked(bool checked);
};

#endif // SSPDWIDGETM1_H
