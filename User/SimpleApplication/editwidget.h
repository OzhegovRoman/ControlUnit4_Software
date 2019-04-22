#ifndef EDITWIDGET_H
#define EDITWIDGET_H

#include <QWidget>
#include "Interfaces/cuiointerface.h"

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

    void setInterface(cuIOInterface *value);
    void setIndex(int value);

private slots:
    void on_pushButton_clicked(bool checked);

    void on_doubleSpinBox_editingFinished();

private:
    Ui::EditWidget *ui;
    cuIOInterface *interface{};
    int index{};

};

#endif // EDITWIDGET_H
