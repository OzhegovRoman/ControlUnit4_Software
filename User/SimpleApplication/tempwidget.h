#ifndef TEMPWIDGET_H
#define TEMPWIDGET_H

#include <QWidget>
#include "Drivers/ccu4tdm0driver.h"

namespace Ui {
class TempWidget;
}

class TempWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TempWidget(QWidget *parent = nullptr);
    ~TempWidget() override;

    void updateWidget();
    void openWidget();
    void closeWidget();

    void setDriver(cCu4TdM0Driver *driver);

private slots:
    void dataUpDated(CU4TDM0V1_Data_t data);

private:
    Ui::TempWidget *ui;
    cCu4TdM0Driver *mDriver{nullptr};
    bool mClosed{true};
};

#endif // TEMPWIDGET_H