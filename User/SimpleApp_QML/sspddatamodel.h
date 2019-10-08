#ifndef SSPDDATAMODEL_H
#define SSPDDATAMODEL_H

#include <QAbstractListModel>

class SspdData;

class SspdDataModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(SspdData *data READ data WRITE setData)

public:
    explicit SspdDataModel(QObject *parent = nullptr);

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

    SspdData *data() const;
    void setData(SspdData *data);

signals:
    void newDataSetted(int index, double data);

private:
    SspdData *mData;
};

#endif // SSPDDATAMODEL_H
