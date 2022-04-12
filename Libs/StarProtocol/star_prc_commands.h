/*
 * star_prc_commands.h
 *
 *  Created on: 13 мая 2015 г.
 *      Author: Roman
 */

#ifndef STAR_PRC_COMMANDS_H_
#define STAR_PRC_COMMANDS_H_

#include "star_prc_structs.h"

/*
 * Общие команды для всех устройств
 */
#define MaxBufferSize   	0x100

namespace cmd {
enum General {
    G_Init            =   0x00, // Инициализация устройства
    G_GoToPark                , // Запарковать
    G_ListeningOff            , // Оглушить
    G_GetStatus               , // Получить статус устройства
    G_GetLastError            , // Последняя из случившихся ошибок текстовым сообщением
    G_GetDeviceId             , // Уникальный идентификатор устройства
    G_GetDeviceType           , // Тип устройства
    G_GetModVersion           , // Модификация устройства
    G_GetHwVersion            , // Версия железа устройства
    G_GetFwVersion            , // Версия Firmware
    G_GetDeviceDescription    , // Полное описание устройства текстом
    G_Reboot                  , // Перезагрузка и уход в автозагрузку
    G_WriteEeprom               // запись в постоянную память
};


/*
 * Команды для CU4SDM0V1
 */
enum enum_CU4SDM0V1 {
    SD_GetData   =           0x10,  // данные устройства
    SD_GetCurrent            ,      // ток смещения
    SD_SetCurrent            ,
    SD_GetVoltage            ,      // напряжение смещения
    SD_GetCounts             ,      // отсчеты счетчика
    SD_GetStatus             ,      // статус
    SD_SetStatus             ,
    // побитовые операции со статусом
    SD_SetShortEnable        ,      // Закоротка
    SD_SetAmpEnable          ,      // Усилитель
    SD_SetRfKeyToComparatorEnable , // Rf ключ
    SD_SetComparatorLatchEnable,    // Компаратор
    SD_SetCounterEnable      ,      // счетчик импульсов
    SD_SetAutoResEnable      ,      // система автосброса

    SD_GetParams             ,      // параметры устройства
    SD_SetParams             ,
    SD_GetCmpRef             ,      // Уровень компарации
    SD_SetCmpRef             ,
    SD_GetTimeConst          ,      // Постоянная времени счетчика
    SD_SetTimeConst          ,
    SD_GetARThreshold        ,      // Уровень срабатывания системы автосброса
    SD_SetARThreshold        ,
    SD_GetARTimeOut          ,      // Постоянная времени системы автосброса
    SD_SetARTimeOut          ,
    SD_GetARCounts           ,      // Отдельно читаем сколько раз случилось срабатывание автосброса unsigned int
    SD_SetARCounts           ,

    SD_GetEepromConst        ,      // EEPROM_Coeff Коэффициенты для записи в память
    SD_SetEepromConst        ,
    SD_GetCurrentAdcCoeff    ,      // Current_ADC_slope
    SD_SetCurrentAdcCoeff    ,
    SD_GetVoltageAdcCoeff    ,      // Voltage_ADC_slope
    SD_SetVoltageAdcCoeff    ,
    SD_GetCurrentDacCoeff    ,      // Current_DAC_slope
    SD_SetCurrentDacCoeff    ,
    SD_GetComparatorCoeff    ,      // Cmp_Ref_DAC_slope
    SD_SetComparatorCoeff    ,

    // новые команды
    SD_GetPIDStatus          ,      // PID регулятор стабилизации тока смещения
    SD_SetPIDStatus          ,

    //    // PWM на ShortCircuitPin
    //    SD_GetPWMShortCircuitStatus     ,
    //    SD_SetPWMShortCircuitStatus     ,
    //    SD_GetPWMShortCircuitFrequency  ,
    //    SD_SetPWMShortCircuitFrequency  ,
    //    SD_GetPWMShortCircuitDuty       ,
    //    SD_SetPWMShortCircuitDuty       ,

    // расширение команд для CU4SDM1
    SD_GetCurrentMonitor            ,
    SD_GetCurrentMonitorAdcCoeff    ,
    SD_SetCurrentMonitorAdcCoeff    ,

    SD_SetHFModeEnable        ,      // HighFrequency mode

    SD_SetHFPulseWidth  ,       //HighFrequency PulseWidth. Нужен для правильного расчета ошибки тока
    SD_GetHFPulseWidth  ,       //HighFrequency PulseWidth. Нужен для правильного расчета
};
/*
 * Команды для CU4TDM0V1
 */
enum enum_CU4TDM0V1 {
    // данные устройства
    TD_GetData                      = 0x10, // Получение данных с устройства
    TD_GetTemperature               = 0x11, // температура
    TD_GetPressure                  = 0x12, // давление
    TD_GetTempSensorCurrent         = 0x13, // Ток смещения датчика температуры
    TD_SetTempSensorCurrent         = 0x14,
    TD_GetTempSensorVoltage         = 0x15, // Напряжение на датчике температуры
    TD_GetPressSensorVoltageP       = 0x16, // Напряжение на датчике давления
    TD_GetPressSensorVoltageN       = 0x17,
    TD_GetTurnOnStatus              = 0x18, // включить-выключить
    TD_SetTurnOnStatus              = 0x19,

    TD_GetEepromConst               = 0x1a,
    TD_SetEepromConst               = 0x1b,

    TD_GetTempSensorCurrentAdcCoeff = 0x1c,
    TD_SetTempSensorCurrentAdcCoeff = 0x1d,

    TD_GetTempSensorCurrentDacCoeff = 0x1e,
    TD_SetTempSensorCurrentDacCoeff = 0x1f,

    TD_GetTempSensorVoltageCoeff    = 0x20,
    TD_SetTempSensorVoltageCoeff    = 0x21,

    TD_GetPressSensorVoltagePCoeff  = 0x22,
    TD_SetPressSensorVoltagePCoeff  = 0x23,
    TD_GetPressSensorVoltageNCoeff  = 0x24,
    TD_SetPressSensorVoltageNCoeff  = 0x25,

    TD_GetPressSensorCoeff          = 0x26,
    TD_SetPressSensorCoeff          = 0x27,

    TD_GetTempTableValues           = 0x28,
    TD_SetTempTableValues           = 0x29
};

/*
 * Команды для CU4TDM1
 */
enum enum_CU4TDM1 {
    // данные устройства
    //TDM1_GetData                      = 0x10, // не реализована
    TDM1_GetTemperature               = 0x11, // реализовывается по другому, требует 1 байт c адресом канала (0-3) или 255 чтобы получить все данные сразу же
    TDM1_SetTempSensorCurrent         = 0x14, // реализовывается по другому, требует 1 байт c адресом канала (0-3) или 255 чтобы получить все данные сразу же
    TDM1_GetTempSensorVoltage         = 0x15, // реализовывается по другому, требует 1 байт c адресом канала (0-3) или 255 чтобы получить все данные сразу же

    TDM1_GetEepromConst               = 0x1a, //
    TDM1_SetEepromConst               = 0x1b, //

    TDM1_GetTempTableValues           = 0x28, // не реализована
    TDM1_SetTempTableValues           = 0x29, // не реализована

    TDM1_GetRelaysStatus              = 0x2A, // включить-выключить
    TDM1_SetRelaysStatus              = 0x2B,

    TDM1_GetSwitcherMode              = 0x2C, // изменение режима переключателя
    TDM1_SetSwitcherMode              = 0x2D,

    TDM1_GetDefaultParams             = 0x2E,  // получение параметров по умолчанию
    TDM1_SetDefaultParams             = 0x2F,

};

enum enum_CU4TDM1_SwitcherMode {
    smConnect = 0,
    smDisconnect = 1,
    smAC = 2,
    smNone = 3
};

/*
 * Команды для CU4HTM0
 */
enum enum_CU4HTM0 {
    // команды
    HT_StartHeating                 = G_WriteEeprom + 1,
    HT_StopHeating                  = G_WriteEeprom + 2,
    // данные устройства
    HT_GetData                      = 0x10, // Получение данных с устройства

    HT_GetCurrent                   = 0x11, // получение тока печки в [А]
    HT_SetCurrent                   = 0x12, // работает только в ручном режиме

    HT_GetRelayStatus               = 0x13, // получение статуса выходного реле 0 - отключено, 1 - подключено
    HT_SetRelayStatus               = 0x14,

    HT_GetMode                      = 0x15,
    HT_SetMode                      = 0x16,

    // eeprom константы
    HT_GetEepromConst               = 0x17,
    HT_SetEepromConst               = 0x18,

    HT_GetMaxCurrent                = 0x19, // максимальный ток печки [A]
    HT_SetMaxCurrent                = 0x1a,

    HT_GetFrontEdgeTime             = 0x1b, // получить время линейного нарастания тока печки [сек]
    HT_SetFrontEdgeTime             = 0x1c,
    HT_GetHoldTime                  = 0x1d, // получить время удержания максимального тока печки [сек]
    HT_SetHoldTime                  = 0x1e,
    HT_GetRearEdgeTime              = 0x1f, // получить время линейного спада тока печки [сек]
    HT_SetRearEdgeTime              = 0x20,

    HT_GetCurrentAdcCoeff           = 0x21,
    HT_SetCurrentAdcCoeff           = 0x22,

    HT_GetCurrentDacCoeff           = 0x23,
    HT_SetCurrentDacCoeff           = 0x24,
};

/*
 * Команды для CU4CLM0
 */
enum enum_CU4CLM0 {
    // данные устройства
    CL_GetData                      = 0x10, // Получение данных с устройства

    CL_GetCurrent                   = 0x11, // получение тока смещения [А]
    CL_SetCurrent                   = 0x12,

    CL_GetShortEnable               = 0x13, // Закоротка
    CL_SetShortEnable               = 0x14, // Закоротка

    CL_GetPIDStatus                 = 0x15, // PID регулятор стабилизации тока смещения
    CL_SetPIDStatus                 = 0x16,

    // eeprom константы
    CL_GetEepromConst               = 0x17,
    CL_SetEepromConst               = 0x18,

    CL_GetCurrentAdcCoeff           = 0x19,
    CL_SetCurrentAdcCoeff           = 0x1a,

    CL_GetCurrentDacCoeff           = 0x1b,
    CL_SetCurrentDacCoeff           = 0x1c,

    CL_GetCurrentLimits             = 0x1d,
    CL_SetCurrentLimits             = 0x1e,

    CL_GetCurrentStep               = 0x1f,
    CL_SetCurrentStep               = 0x20,

};

/*
 * Команды для CU4BSM0
 */
enum enum_CU4BSM0 {
    // данные устройства
    BS_GetData                      = 0x10, // Получение данных с устройства

    BS_GetCurrent                   = 0x11, // получение тока смещения [А]
    BS_SetCurrent                   = 0x12,

    BS_GetVoltage                   = 0x13, // получение напряжения смещения [В]
    BS_SetVoltage                   = 0x14,

    BS_GetShortEnable               = 0x15, // Закоротка
    BS_SetShortEnable               = 0x16, // Закоротка

    BS_GetMode                      = 0x17, // режим смещения
    BS_SetMode                      = 0x18, //

    BS_GetPIDStatus                 = 0x19, // PID регулятор стабилизации тока смещения
    BS_SetPIDStatus                 = 0x1A,

    // eeprom константы
    BS_GetEepromConst               = 0x1B, // EEPROM_Coeff Коэффициенты для записи в память
    BS_SetEepromConst               = 0x1C,

    BS_GetCurrentAdcCoeff           = 0x1D, // Current_ADC
    BS_SetCurrentAdcCoeff           = 0x1E,

    BS_GetVoltageAdcCoeff           = 0x1F, // Voltage_ADC
    BS_SetVoltageAdcCoeff           = 0x20,

    BS_GetCurrentDacCoeff           = 0x21, // Current_DAC
    BS_SetCurrentDacCoeff           = 0x22,

    BS_GetVoltageDacCoeff           = 0x23, // Voltage_DAC
    BS_SetVoltageDacCoeff           = 0x24,

    BS_GetCurrentLimits             = 0x25,
    BS_SetCurrentLimits             = 0x26,

    BS_GetCurrentStep               = 0x27,
    BS_SetCurrentStep               = 0x28,

    BS_GetVoltageLimits             = 0x29,
    BS_SetVoltageLimits             = 0x2A,

    BS_GetVoltageStep               = 0x2B,
    BS_SetVoltageStep               = 0x2C,

    BS_GetCurrentMonitorResistance  = 0x2D,
    BS_SetCurrentMonitorResistance  = 0x2E,

    // sweep params
    BS_GetSweepParams               = 0x2F,
    BS_SetSweepParams               = 0x30,

    BS_StartSweep                   = 0x31,
    BS_StopSweep                    = 0x32,
    BS_SweepData                    = 0x33,
};

}

#endif /* STAR_PRC_COMMANDS_H_ */
