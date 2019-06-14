#ifndef ALLCHANNELSDATAMODEL_H
#define ALLCHANNELSDATAMODEL_H

#include <QObject>
#include <QAbstractListModel>

struct deviceInfo_t {
    quint8  devAddress;
    bool isSspd;
    bool isShorted;
    double temperature;
    double current;
    double voltage;
    unsigned int triggerCount;
};


class AllChannelsDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AllChannelsDataModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void initialize(const QString& info);

    QList<deviceInfo_t> devices;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void setCurrent(int idx, float data);

signals:
    void editCompleted (const QString &);
};

#endif // ALLCHANNELSDATAMODEL_H
