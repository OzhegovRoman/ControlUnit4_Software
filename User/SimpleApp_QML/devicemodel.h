#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractListModel>
#include <QVector>

class DeviceList;

class DeviceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(DeviceList *list READ list WRITE setList )

public:
    explicit DeviceModel(QObject *parent = nullptr);

    enum {
        TypeRole = Qt::UserRole,
        AddressRole
    };
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    DeviceList *list() const;
    void setList(DeviceList *list);

private:
    DeviceList *mList;

};

#endif // DEVICEMODEL_H
