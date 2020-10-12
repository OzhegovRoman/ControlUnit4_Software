#ifndef UNITDATA_H
#define UNITDATA_H

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
    QVector<DataItem> *items();

    int getIndexByName(QString name);

    QString currentType() const;
    void setCurrentType(const QString &currentType);

    int currentAddress() const;
    void setCurrentAddress(int currentAddress);

signals:
    void dataChanged(int index);
    void newDataSetted(int index, double value);

public slots:
    void setData(int index, double value);
    void setData(int index, bool value);
    void setData(QString name, double value);
    void setData(QString name, bool value);

protected:
    virtual void initialize();

private:
    QVector<DataItem> *mItems;
    QString mCurrentType;
    int mCurrentAddress;
};

#endif // UNITDATA_H
