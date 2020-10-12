#ifndef UNITDATAMODEL_H
#define UNITDATAMODEL_H

#include <QAbstractListModel>

class UnitData;

class UnitDataModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(UnitData *data READ data WRITE setData)

public:
    explicit UnitDataModel(QObject *parent = nullptr);

    enum {
        NameRole = Qt::UserRole,
        TitleRole,
        ValueRole,
        TypeRole,
        FixedRole,
        GroupRole,
        StepRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    QHash<int, QByteArray> roleNames() const override;

    UnitData *data() const;
    void setData(UnitData *data);

signals:
    void newDataSetted(int index, double data);

private:
    UnitData *mData;
};

#endif // UNITDATAMODEL_H
