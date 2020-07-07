#ifndef FTWIDGET_H
#define FTWIDGET_H

#include <QObject>
#include <QTimer>
#include "riverdieve.h"

class FTWidget : public QObject
{
    Q_OBJECT
public:
    explicit FTWidget(QObject *parent = nullptr);

    int sleepTimer() const;
    void setSleepTimer(int sleepTimer);

    bool isStoped() const;

    Gpu_Hal_Context_t *host() const;
    void setHost(Gpu_Hal_Context_t *host);
    
public slots:
    void exec();
    void terminate();
    
signals:
    
protected:
    virtual void setup() = 0;
    virtual void loop();

private:
    Gpu_Hal_Context_t *mHost;
    bool mTerminateFlag;
    int mSleepTimer;
    QTimer *mTimer;
};

#endif // FTWIDGET_H
