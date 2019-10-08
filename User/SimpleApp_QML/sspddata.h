#ifndef SSPDDATALIST_H
#define SSPDDATALIST_H

#include <QObject>
#include <QVector>

struct SspdDataItem
{
    QString name;
    QString title;
    double value;
    QString type;
    int fixed;
    QString group;
    double step;
};

class SspdData : public QObject
{
    Q_OBJECT
public:
    explicit SspdData(QObject *parent = nullptr);
    QVector<SspdDataItem> items() const;


    int getIndexByName(QString name);

signals:
    void dataChanged(int index);
    void newDataSetted(int index, double value);

public slots:
    void setData(int index, double value);
    void setData(int index, bool value);

private:
    QVector<SspdDataItem> mItems;

};

#endif // SSPDDATALIST_H
