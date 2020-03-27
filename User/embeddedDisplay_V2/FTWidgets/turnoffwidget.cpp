#include "turnoffwidget.h"

void TurnOffWidget::turnOff(FT801_SPI *ft801)
{
    if (ft801 == nullptr)
        return;
    ft801->DLStart();
    ft801->Cmd_Gradient(464,73, 0x707070, 464,283, 0x000000);

    ft801->ColorRGB(255,255,255);
    ft801->Cmd_Text(240, 34, 29, FT_OPT_CENTER, "Control unit will be turned off or rebooted.");
    ft801->Cmd_Text(240, 62, 29, FT_OPT_CENTER, "Please wait until operation is done.");
    ft801->Cmd_Spinner(240, 164, 0, 1);
    ft801->DLEnd();
    ft801->Finish();
}
