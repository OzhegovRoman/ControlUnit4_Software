#ifndef HEATERWIDGET_H
#define HEATERWIDGET_H

#include <QWidget>
#include "commonwidget.h"

#include "Drivers/heaterdriverm0.h"
#include <QTimer>

namespace Ui {
class HeaterWidget;
}

class HeaterWidget : public CommonWidget
{
    Q_OBJECT

public:
    explicit HeaterWidget(QWidget *parent = nullptr);
    ~HeaterWidget();

    HeaterDriverM0 *driver() const;
    void setDriver(HeaterDriverM0 *newDriver);

private:
    Ui::HeaterWidget *ui;
    HeaterDriverM0 * mDriver{nullptr};
//    QTimer *progressTimer;
    int currentProgress{0};
    int heatingProgress{0};
    int waitingTime{0};
    QString ProgressBarBaseStyle;
    QString ProgressBarPressedStyle;

    bool mMousePressed;

    void startHeating();

private slots:
    void updateProgressBar();
    void updateHeatingProgress();

    // CommonWidget interface
public:
    virtual void closeWidget() override;
    virtual void openWidget() override;
    virtual void updateWidget() override;

    // QWidget interface
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // HEATERWIDGET_H
