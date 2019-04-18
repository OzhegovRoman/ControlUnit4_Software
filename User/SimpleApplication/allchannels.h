#ifndef ALLCHANNELS_H
#define ALLCHANNELS_H

#include <QWidget>
#include "allchannelsdatamodel.h"
#include "allchannelsdatadelegate.h"
#include "Interfaces/cuiointerface.h"

namespace Ui {
class AllChannels;
}

class AllChannels : public QWidget
{
    Q_OBJECT

public:
    explicit AllChannels(QWidget *parent = nullptr);
    ~AllChannels();

    void openWidget();
    void closeWidget();
    void updateWidget();

    void initialize(QString deviceInfo);

    void setInterface(cuIOInterface *interface);

private:
    Ui::AllChannels *ui;
signals:
    void setTimeOut(int);

private slots:
   void on_pbSetUpdateTime_clicked();

private:
    AllChannelsDataModel *model;
    AllChannelsDataDelegate *delegate;
    cuIOInterface *mInterface;
};

#endif // ALLCHANNELS_H
