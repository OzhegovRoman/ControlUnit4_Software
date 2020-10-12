#ifndef SSPDWIDGET_H
#define SSPDWIDGET_H

#include <QWidget>
#include "commonwidget.h"

#include "Drivers/sspddriverm0.h"

namespace Ui {
class SspdWidget;
}

class SspdWidget : public CommonWidget
{
    Q_OBJECT

public:
    explicit SspdWidget(QWidget *parent = nullptr);
    ~SspdWidget() override;

    void setDriver(SspdDriverM0 *driver);

    void updateWidget() override;
    void openWidget() override;

signals:
    void dataUpdated(CU4SDM0V1_Data_t data);
private:
    Ui::SspdWidget *ui;
    SspdDriverM0 *mDriver{nullptr};
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
