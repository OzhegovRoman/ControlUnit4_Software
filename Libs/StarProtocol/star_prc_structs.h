#ifndef STAR_PRC_STRUCTS
#define STAR_PRC_STRUCTS

template <typename T>
struct pair_t {
    T first;
    T second;
};

#define UART_BAUD_RATE 500000

// CU4SDM0V1 Статус
typedef union {
    struct {
        unsigned char stInite: 1;
        unsigned char stShorted: 1;
        unsigned char stAmplifierOn: 1;
        unsigned char stRfKeyToCmp: 1;
        unsigned char stComparatorOn: 1;
        unsigned char stCounterOn: 1;
        unsigned char stAutoResetOn: 1;
        unsigned char stError: 1;
    };
    unsigned char Data;   // статус
} CU4SDM0_Status_t;

typedef union {
    struct {
        unsigned char stInite: 1;
        unsigned char stShorted: 1;
        unsigned char stAmplifierOn: 1;
        unsigned char stHFModeOn: 1;
        unsigned char stComparatorOn: 1;
        unsigned char stCounterOn: 1;
        unsigned char stAutoResetOn: 1;
        unsigned char stError: 1;
    };
    unsigned char Data;   // статус
} CU4SDM1_Status_t;

// CU4SDM0V1 данные
typedef struct {
    float Current;          // ток смещения детектора в [Амперах]
    float Voltage;          // напряжение смещения детектора в [Вольтах]
    float Counts;           // количество отсчетов
    CU4SDM0_Status_t Status;
} CU4SDM0V1_Data_t;

// CU4SDM1V1 данные
typedef struct {
    float Current;          // ток смещения детектора в [Амперах]
    float Voltage;          // напряжение смещения детектора в [Вольтах]
    float Counts;           // количество отсчетов
    CU4SDM1_Status_t Status;
} CU4SDM1_Data_t;

// Подробное описание битов статуса устройства
#define CU4SDM0V1_STATUS_INITED        (0x01) // устройство проинициализировано / запарковано
#define CU4SDM0V1_STATUS_SHORT_ON      (0x02) // выход закорочен
#define CU4SDM0V1_STATUS_AMP_ON        (0x04) // включен усилитель
#define CU4SDM0V1_STATUS_RF_KEY_CMP    (0x08) // RF ключ переключен на компаратор
#define CU4SDM0V1_STATUS_CMP_ON        (0x10) // компаратор работает
#define CU4SDM0V1_STATUS_COUNTER_ON    (0x20) // счетчик работает
#define CU4SDM0V1_STATUS_AUTO_RES_ON   (0x40) // автосброс работает
#define CU4SDM0V1_STATUS_ERROR         (0x80) // случилась ошибка

#define CU4SDM0V1_STATUS_COUNTER_WORKED (CU4SDM0V1_STATUS_AMP_ON | CU4SDM0V1_STATUS_RF_KEY_CMP | CU4SDM0V1_STATUS_CMP_ON | CU4SDM0V1_STATUS_COUNTER_ON)
#define CU4SDM1V1_STATUS_COUNTER_WORKED (CU4SDM0V1_STATUS_AMP_ON | CU4SDM0V1_STATUS_CMP_ON | CU4SDM0V1_STATUS_COUNTER_ON)

// CU4SDM0V1 параметры
typedef struct {
    float Cmp_Ref_Level;        // уровень компарации в [Вольтах]
    float Time_Const;           // постоянная времени счетчика испульсов в [секундах]
    float AutoResetThreshold;   // напряжение срабатывания системы автосброса в [Вольтах]
    float AutoResetTimeOut;     // время закорачивания системы автосброса в [секундах]
    unsigned int AutoResetCounts;
} CU4SDM0V1_Param_t;

using CU4SDM1_Param_t = CU4SDM0V1_Param_t;

typedef struct {
    float slope;
    float intercept;
} LinRegCoeff_t;


// CU4SDM0V1 уникальные калибровочные константы
// ToDo: завести тип LinReg
typedef struct {
    pair_t<float> Current_ADC;    // из показаний АЦП ток пересчитывается как Current_ADC_slope*ADC_Value+Current_ADC_intercept
    pair_t<float> Voltage_ADC;    // из показаний АЦП напряжение пересчитывается как Voltage_ADC_slope*ADC_Value+Voltage_ADC_intercept
    pair_t<float> Current_DAC;    // требуемое к установке значение ЦАП считается как Value*DAC_slope+DAC_intercept;
    pair_t<float> Cmp_Ref_DAC;
} CU4SDM0V1_EEPROM_Const_t;

typedef struct {
    pair_t<float> Current_ADC;    // из показаний АЦП ток пересчитывается как Current_ADC_slope*ADC_Value+Current_ADC_intercept
    pair_t<float> Voltage_ADC;    // из показаний АЦП напряжение пересчитывается как Voltage_ADC_slope*ADC_Value+Voltage_ADC_intercept
    pair_t<float> Current_DAC;    // требуемое к установке значение ЦАП считается как Value*DAC_slope+DAC_intercept;
    float         PulseWidth;     // время срабатывания детектора в секундах  
    pair_t<float> Cmp_Ref_DAC;
} CU4SDM1_EEPROM_Const_t;

//--------------------------------------------------------------------------------------------------------------
// CU4TDM0V1 данные
typedef struct {
    float Temperature;
    float Pressure;
    float TempSensorCurrent;
    float TempSensorVoltage;
    float PressSensorVoltageP;
    float PressSensorVoltageN;
    bool  CommutatorOn;
} CU4TDM0V1_Data_t;

#define CU4TDM0V1_STATUS_INITED        ((unsigned char) 0x01) // устройство проинициализировано / запарковано
#define CU4TDM0V1_STATUS_SENSOR_ON     ((unsigned char) 0x02) // датчики подключены к измерителю
#define CU4TDM0V1_STATUS_ERROR         ((unsigned char) 0x80) // случилась ошибка

// CU4TDM0V1 уникальные калибровочные константы
typedef struct {
    pair_t<float> tempSensorCurrentAdc;
    pair_t<float> tempSensorCurrentDac;
    pair_t<float> tempSensorVoltage;
    pair_t<float> pressSensorVoltageP;
    pair_t<float> pressSensorVoltageN;
    pair_t<float> pressSensorCoeffs;
} CU4TDM0V1_EEPROM_Const_t;

#define TEMP_TABLE_SIZE 100

struct CU4TDM0V1_Temp_Table_Item_t{
    float Voltage;
    float Temperature;
    inline CU4TDM0V1_Temp_Table_Item_t& operator=(const CU4TDM0V1_Temp_Table_Item_t& other) // copy assignment
    {
        if (this != &other) { // self-assignment check expected
            this->Voltage = other.Voltage;
            this->Temperature = other.Temperature;
        }
        return *this;
    }
};

#endif // STAR_PRC_STRUCTS

