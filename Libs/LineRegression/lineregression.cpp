#include "lineregression.h"
#include <QPointF>

lineRegressionCoeff lineRegression(QVector<double> x, QVector<double> y)
{
    lineRegressionCoeff Result;
    // Рассчитываем коэффициенты
    qreal s1 ,s2, s3, s4;

    s1 = s2 = s3 = s4 = 0;
    int N = x.count();
    if (N > y.count()) return Result;
    for (int i = 0; i<N; ++i) {
        s1 = s1 + x[i];
        s2 = s2 + y[i];
        s3 = s3 + qPow(x[i], 2);
        s4 = s4 + x[i]*y[i];
    }
    Result.slope = (N*s4 - s1*s2)/(N*s3 - qPow(s1,2));
    Result.intercept = (s2*s3 - s1*s4)/(N*s3 - qPow(s1,2));
    return Result;
}

lineRegressionCoeff lineRegressionViaPoint(QVector<double> x, QVector<double> y, QPointF point)
{
    lineRegressionCoeff Result;
    qreal s1, s2;
    s1 = s2 = 0;

    int N = x.count();
    if (N > y.count()) return Result;
    for (int i = 0; i<N; ++i) {
        s1 = s1 + (y[i] - point.y())*(x[i] - point.x());
        s2 = s2 + qPow(x[i] - point.x(), 2);
    }
    Result.slope = s1/s2;
    Result.intercept = point.y() - point.x() * Result.slope;
    return Result;
}

int findBadestPoint(QVector<double> x, QVector<double> y)
{
    lineRegressionCoeff coeff = lineRegression(x, y);
    int   N = x.count();
    if (N > y.count()) return -1;

    qreal error = 0;
    int index = 0;
    qreal temp;
    for (int i = 0; i<N; ++i){
        temp = qAbs(y[i]-coeff.slope*x[i]-coeff.intercept);
        if (temp>error){
            error = temp;
            index = i;
        }
    }
    return index;
}

