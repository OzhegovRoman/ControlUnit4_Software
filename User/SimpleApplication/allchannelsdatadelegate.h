#ifndef ALLCHANNELSDATADELEGATE_H
#define ALLCHANNELSDATADELEGATE_H

#include <QObject>
#include <QItemDelegate>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include "allchannelsdatamodel.h"
#include "editwidget.h"
#include "Interfaces/cuiointerface.h"

class AllChannelsDataDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    AllChannelsDataDelegate(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModel(AllChannelsDataModel *value);

    void setInterface(cuIOInterface *value);

private:
    AllChannelsDataModel *model{};
    void finishEditing(QWidget *editor, QAbstractItemDelegate::EndEditHint hint = NoHint);
    cuIOInterface *interface{};

};

#endif // ALLCHANNELSDATADELEGATE_H
