#ifndef WTEMPM1CALIBR_H
#define WTEMPM1CALIBR_H

#include <QWidget>

namespace Ui {
class wTempM1Calibr;
}

class wTempM1Calibr : public QWidget
{
    Q_OBJECT

public:
    explicit wTempM1Calibr(QWidget *parent = nullptr);
    ~wTempM1Calibr();

private:
    Ui::wTempM1Calibr *ui;
};

#endif // WTEMPM1CALIBR_H
