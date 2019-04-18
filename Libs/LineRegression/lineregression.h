#ifndef LINEREGRESSION_H
#define LINEREGRESSION_H

#include <QtCore>
#include <QList>

typedef struct {
    float slope;
    float intercept;
} lineRegressionCoeff;

lineRegressionCoeff lineRegression(QVector<double> x, QVector<double> y);
lineRegressionCoeff lineRegressionViaPoint(QVector<double> x, QVector<double> y, QPointF point);
int findBadestPoint(QVector<double> x, QVector<double> y);


#endif // LINEREGRESSION_H

