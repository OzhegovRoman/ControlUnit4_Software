#include "compactdraw.h"
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QSettings>

ThemeType CompactDraw::themeType = TT_Light;
int16_t CD::panelShade = 0;
int16_t CD::textShade = 0;

Gpu_Hal_Context_t* CD::mHost = nullptr;
bool CD::isWideList = true;

void CompactDraw::setIsWideList(bool value)
   {
   CD::isWideList = value;
   }

CompactDraw::CompactDraw()
   {
   }

void CompactDraw::listButtonText(ColoredStatus cs,  int16_t top, QString col1, QString col2, QString col3)
   {
   int8_t sh;

   uint16_t vLine[2] = { static_cast<uint16_t>(top + 6),
                         static_cast<uint16_t>(top + 26)};
   uint16_t col[7] = {16,174,174,270,280,408,444};

   for (int i = 0; i < 2; ++i) {
      if (i == 0)
         App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextShade)));
      else{
         App_WrCoCmd_Buffer(mHost, COLOR(CD::statusColor(cs,true)));
         }
      sh = textShade*i;
      Gpu_CoCmd_Text(mHost, 16-sh, vLine[1]-10-sh, 28, OPT_CENTERY, col1.toLocal8Bit());
      if (col3.isEmpty()){
         Gpu_CoCmd_Text(mHost, isWideList ? 318-sh : 308-sh, top + 16 -sh, 28, OPT_CENTER, col2.toLocal8Bit());
         }
      else {
         Gpu_CoCmd_Text(mHost, isWideList ? col[4]-10-sh : col[3]-10-sh, vLine[1]-10-sh, 28, OPT_CENTERY | OPT_RIGHTX, col2.toLocal8Bit());
         Gpu_CoCmd_Text(mHost, isWideList ? col[6]-10-sh : col[5]-10-sh, vLine[1]-10-sh, 28, OPT_CENTERY | OPT_RIGHTX, col3.toLocal8Bit());
         }
      App_WrCoCmd_Buffer(mHost, LINE_WIDTH(16));
      App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
      for (int j = 0; j < 3; ++j) {
         for (int k = 0; k < 2; ++k) {
            if (!(col3.isEmpty() && j == 1))
               App_WrCoCmd_Buffer(mHost, VERTEX2II(isWideList ? col[j*2+2]-sh : col[j*2+1]-sh, vLine[k]-sh, 0, 0));
            }
         }
      App_WrCoCmd_Buffer(mHost, END());
      }
   }

void CompactDraw::animatedButtonText(ColoredStatus cs, int16_t top, int16_t center, uint32_t animVal, uint32_t animPeriod, QString text)
   {
   uint16_t textLen = text.length()*11;
   int8_t sh;

   App_WrCoCmd_Buffer(mHost, SCISSOR_XY(180, top));
   App_WrCoCmd_Buffer(mHost, SCISSOR_SIZE(2 * (center-180), 32));
   for (int i = 0; i < 2; ++i) {
      if (i == 0)
         App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextShade)));
      else{
         App_WrCoCmd_Buffer(mHost, COLOR(CD::statusColor(cs,true)));
         }
      sh = textShade*i;
      Gpu_CoCmd_Text(mHost, 180 - animVal * textLen/animPeriod - sh,
                     top + 16 - sh, 28, OPT_CENTERY,
                     text.toLocal8Bit());
      Gpu_CoCmd_Text(mHost, 180 + textLen - animVal * textLen/animPeriod - sh,
                     top + 16 - sh, 28, OPT_CENTERY,
                     text.toLocal8Bit());
      }
   App_WrCoCmd_Buffer(mHost, SCISSOR_XY(0, 0));
   App_WrCoCmd_Buffer(mHost, SCISSOR_SIZE(512, 512));
   }

void CompactDraw::headPanel(QString title, QString subtitle)
   {

   App_WrCoCmd_Buffer(mHost, BEGIN(RECTS));
   //   App_WrCoCmd_Buffer(mHost, COLOR(0xC8CCCCC));
   //   App_WrCoCmd_Buffer(mHost, COLOR(0xff0000));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(0,0,0,0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(480,66,0,0));
   App_WrCoCmd_Buffer(mHost, COLOR(0));

   App_WrCoCmd_Buffer(mHost, COLOR(themeColor(TextNormal)));
   Gpu_CoCmd_Gradient(mHost,100,0,CD::themeColor(Grad_Top),
                      100,66,CD::themeColor(Grad_Bottom));
   if (subtitle.isEmpty()){
      Gpu_CoCmd_Text(mHost, 240, 32, 31, OPT_CENTER, title.toLocal8Bit());
      }
   else {
      Gpu_CoCmd_Text(mHost, 240, 24, 31, OPT_CENTER, title.toLocal8Bit());
      Gpu_CoCmd_Text(mHost, 240, 54, 27, OPT_CENTER, subtitle.toLocal8Bit());
      }
   App_WrCoCmd_Buffer(mHost,  END());
   }

void CompactDraw::mainBackground()
   {
   App_WrCoCmd_Buffer(mHost, SCISSOR_SIZE(480,206));
   App_WrCoCmd_Buffer(mHost, SCISSOR_XY(0,66));

   App_WrCoCmd_Buffer(mHost, BEGIN(RECTS));

   App_WrCoCmd_Buffer(mHost, VERTEX2II(0,66,0,0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(480,271,0,0));
   Gpu_CoCmd_Gradient(mHost,100,66,CD::themeColor(Grad_Top),
                      100,271,CD::themeColor(Grad_Bottom));
   App_WrCoCmd_Buffer(mHost, SCISSOR_SIZE(481,271));
   App_WrCoCmd_Buffer(mHost, SCISSOR_XY(0,0));
   App_WrCoCmd_Buffer(mHost, END());

   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(16));
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(other)));  //info: horisontal Line (upper part)
   App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(0, 68, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(480, 68, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(Grad_Top))); //info: horisontal line (lower part)
   App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(0, 66, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(480, 66, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   }

void CompactDraw::mainArea(uint16_t right, uint16_t bot)
   {
   qDebug() << bot;
   if (bot == 0) bot = 260;
   if (right == 0) right = 470;
   mainArea(82,bot,10,right);
   }

void CompactDraw::mainArea(uint16_t top, uint16_t bot, uint16_t left, uint16_t right)
   {
   qDebug() << top << bot << left << right;
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(40));
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(other)));  //info: button-area panelShade
   App_WrCoCmd_Buffer(mHost, BEGIN(RECTS));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left+panelShade, top+panelShade, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(right+panelShade, bot+panelShade, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());

   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(InnerArea))); //Info: button-area infill
   App_WrCoCmd_Buffer(mHost, BEGIN(RECTS));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left, top, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(right, bot, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   }

void CompactDraw::simpleListHeader(QStringList rows, uint16_t right)
   {
   uint8_t increment = 36;
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(16));
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextInactive)));
   App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
   for (int i = 0; i < rows.count()-1; ++i) {
      App_WrCoCmd_Buffer(mHost, VERTEX2II(18,  116+(i*increment), 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(460, 116+(i*increment), 0, 0));
      }
   App_WrCoCmd_Buffer(mHost, END());

   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextNormal)));

   for (int i = 0; i < rows.count(); ++i) {
      Gpu_CoCmd_Text(mHost, 18, 98+(i*increment), 29, OPT_CENTERY, rows.at(i).toLocal8Bit());
      }
   }

void CompactDraw::simpleListValues(QStringList rows, uint16_t right)
   {

   }

void CompactDraw::toggleButton(uint16_t x, uint16_t y, uint16_t w, uint16_t h, bool isEnabled, bool isFlat, const char *caption)
   {
   App_WrCoCmd_Buffer(mHost, COLOR(themeColor(Colors::TextNormal)));
   Gpu_CoCmd_FgColor(mHost, (isEnabled) ? CD::themeColor(Colors::SliderPoint) : CD::themeColor(Grad_Bottom));
   Gpu_CoCmd_Button(mHost, x,y,w,h, 28, isFlat ? OPT_FLAT : 0, caption);
   }

void CompactDraw::sliderButton(uint16_t left, uint16_t top, const char* labels, uint16_t buttonTag, bool isEnabled, uint8_t width, bool isNormalyEnabled)
   {
   Gpu_CoCmd_FgColor(mHost, COLOR(CD::themeColor(Colors::SliderPoint)));
   Gpu_CoCmd_BgColor(mHost, (!(isEnabled^isNormalyEnabled)) ? themeColor(Colors::SliderBG) : themeColor(Colors::InnerArea));
   App_WrCoCmd_Buffer(mHost, TAG(buttonTag));
   Gpu_CoCmd_Toggle(mHost, left, top, width, 27, 0, (!(isEnabled^isNormalyEnabled)) ? 65535 : 0, labels);
   }

void CompactDraw::updateIndicator(uint16_t left, uint16_t top, ColoredStatus cs)
   {
   App_WrCoCmd_Buffer(mHost, COLOR(CD::statusColor(cs,false)));
   App_WrCoCmd_Buffer(mHost, BEGIN(POINTS));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left, top, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   }

void CompactDraw::buttonBack(uint16_t leftX, uint16_t centY)
   {
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(32));
   App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextNormal)));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+4,  centY, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+32, centY, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX,    centY, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+14, centY-14, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX,    centY, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+14, centY+14, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   }

void CompactDraw::buttonMenu(uint16_t leftX, uint16_t centY)
   {
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(32));
   App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextNormal)));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX,    centY, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+32, centY, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX,    centY-12, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+32, centY-12, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX,    centY+12, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(leftX+32, centY+12, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   }

void CompactDraw::buttonInfo()
   {
   uint16_t side = 18;
   uint16_t top = 33-side/2;
   uint16_t left = 470-top-side/2;

   fastButton(top,left,side);

   Gpu_CoCmd_Text(mHost, left+side/2, top+side/2, 30, OPT_CENTER, "i");

   }

void CompactDraw::buttonChangeTheme()
   {
   uint16_t side = 18;
   uint16_t top = 33-side/2;
   uint16_t left = 470-top-side/2;

   fastButton(top,left,side);

   switch (themeType) {
      case TT_Dark:  {
         drawSun(top, left, side);
         } break;
      case TT_Light: {
         drawMoon(top, left, side);
         } break;
      }
   }

void CompactDraw::setThemeType(const ThemeType &value)
   {
   themeType = value;
   panelShade = (themeType == TT_Light)?-2:2;
   textShade = (themeType == TT_Light)?-1:1;
   }

void CompactDraw::cycleTheme()
   {
   ThemeType tt = (ThemeType)((CD::themeType+1)%_TT_Count);
   CD::setThemeType((ThemeType)((CD::themeType+1)%_TT_Count));
   }

void CompactDraw::saveTheme()
   {
   QSettings settings("Scontel", "cu-embededDisplay");
   settings.setValue("Theme", themeType);
   }

void CompactDraw::loadTheme()
   {
   QSettings settings("Scontel", "cu-embededDisplay");
   setThemeType(static_cast<ThemeType>(settings.value("Theme", 0).toUInt()));
   }

void CompactDraw::fastButton(uint16_t top, uint16_t left, uint16_t side)
   {
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextNormal)));
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(200));
   App_WrCoCmd_Buffer(mHost, BEGIN(RECTS));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left-1, top-1, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left+side+1, top+side+1, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());

   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(Grad_Center)));
   App_WrCoCmd_Buffer(mHost, BEGIN(RECTS));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left, top, 0, 0));
   App_WrCoCmd_Buffer(mHost, VERTEX2II(left+side, top+side, 0, 0));
   App_WrCoCmd_Buffer(mHost, END());
   App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextNormal)));
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(16));
   }

void CompactDraw::drawSun(uint16_t top, uint16_t left, uint16_t side)
   {
   uint16_t centY = top+(side/2) + textShade;
   uint16_t centX = left+(side/2) + textShade;
   App_WrCoCmd_Buffer(mHost, LINE_WIDTH(16));
   for (int i = 0; i < 2; ++i) {
      App_WrCoCmd_Buffer(mHost, BEGIN(LINES));
      if (i == 1){
         centX-=textShade;
         centY-=textShade;
         App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextNormal)));
         }
      else
         App_WrCoCmd_Buffer(mHost, COLOR(CD::themeColor(TextShade)));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX,    centY+14, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX,    centY-14, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX+14, centY, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX-14, centY, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX-9,  centY-9, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX+9,  centY+9, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX-9,  centY+9, 0, 0));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX+9,  centY-9, 0, 0));
      App_WrCoCmd_Buffer(mHost, END());

      App_WrCoCmd_Buffer(mHost, BEGIN(POINTS));
      App_WrCoCmd_Buffer(mHost, POINT_SIZE(100));
      App_WrCoCmd_Buffer(mHost, VERTEX2II(centX, centY, 0, 0));
      App_WrCoCmd_Buffer(mHost, END());
      }

   }

void CompactDraw::drawMoon(uint16_t top, uint16_t left, uint16_t side)
   {
   uint16_t centY = top+(side/2) + textShade;
   uint16_t centX = left+(side/2) + textShade;

   App_WrCoCmd_Buffer(mHost,    BEGIN(POINTS));
   App_WrCoCmd_Buffer(mHost,    COLOR(themeColor(TextShade)));
   App_WrCoCmd_Buffer(mHost,    POINT_SIZE(241));
   App_WrCoCmd_Buffer(mHost,    VERTEX2II(centX, centY, 0, 0));
   App_WrCoCmd_Buffer(mHost,    COLOR(themeColor(TextNormal)));
   App_WrCoCmd_Buffer(mHost,    VERTEX2II(centX-1, centY, 0, 0));

   App_WrCoCmd_Buffer(mHost,    COLOR(themeColor(TextShade)));
   App_WrCoCmd_Buffer(mHost,    POINT_SIZE(222));
   App_WrCoCmd_Buffer(mHost,    VERTEX2II(centX-5, centY, 0, 0));
   App_WrCoCmd_Buffer(mHost,    COLOR(themeColor(Grad_Center)));
   App_WrCoCmd_Buffer(mHost,    POINT_SIZE(223));
   App_WrCoCmd_Buffer(mHost,    VERTEX2II(centX-6, centY, 0, 0));
   App_WrCoCmd_Buffer(mHost,    END());
   }

uint32_t CompactDraw::toBaseColor(uint32_t srcColor)
   {

   }

uint32_t CompactDraw::themeColor(Colors color)
   {
   uint32_t rv = 0;
   switch (CompactDraw::themeType) {
      case TT_Light: {
         switch(color){
            case Main:         rv = 0xFFFFFF;  break;
            case TextShade:    rv = 0x777777;  break;
            case TextInactive: rv = 0xAAAAAA;  break;
            case TextNormal:   rv = 0x000000;  break;
            case TextFail:     rv = 0xff672E;  break;
            case LedWait:      rv = 0xAAAAAA;  break;
            case LedOk:        rv = 0x00CC00;  break;
            case LedFail:      rv = 0xff672E;  break;
            case SliderPoint:  rv = 0xff622E;  break;
            case SliderBG:     rv = 0xff9b7d;  break;
            case Grad_LogoTop: rv = 0xC4c8c8; break;
            case Grad_Buttons:
            case Grad_Top:     rv = 0xC4C8C8;  break;
            case Grad_Center:  rv = 0xd7d7d7;  break;
            case Grad_LogoBot:
            case Grad_Bottom:  rv = 0xE2E6E6;  break;
            case other:        rv = 0x646666;  break;
            case InnerArea:    rv = 0xECF0F0;  break;
            default: break;
            }
         } break;
      case TT_Dark: {
         switch(color){
            case Main:         rv = 0x000000;  break;
            case TextShade:    rv = 0x828282;  break;
            case TextInactive: rv = 0x878787;  break;
            case TextNormal:   rv = 0xCCCCCC;  break;
            case TextFail:     rv = 0xDD610E;  break;
            case LedWait:      rv = 0x1F1F1F;  break;
            case LedOk:        rv = 0x1dcc37;  break;
            case LedFail:      rv = 0xFF622E;  break;
            case SliderPoint:  rv = 0xC8510b;  break;
            case SliderBG:     rv = 0x783508;  break;
            case Grad_Buttons: rv = 0xffffff;  break;
            case Grad_LogoTop: rv = 0x555555;  break;
            case Grad_Top:     rv = 0x3E3E3E;  break;
            case Grad_Center:  rv = 0x1F1F1F;  break;
            case Grad_LogoBot: rv = 0x828282;  break;
            case Grad_Bottom:  rv = 0x000000;  break;
            case other:        rv = 0x828282;  break;
            case InnerArea:    rv = 0x000000;  break;
            default: break;
            }
         }
      default: break;
      }
   return rv;
   }

uint32_t CompactDraw::statusColor(ColoredStatus cs, bool isText)
   {
   uint32_t rv = 0xff0000;
   if (isText){
      switch(cs){
         case CS_Normal:     rv = themeColor(TextNormal);     break;
         case CS_Inactive:   rv = themeColor(TextInactive);   break;
         case CS_Fail:       rv = themeColor(TextFail);       break;
         }
      }
   else {
      switch(cs){
         case CS_Normal:     rv = themeColor(LedOk);     break;
         case CS_Inactive:   rv = themeColor(Grad_Center);   break;
         case CS_Fail:       rv = themeColor(LedFail);       break;
         }
      }
   return rv;
   }
