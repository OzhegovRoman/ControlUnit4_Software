#ifndef SISBIASSOURCEWIDGET_H
#define SISBIASSOURCEWIDGET_H

#include <QWidget>
#include "commonwidget.h"
#include "Drivers/sisbiassourcedriverm0.h"
#include <QTimer>


namespace Ui {
class SisBiasSourceWidget;
}

class SisBiasSourceWidget : public CommonWidget
{
    Q_OBJECT

public:
    explicit SisBiasSourceWidget(QWidget *parent = nullptr);
    ~SisBiasSourceWidget();

signals:
    void dataUpdated(CU4BSM0_Data_t data);

private:
    Ui::SisBiasSourceWidget *ui;
    SisBiasSourceDriverM0 *mDriver;

    // CommonWidget interface
public:
    virtual void closeWidget() override;
    virtual void openWidget() override;
    virtual void updateWidget() override;

    SisBiasSourceDriverM0 *driver() const;
    void setDriver(SisBiasSourceDriverM0 *newDriver);

private slots:
    void updateData(CU4BSM0_Data_t data);

    void on_cbShort_clicked(bool checked);
    void on_pbSetU_clicked();
    void on_pbSetI_clicked();
    void on_rbU_toggled(bool checked);
};

#endif // SISBIASSOURCEWIDGET_H
