#ifndef COMPACTDRAW_H
#define COMPACTDRAW_H

#include "riverdieve.h"
//#include <stdint.h>

enum ThemeType{
   TT_Dark = 0,
   TT_Light,
   _TT_Count,
   };

enum ColoredStatus{
   CS_Normal,
   CS_Inactive,
   CS_Fail,
   };


enum Colors{
   Main = 0,
   TextShade,
   TextNormal,
   TextFail,
   TextInactive,
   LedWait,
   LedOk,
   LedFail,
   SliderPoint,
   SliderBG,
   Grad_Buttons,
   Grad_Top,
   Grad_Center,
   Grad_Bottom,
   other,
   InnerArea,
   };

struct RGB{
   union {
      uint32_t color : 24;
      uint8_t col[3];
      struct{
         uint8_t R;
         uint8_t G;
         uint8_t B;
         };

      };
   };

class CompactDraw
   {
   static bool isWideList;
   static ThemeType themeType;
   static int16_t panelShade;
   static int16_t textShade;

   static void fastButton(uint16_t top, uint16_t left, uint16_t side);
   static void drawSun (uint16_t top, uint16_t left, uint16_t side);
   static void drawMoon(uint16_t top, uint16_t left, uint16_t side);
   static uint32_t toBaseColor(uint32_t srcColor);

public:   
   static Gpu_Hal_Context_t *mHost;

   static uint32_t themeColor(Colors color);
   static uint32_t statusColor(ColoredStatus cs, bool isText);

   CompactDraw();

   static void listButtonText(ColoredStatus cs, int16_t top, QString col1, QString col2, QString col3 = QString());
   static void animatedButtonText(ColoredStatus cs, int16_t  top, int16_t center, uint32_t animVal, uint32_t animPeriod, QString text);
   static void headPanel(QString title, QString subtitle = QString());
   static void mainBackground();
   static void mainArea(uint16_t right = 470, uint16_t bot = 0);
   static void mainArea(uint16_t top, uint16_t bot, uint16_t left, uint16_t right);
   static void simpleListHeader(QStringList rows, uint16_t right = 470);
   static void simpleListValues(QStringList rows, uint16_t right = 470); //не. Выходит жирновато.
   static void toggleButton(uint16_t x, uint16_t y, uint16_t w, uint16_t, bool isEnabled, bool isFlat, const char* caption);
   static void sliderButton(uint16_t left, uint16_t top, const char* labels, uint16_t buttonTag, bool isEnabled, uint8_t width = 111, bool isNormalyEnabled = true);
   static void updateIndicator(uint16_t left,uint16_t top, ColoredStatus cs);
   static void buttonBack(uint16_t leftX = 26, uint16_t centY = 32);
   static void buttonInfo();
   static void buttonMenu(uint16_t leftX = 420, uint16_t centY = 32);
   static void buttonChangeTheme();

   static void setIsWideList(bool value);
   static void setThemeType(const ThemeType &value);
   static void cycleTheme();
   };

typedef CompactDraw CD;

#endif // COMPACTDRAW_H
