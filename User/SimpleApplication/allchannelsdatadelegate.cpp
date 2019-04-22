#include "allchannelsdatadelegate.h"
#include <QDebug>
#include "editwidget.h"

AllChannelsDataDelegate::AllChannelsDataDelegate(QObject *parent):
    QItemDelegate (parent)
{
}


QWidget *AllChannelsDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    qDebug()<<"create Editor";
    auto *widget = new EditWidget(parent);
    widget->setCurrent(model->devices[index.row()].current*1e6);
    qDebug()<<model->devices[index.row()].current;
    widget->setChecked(model->devices[index.row()].isShorted);
    widget->setInterface(interface);
    widget->setIndex(model->devices[index.row()].devAddress);
    return widget;
}

void AllChannelsDataDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    // данные обновляться не будут
    Q_UNUSED(editor)
    Q_UNUSED(index)
}

void AllChannelsDataDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    // напрямую отправлять данные не будем
    qDebug()<<"setModelData";
    Q_UNUSED(editor)
    Q_UNUSED(model)
    Q_UNUSED(index)
}

void AllChannelsDataDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qDebug()<<"updateEditorGeometry";
    QItemDelegate::updateEditorGeometry(editor, option, index);
}

void AllChannelsDataDelegate::setModel(AllChannelsDataModel *value)
{
    model = value;
}

void AllChannelsDataDelegate::setInterface(cuIOInterface *value)
{
    qDebug()<<"setInterface()"<<value;
    interface = value;
}
