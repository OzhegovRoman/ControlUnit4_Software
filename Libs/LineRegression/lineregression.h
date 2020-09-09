#ifndef LINEREGRESSION_H
#define LINEREGRESSION_H

#include <QtCore>
#include <QList>
#include <QPointF>

typedef struct {
    float slope;
    float intercept;
} lineRegressionCoeff;

lineRegressionCoeff lineRegression(QVector<double> x, QVector<double> y);
lineRegressionCoeff lineRegression(QVector<QPointF> vector);

lineRegressionCoeff lineRegressionViaPoint(QVector<double> x, QVector<double> y, QPointF point);
lineRegressionCoeff lineRegressionViaPoint(QVector<QPointF> vector, QPointF point);

int findBadestPoint(QVector<double> x, QVector<double> y);
int findBadestPoint(QVector<QPointF> vector);


#endif // LINEREGRESSION_H

