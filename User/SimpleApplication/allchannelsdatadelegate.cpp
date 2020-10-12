#include "allchannelsdatadelegate.h"
#include <QDebug>
#include "editwidget.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"

AllChannelsDataDelegate::AllChannelsDataDelegate(QObject *parent):
    QItemDelegate (parent)
{
}


QWidget *AllChannelsDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    qDebug()<<"create Editor";
    auto *widget = new EditWidget(parent);
    {
        auto* driver = qobject_cast<SspdDriverM0*>(model->drivers[index.row()]);
        if (driver){
            widget->setCurrent(driver->current()->currentValue()*1e6);
            widget->setChecked(driver->status()->currentValue().stShorted);
            widget->setInterface(interface);
            widget->setIndex(driver->devAddress());
        }
    }
    {
        auto* driver = qobject_cast<SspdDriverM1*>(model->drivers[index.row()]);
        if (driver){
            widget->setCurrent(driver->current()->currentValue()*1e6);
            widget->setChecked(driver->status()->currentValue().stShorted);
            widget->setInterface(interface);
            widget->setIndex(driver->devAddress());
        }
    }
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
    Q_UNUSED(editor)
    Q_UNUSED(model)
    Q_UNUSED(index)
}

void AllChannelsDataDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
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
