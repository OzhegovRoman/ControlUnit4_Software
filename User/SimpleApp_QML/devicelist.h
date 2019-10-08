#ifndef DEVICELIST_H
#define DEVICELIST_H

#include <QObject>
#include <QVector>

struct DeviceItem
{
    QString m_type;
    int m_address;
};

class DeviceList : public QObject
{
    Q_OBJECT
public:
    explicit DeviceList(QObject *parent = nullptr);

    QVector <DeviceItem> items() const;
    bool setItemAt(int index, const DeviceItem &item);

signals:
    void preItemAppended();
    void postItemAppended();

    void preItemRemoved(int index);
    void postItemRemoved();

public slots:
    void appendItem();
    void appendItem(const DeviceItem &item);
    void removeAll();
private:
    QVector<DeviceItem> mItems;
};

#endif // DEVICELIST_H
