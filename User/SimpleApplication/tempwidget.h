#ifndef TEMPWIDGET_H
#define TEMPWIDGET_H

#include <QWidget>
#include "Drivers/tempdriverm0.h"
#include "commonwidget.h"

namespace Ui {
class TempWidget;
}

class TempWidget : public CommonWidget
{
    Q_OBJECT

public:
    explicit TempWidget(QWidget *parent = nullptr);
    ~TempWidget() override;

    void updateWidget() override;
    void openWidget() override;
    void closeWidget() override;

    void setDriver(TempDriverM0 *driver);

private slots:
    void dataUpDated(CU4TDM0V1_Data_t data);

private:
    Ui::TempWidget *ui;
    TempDriverM0 *mDriver{nullptr};
    bool mClosed{true};
};

#endif // TEMPWIDGET_H
