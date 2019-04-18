#ifndef SSPDWIDGET_H
#define SSPDWIDGET_H

#include <QWidget>
#include "Drivers/ccu4sdm0driver.h"

namespace Ui {
class SspdWidget;
}

class SspdWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SspdWidget(QWidget *parent = nullptr);
    ~SspdWidget();

    void updateWidget();
    void setDriver(cCu4SdM0Driver *driver);
    void openWidget();
    void closeWidget();

public:

signals:
    void dataUpdated(CU4SDM0V1_Data_t data);
private:
    Ui::SspdWidget *ui;
    cCu4SdM0Driver *mDriver;
private slots:
    void updateData(CU4SDM0V1_Data_t data);
    void paramsUpdated(CU4SDM0V1_Param_t params);
    void on_cbShort_clicked(bool checked);
    void on_cbAmplifier_clicked(bool checked);
    void on_cbComparator_clicked(bool checked);
    void on_pbSetI_clicked();
    void on_pbSetCmp_clicked();
    void on_cbAutoreset_clicked(bool checked);
    void on_pbSetParams_clicked();
};

#endif // SSPDWIDGET_H
