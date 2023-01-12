#ifndef NICESCALE_H
#define NICESCALE_H


class NiceScale
{
public:
    NiceScale(double min, double max);
    double maxTicks() const;
    void setMaxTicks(double newMaxTicks);

    double min() const;
    void setMin(double min);

    double max() const;
    void setMax(double max);

    void setMinMaxPoints(double min, double max);

    double tickSpacing() const;

    double niceMin() const;

    double niceMax() const;

    int tickCount() const;

private:
     double mMinPoint;
     double mMaxPoint;
     double mMaxTicks = 10;

     double mTickSpacing;
     int mTickCount;
     double mRange;
     double mNiceMin;
     double mNiceMax;

     void calculate();
     double niceNum(double range, bool round);
};

#endif // NICESCALE_H
