#ifndef GUIITEMS_H
#define GUIITEMS_H

#include <QObject>
#include <QtGlobal>
#include <QQmlEngine>



struct DataItem
{
    QString name;
    QString title;
    double value;
    QString type;
    int fixed;
    QString group;
    double step;
};

class GuiItems : public QObject
   {
   Q_OBJECT
public:
   GuiItems();

   enum WidgetType{
      WT_Switch,
      WT_Button,
      WT_DelayButton,
      WT_ReadOnly,
      WT_AdjustBox,
      };
   Q_ENUMS(WidgetType)


   static void declareQML();
   };

#endif // GUIITEMS_H
