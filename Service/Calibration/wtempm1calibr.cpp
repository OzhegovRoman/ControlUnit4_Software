#include "wtempm1calibr.h"
#include "ui_wtempm1calibr.h"
#include <QDebug>
#include <QJsonArray>

wTempM1Calibr::wTempM1Calibr(QWidget *parent) :
    DriverWidget(parent),
    ui(new Ui::wTempM1Calibr)
{
    ui->setupUi(this);
    for (int i = 0; i< 4; ++i){
        mSupportWidget[i] = new wTempM1SupportWidget(this);
        QVBoxLayout *vbox = new QVBoxLayout();
        vbox->addWidget(mSupportWidget[i]);
        ui->tabWidget->widget(i)->setLayout(vbox);
    }
}

wTempM1Calibr::~wTempM1Calibr()
{
    delete ui;
}

void wTempM1Calibr::setDriver(CommonDriver *driver)
{
    mTempDriver = qobject_cast<TempDriverM1*>(driver);
    if (mTempDriver){
        DriverWidget::setDriver(driver);
    }
}

void wTempM1Calibr::enableGUI(bool enable)
{
    ui->tabWidget->setEnabled(enable);
}

void wTempM1Calibr::getEeprom()
{
    bool ok = mTempDriver->readEepromCoeffs();
    if (!ok) return;

    ok = mTempDriver->readDefaultParams();
    if (!ok) return;

    ok = mTempDriver->receiveTempTables();
    if (!ok) return;

    for (int i = 0; i < 4; ++i){
        mSupportWidget[i]->setEepromCoeff(mTempDriver->eepromCoeff(i));
        mSupportWidget[i]->setDefaultParam(mTempDriver->defaultParam(i));
        mSupportWidget[i]->setTemperatureTable(mTempDriver->tempTable(static_cast<uint8_t>(i)));
    }
}

void wTempM1Calibr::setEeprom()
{
    for (int i = 0; i < 4; ++i){
        mTempDriver->setDefaultParam(i, mSupportWidget[i]->getDefaultParam());
        mTempDriver->setEepromCoeff(i, mSupportWidget[i]->eepromCoeff());
        mTempDriver->setTempTable(i, mSupportWidget[i]->temperatureTable());
    }

    bool ok = mTempDriver->writeDefaultParams();
    if (!ok) return;
    ok = mTempDriver->writeEepromCoeffs();
    if (!ok) return;
    ok = mTempDriver->sendTempTables();
    if (!ok) return;
}

QJsonObject wTempM1Calibr::eepromToJson()
{
    QJsonObject calibrObject;
    QJsonArray channelValue;
    for (int i = 0; i < 4; ++i){
        QJsonArray value;
        QJsonObject channelObject;
        auto eepromConst = mSupportWidget[i]->eepromCoeff();
        value.append(static_cast<double>(eepromConst.current.first));
        value.append(static_cast<double>(eepromConst.current.second));
        channelObject["CurrentCoeffs"] = value;
        value[0] = static_cast<double>(eepromConst.voltage.first);
        value[1] = static_cast<double>(eepromConst.voltage.second);
        channelObject["VoltageCoeffs"] = value;

        auto defaultParams = mSupportWidget[i]->getDefaultParam();
        channelObject["DefaultCurrent"] = static_cast<double>(defaultParams.current);
        channelObject["Enable"] = defaultParams.enable;

        QJsonArray table;
        CU4TDM0V1_Temp_Table_Item_t *tTable = mSupportWidget[i]->temperatureTable();
        for (int j = 0; j < TEMP_TABLE_SIZE; ++j){
            value[0] = static_cast<double>(tTable[j].Voltage);
            value[1] = static_cast<double>(tTable[j].Temperature);
            table.append(value);
        }
        channelObject["TempTable"] = table;
        channelValue.append(channelObject);
    }
    calibrObject["Data"] = channelValue;
    return calibrObject;
}

void wTempM1Calibr::eepromFromJson(QJsonObject data)
{
    QJsonArray channelValue = data["Data"].toArray();
    for (int i = 0; i< 4; ++i){
        QJsonObject channelObject = channelValue[i].toObject();
        TDM1_EepromCoeff eepromCoeff;
        QJsonArray value = channelObject["CurrentCoeffs"].toArray();
        eepromCoeff.current.first  = value[0].toDouble(0);
        eepromCoeff.current.second = value[1].toDouble(0);
        value = channelObject["VoltageCoeffs"].toArray();
        eepromCoeff.voltage.first  = value[0].toDouble(0);
        eepromCoeff.voltage.second = value[1].toDouble(0);
        mSupportWidget[i]->setEepromCoeff(eepromCoeff);

        DefaultParam params;
        params.current = channelObject["DefaultCurrent"].toDouble(0);
        params.enable  = channelObject["Enable"].toBool(false);
        mSupportWidget[i]->setDefaultParam(params);

        QJsonArray table = channelObject["TempTable"].toArray();
        CU4TDM0V1_Temp_Table_Item_t mTable[TEMP_TABLE_SIZE];
        for (int i = 0; i< TEMP_TABLE_SIZE; ++i){
            value = table[i].toArray();
            mTable[i].Voltage     = value[0].toDouble();
            mTable[i].Temperature = value[1].toDouble();
        }
        mSupportWidget[i]->setTemperatureTable(mTable);
    }
}
