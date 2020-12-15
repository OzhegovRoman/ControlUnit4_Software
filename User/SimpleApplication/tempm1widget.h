#ifndef TEMPM1WIDGET_H
#define TEMPM1WIDGET_H

#include <QWidget>
#include "Drivers/tempdriverm1.h"
#include <QTimer>

#include "commonwidget.h"
#include "tempreset_addon.h"

namespace Ui {
class TempM1Widget;
}

class TempM1Widget : public CommonWidget
{
    Q_OBJECT

   TemperatureResetAddon *tempReset;

public:
    explicit TempM1Widget(QWidget *parent = nullptr);
    ~TempM1Widget();

    void updateWidget() override;
    void openWidget() override;
    void closeWidget() override;

    void setTempReset(TemperatureRecycleInterface *value);
    void setDriver(TempDriverM1 *driver);

public slots:
    void onTimerTicker();

private slots:
    void on_cb5V_clicked(bool checked);
    void on_cb25V_clicked(bool checked);

private:
    Ui::TempM1Widget *ui;
    TempDriverM1 *mDriver{nullptr};
    bool mClosed{true};
    QTimer mTimer;
};

#endif // TEMPM1WIDGET_H
