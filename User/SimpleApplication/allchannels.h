#ifndef ALLCHANNELS_H
#define ALLCHANNELS_H

#include <QWidget>
#include "allchannelsdatamodel.h"
#include "allchannelsdatadelegate.h"
#include "Interfaces/cuiointerface.h"
#include "Drivers/commondriver.h"
#include "commonwidget.h"

namespace Ui {
class AllChannels;
}

class AllChannels : public CommonWidget
{
    Q_OBJECT

public:
    explicit AllChannels(QWidget *parent = nullptr);
    ~AllChannels() override;

    void openWidget()   override;
    void closeWidget()  override;
    void updateWidget() override;

    void initialize(const QVector<CommonDriver *> &mDrivers);

    void setInterface(cuIOInterface *interface);

private:
    Ui::AllChannels *ui;
signals:
    void setTimeOut(int);

private slots:
   void on_pbSetUpdateTime_clicked();
   void on_pbLogPath_clicked();

private:
    AllChannelsDataModel *model;
    AllChannelsDataDelegate *delegate;
    cuIOInterface *mInterface{nullptr};
};

#endif // ALLCHANNELS_H
