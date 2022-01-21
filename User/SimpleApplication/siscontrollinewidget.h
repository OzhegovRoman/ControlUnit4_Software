#ifndef SISCONTROLLINEWIDGET_H
#define SISCONTROLLINEWIDGET_H

#include <QWidget>
#include "commonwidget.h"

#include "Drivers/siscontrollinedriverm0.h"
#include <QTimer>

namespace Ui {
class SisControlLineWidget;
}

class SisControlLineWidget : public CommonWidget
{
    Q_OBJECT

public:
    explicit SisControlLineWidget(QWidget *parent = nullptr);
    ~SisControlLineWidget();

signals:
    void dataUpdated(CU4CLM0V0_Data_t data);

private:
    Ui::SisControlLineWidget *ui;
    SisControlLineDriverM0 * mDriver{nullptr};

    // CommonWidget interface
public:
    virtual void closeWidget() override;
    virtual void openWidget() override;
    virtual void updateWidget() override;

    SisControlLineDriverM0 *driver() const;
    void setDriver(SisControlLineDriverM0 *newDriver);

private slots:
    void updateData(CU4CLM0V0_Data_t data);
    void on_pbSetI_clicked();
    void on_cbShort_clicked(bool checked);
};



#endif // SISCONTROLLINEWIDGET_H
