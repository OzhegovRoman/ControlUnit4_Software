#include "ftwidget.h"
#include <QThread>

FTWidget::FTWidget(QObject *parent)
    : QObject(parent)
    , mHost (nullptr)
    , mTerminateFlag (false)
    , mSleepTimer (50)
    , mTimer(new QTimer(this))
{
    mTimer->setSingleShot(true);
    connect(mTimer, &QTimer::timeout, this, &FTWidget::loop);
}

void FTWidget::exec()
{
    mTerminateFlag = false;
    setup();
    // сделать возможность прервать работу
    // реализайия отрисовки асинхронна
    loop();
    // по выходу послать сигнал окончания работы
}

void FTWidget::terminate()
{
    mTerminateFlag = true;
}

void FTWidget::loop()
{
    // перезапускаем таймер
    // к этому моменту должно быть выполнено все операции
    if (!mTerminateFlag)
        mTimer->start(mSleepTimer);
}

Gpu_App_Context_t *FTWidget::host() const
{
    return mHost;
}

void FTWidget::setHost(Gpu_App_Context_t *host)
{
    mHost = host;
}

bool FTWidget::isStoped() const
{
    return mTerminateFlag;
}

int FTWidget::sleepTimer() const
{
    return mSleepTimer;
}

void FTWidget::setSleepTimer(int sleepTimer)
{
    mSleepTimer = sleepTimer;
}


