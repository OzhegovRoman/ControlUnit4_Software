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
    //    SD_GetCurrentMonitor            ,
    //    SD_GetCurrentMonitorAdcCoeff    ,
    //    SD_SetCurrentMonitorAdcCoeff
    SD_SetHFMode        ,      // HighFrequency mode
    SD_GetHFMode        ,      // HighFrequency mode
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
}

#endif /* STAR_PRC_COMMANDS_H_ */
