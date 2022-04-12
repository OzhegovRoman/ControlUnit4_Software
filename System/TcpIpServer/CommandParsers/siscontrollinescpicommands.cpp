#include "siscontrollinescpicommands.h"

SisControlLineScpiCommands::SisControlLineScpiCommands(QObject *parent)
    : CommonScpiCommands (parent)
{

}

//CTRL:DEVX:
/** формат команд для  Control Line
 * ConTRolLine:DEVice<N>:HEAT                            - включение нагрева
 * ConTRolLine:DEVice<N>:STOP                            - остановка
 * ConTRolLine:DEVice<N>:DATA?                           - получение всех данных
 * ConTRolLine:DEVice<N>:CURRent?                        - получение тока печки [A]
 * ConTRolLine:DEVice<N>:CURRent <Value|Float>           - установка тока печки [A]
 * ConTRolLine:DEVice<N>:RELayStatus?                    - получение статуса реле  (RELAY_OPEN=0, RELAY_CLOSE=1)
 * ConTRolLine:DEVice<N>:RELayStatus <Value|Byte>        - установка статуса реле
 * ConTRolLine:DEVice<N>:MODE?                           - получение режима работы (MANUAL=0, AUTO=1)
 * ConTRolLine:DEVice<N>:MODE <Value|Byte>               - установка режима работы
 * ConTRolLine:DEVice<N>:EEPRomconst?                    - получение коэффициентов из постоянной памяти
 * ConTRolLine:DEVice<N>:EEPRomconst <Value|JSON>        - установка коэффициентов из постоянной памяти
 * ConTRolLine:DEVice<N>:CurrentADCcoeff?                - получение коэффициентов
 * ConTRolLine:DEVice<N>:CurrentADCcoeff <Value|JSON>    - установка коэффициентов
 * ConTRolLine:DEVice<N>:CurrentDACcoeff?                - получение коэффициентов
 * ConTRolLine:DEVice<N>:CurrentDACcoeff <Value|JSON>    - установка коэффициентов
 * ConTRolLine:DEVice<N>:MAXimumCurrent?                 - получение максимального тока печки
 * ConTRolLine:DEVice<N>:MAXimumCurrent <Value|float>    - установка максимального тока печки
 * ConTRolLine:DEVice<N>:FrontedgeTIMe?                  - получение времени нарастания тока печки
 * ConTRolLine:DEVice<N>:FrontedgeTIMe <Value|float>     - установка времени нарастания тока печки
 * ConTRolLine:DEVice<N>:HoldTIMe?                       - получение времени удержания тока печки
 * ConTRolLine:DEVice<N>:HoldTIMe <Value|float>          - установка времени удержания тока печки
 * ConTRolLine:DEVice<N>:RearedgeTIMe?                   - получение времени спада тока печки
 * ConTRolLine:DEVice<N>:RearedgeTIMe <Value|float>      - установка времени спада тока печки
 **/
bool SisControlLineScpiCommands::executeCommand(QString command, QString params)
{

}
