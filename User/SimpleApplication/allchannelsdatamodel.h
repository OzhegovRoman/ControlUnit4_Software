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
};


class AllChannelsDataModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AllChannelsDataModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void initialize(QString info);

    QList<deviceInfo_t> devices;

    // QAbstractItemModel interface
public:
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const ;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void setCurrent(int idx, float data);

signals:
    void editCompleted (const QString &);
};

#endif // ALLCHANNELSDATAMODEL_H
