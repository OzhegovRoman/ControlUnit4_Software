#ifndef CUABSTRACTDRIVER_H
#define CUABSTRACTDRIVER_H

#include <QObject>
#include <QTimer>
#include "../cuiodeviceimpl.h"
#include "../cudid.h"
#include <QDebug>

//#define DRIVER_TIME_OUT 2000

template<typename T>
class cuDeviceParam;

template<typename T>
class cuDeviceParam_settable;

class AbstractDriver : public cuIODeviceImpl
{
    Q_OBJECT
public:
    explicit AbstractDriver(QObject *parent = nullptr);
    ~AbstractDriver() override;

    // Инициализация устройства
    void init();

    // Запарковать устройство
    void goToPark();

    // Оглушить устройство
    void listeningOff();

    // Получить статус устройства
    void getStatus();

    // Получить последнюю из случившихся ошибок
    void getLastError();

    // Получить полное описание устройства
    void rebootDevice();

    void writeEeprom();

    bool waitingAnswer();

    bool getDeviceInfo();

    cuDeviceParam<QString> *getDeviceType() const;

    cuDeviceParam<QString> *getModificationVersion() const;

    cuDeviceParam<QString> *getHardwareVersion() const;

    cuDeviceParam<QString> *getFirmwareVersion() const;

    cuDeviceParam<QString> *getDeviceDescription() const;

    cuDeviceParam<cUDID> *getUDID() const;

    int getDriverTimeOut() const;
    void setDriverTimeOut(int driverTimeOut);

signals:
    void eepromConstWrited();

public slots:
    void sendMsg(quint8 command, quint8 dataLength, quint8* data) override;

private slots:
    void waitingTimerTimeOut();

protected:
    bool pMsgReceived(quint8 address, quint8 command, quint8 dataLength, quint8* data) override;

private:
    bool mAnswerReceive {false};
    bool mTimeOut {false};
    int mDriverTimeOut {500};
    QTimer *mTimer;
    cuDeviceParam<QString> *mDeviceType;
    cuDeviceParam<QString> *mModificationVersion;
    cuDeviceParam<QString> *mHardwareVersion;
    cuDeviceParam<QString> *mFirmwareVersion;
    cuDeviceParam<QString> *mDeviceDescription;
    cuDeviceParam<cUDID> *mUDID;
};

template<typename T>
class cuDeviceParam
{
public:

    cuDeviceParam(AbstractDriver *parent = nullptr, quint8 command = 0):
        mDriver(parent),
        mCommand(command),
        mValue(T())
    {
    }

    /**
     * @brief getCurrentValue
     * @return
     */
    T getCurrentValue() const {
        return mValue;
    }

    /**
     * @brief setCurrentValue
     * @param value
     */
    void setCurrentValue(const T &value)
    {
        mValue = value;
    }

    /**
     * @brief getValue
     * @return
     */
    void getValue() {
        mDriver->sendMsg(mCommand, 0, nullptr);
    }

    /**
     * @brief getValueSequence
     * @param ok
     * @return
     */
    T getValueSequence(bool *ok = nullptr, int try_count = 1) {
        bool tmp = false;
        while (try_count-- > 0){
            getValue();
            tmp = mDriver->waitingAnswer();
            if (tmp)
                break;
        }
        if (ok)
            *ok = tmp;
        return getCurrentValue();
    }
private:
    AbstractDriver *mDriver;
    quint8 mCommand;
    T mValue;

    friend AbstractDriver;
    friend cuDeviceParam_settable<T>;
};

template<typename T>
class cuDeviceParam_settable : public cuDeviceParam<T>
{
public:
    cuDeviceParam_settable(AbstractDriver *parent = nullptr, quint8 command = 0)
        : cuDeviceParam<T>(parent, command)
    {

    }

    void setValue(const T &value) {
        cuDeviceParam<T>::mDriver->sendMsg(cuDeviceParam<T>::mCommand+1, sizeof(T), (quint8*) &value);
        mLastSettedValue = value;
        mUnsettedPreviously = false;
    }

    void setValueSequence(const T &value, bool *ok = nullptr, int try_count = 1) {
        bool tmp = false;
        while (try_count-- > 0){

            setValue(value);
            tmp = cuDeviceParam<T>::mDriver->waitingAnswer();

            if (tmp){
                break;
            }
        }
        if (ok)
            *ok = tmp;
    }

    T lastSettedValue() {
        return mLastSettedValue;
    }
    bool isUnsettedPreviously() {
        return mUnsettedPreviously;
    }
private:
    T mLastSettedValue;
    bool mUnsettedPreviously {true};
};

#endif // CUABSTRACTDRIVER_H
