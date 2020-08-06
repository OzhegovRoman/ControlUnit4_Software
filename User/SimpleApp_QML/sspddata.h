#ifndef SSPDDATALIST_H
#define SSPDDATALIST_H

#include <QObject>
#include <QVector>

struct DataItem
{
    QString name;
    QString title;
    double value;
    QString type;
    int fixed;
    QString group;
    double step;
};

class UnitData : public QObject
{
    Q_OBJECT
public:
    explicit UnitData(QObject *parent = nullptr);
    QVector<DataItem> items() const;

    int getIndexByName(QString name);

signals:
    void dataChanged(int index);
    void newDataSetted(int index, double value);

public slots:
    void setData(int index, double value);
    void setData(int index, bool value);

protected:
    virtual void initialize();

private:
    QVector<DataItem> mItems;

};

#endif // SSPDDATALIST_H
