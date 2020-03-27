#include "ftwidget.h"
#include <QThread>

FTWidget::FTWidget(QObject *parent)
    : QObject(parent)
    , mFt801 (nullptr)
    , mTerminateFlag (false)
    , mSleepTimer (50)
    , mTimer(new QTimer(this))
{
    mTimer->setSingleShot(true);
    connect(mTimer, &QTimer::timeout, this, &FTWidget::loop);
}

FT801_SPI *FTWidget::ft801() const
{
    return mFt801;
}

void FTWidget::setFt801(FT801_SPI *fT801)
{
    mFt801 = fT801;
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


