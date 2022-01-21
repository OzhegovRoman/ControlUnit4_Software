#include "allchannelsdatadelegate.h"
#include <QDebug>
#include "editwidget.h"
#include "Drivers/sspddriverm0.h"
#include "Drivers/sspddriverm1.h"
#include "Drivers/siscontrollinedriverm0.h"

AllChannelsDataDelegate::AllChannelsDataDelegate(QObject *parent):
    QItemDelegate (parent)
{
}


QWidget *AllChannelsDataDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    qDebug()<<"create Editor";
    EditWidget * widget = new EditWidget(parent);
    widget->setDriver(model->drivers[index.row()]);
    {
        auto* driver = qobject_cast<SspdDriverM0*>(model->drivers[index.row()]);
        if (driver){
            widget->setCurrent(driver->current()->currentValue()*1e6);
            widget->setChecked(driver->status()->currentValue().stShorted);
        }
    }
    {
        auto* driver = qobject_cast<SspdDriverM1*>(model->drivers[index.row()]);
        if (driver){
            widget->setCurrent(driver->current()->currentValue()*1e6);
            widget->setChecked(driver->status()->currentValue().stShorted);
        }
    }
    {
        auto* driver = qobject_cast<SisControlLineDriverM0*>(model->drivers[index.row()]);
        if (driver){
            widget->setCurrent(driver->current()->currentValue()*1e3);
            widget->setChecked(driver->shortEnable()->currentValue());
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
