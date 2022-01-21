#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <QWidget>
#include "Drivers/drivers.h"

namespace Ui {
class EditWidget;
}

class EditWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditWidget(QWidget *parent = nullptr);
    ~EditWidget() override;

    void setCurrent(double value);
    double getCurrent();
    void setChecked(bool value);
    bool getChecked();

    void setDriver(CommonDriver *newDriver);

private slots:
    void on_pushButton_clicked(bool checked);
    void on_doubleSpinBox_editingFinished();

private:
    Ui::EditWidget *ui;
    cuIOInterface *interface{};
    CommonDriver *mDriver;
};

#endif // EDITWIDGET_H
