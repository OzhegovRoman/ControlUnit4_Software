#include "nicescale.h"
#include <qmath.h>

NiceScale::NiceScale(double min, double max)
    : mMinPoint(min)
    , mMaxPoint(max)
{
    calculate();
}

double NiceScale::maxTicks() const
{
    return mMaxTicks;
}

void NiceScale::setMaxTicks(double newMaxTicks)
{
    mMaxTicks = newMaxTicks;
    calculate();
}

double NiceScale::min() const
{
    return mMinPoint;
}

void NiceScale::setMin(double min)
{
    mMinPoint = min;
    calculate();
}

double NiceScale::max() const
{
    return mMaxPoint;
}

void NiceScale::setMax(double max)
{
    mMaxPoint = max;
    calculate();
}

void NiceScale::setMinMaxPoints(double min, double max)
{
    mMinPoint = min;
    mMaxPoint = max;
    calculate();
}

double NiceScale::tickSpacing() const
{
    return mTickSpacing;
}

double NiceScale::niceMin() const
{
    return mNiceMin;
}

double NiceScale::niceMax() const
{
    return mNiceMax;
}

int NiceScale::tickCount() const
{
    return mTickCount;
}

void NiceScale::calculate()
{
    mRange = niceNum(mMaxPoint - mMinPoint, false);
    mTickSpacing = niceNum(mRange / (mMaxTicks - 1), true);
    mNiceMin =
            qFloor(mMinPoint / mTickSpacing) * mTickSpacing;
    mNiceMax =
            qCeil(mMaxPoint / mTickSpacing) * mTickSpacing;
    mTickCount = (mNiceMax-mNiceMin)/ mTickSpacing + 1;
}

double NiceScale::niceNum(double range, bool round)
{
    double exponent; /** exponent of range */
    double fraction; /** fractional part of range */
    double niceFraction; /** nice, rounded fraction */

    exponent = qFloor(qLn(range)/qLn(10));
    fraction = range / qPow(10, exponent);

    if (round) {
        if (fraction < 1.5)
            niceFraction = 1;
        else if (fraction < 3)
            niceFraction = 2;
        else if (fraction < 7)
            niceFraction = 5;
        else
            niceFraction = 10;
    } else {
        if (fraction <= 1)
            niceFraction = 1;
        else if (fraction <= 2)
            niceFraction = 2;
        else if (fraction <= 5)
            niceFraction = 5;
        else
            niceFraction = 10;
    }

    return niceFraction * qPow(10, exponent);
}
