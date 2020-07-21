#include "wtempm1calibr.h"
#include "ui_wtempm1calibr.h"

wTempM1Calibr::wTempM1Calibr(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::wTempM1Calibr)
{
    ui->setupUi(this);
}

wTempM1Calibr::~wTempM1Calibr()
{
    delete ui;
}
