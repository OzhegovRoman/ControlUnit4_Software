#include "guiitems.h"

GuiItems::GuiItems():QObject()
   {
   }

void GuiItems::declareQML()
   {
   qmlRegisterType<GuiItems>("GuiItems",1,0,"GuiItems");
   }
