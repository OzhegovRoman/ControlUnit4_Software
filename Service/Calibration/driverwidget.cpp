#include "driverwidget.h"

DriverWidget::DriverWidget(QWidget *parent):
    QWidget(parent)
{

}

CommonDriver *DriverWidget::driver() const
{
    return mDriver;
}

void DriverWidget::setDriver(CommonDriver *driver)
{
    mDriver = driver;
}
