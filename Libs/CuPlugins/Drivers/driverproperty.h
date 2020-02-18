#ifndef DRIVERPROPERTY_H
#define DRIVERPROPERTY_H

#include "abstractdriver_v2.h"
#include <QObject>
#include <QVariant>
#include "../cudid.h"

class ServiceSignal {
public:

    ServiceSignal() : current_id_(0) {}

    // copy creates new signal
    ServiceSignal(ServiceSignal const& other) : current_id_(0)
    {
        Q_UNUSED(other)
    }

    // connects a std::function to the signal. The returned
    // value can be used to disconnect the function again
    int connect(std::function<void()> const& slot) const {
        slots_.insert(std::make_pair(++current_id_, slot));
        return current_id_;
    }

    // disconnects a previously connected function
    void disconnect(int id) const {
        slots_.erase(id);
    }

    // disconnects all previously connected functions
    void disconnect_all() const {
        slots_.clear();
    }

    // calls all connected functions
    void emit_signal() {
        for(auto const& it : slots_) {
            it.second();
        }
    }

    // assignment creates new Signal
    ServiceSignal& operator=(ServiceSignal const& other) {
        Q_UNUSED(other)
        disconnect_all();
        return *this;
    }

private:
    mutable std::map<int, std::function<void()>> slots_;
    mutable int current_id_;
};

class TempDriverM0;

class DriverProperty_p
{
public:
    virtual ~DriverProperty_p(){}
    quint8 cmdGetter() const;
    void setCmdGetter(const quint8 &cmdGetter);

    quint8 cmdSetter() const;
    void setCmdSetter(const quint8 &cmdSetter);

    virtual ServiceSignal *gettedSignal();
    virtual ServiceSignal *settedSignal();

protected:
    DriverProperty_p(AbstractDriver_V2 * parent, quint8 cmdGetter, quint8 cmdSetter = 0xFF);

    void readWrite(quint8 cmd, QByteArray data);
    QByteArray readWriteSync(quint8 cmd, QByteArray data, bool *ok = nullptr, int tryCount = 1);

    QByteArray data() const;
    void setBufferData(const QByteArray &buffer);
    QByteArray lastSettedData() const;

private:
    AbstractDriver_V2 *mDriver;
    quint8 mCmdGetter;
    quint8 mCmdSetter;
    QByteArray mBuffer;
    QByteArray mLastSettedValue;
    ServiceSignal *mGettedSignal;
    ServiceSignal *mSettedSignal;

    friend AbstractDriver_V2;
    friend TempDriverM0; //к сожалению нужен доступ к побитному вводу выводу для получения температурной таблицы
};

class DriverCommand: protected DriverProperty_p
{
public:
    DriverCommand(AbstractDriver_V2 * parent, quint8 command)
        : DriverProperty_p(parent, 0xFF, command)
    {}

    void execute();
    void executeSync(bool *ok = nullptr, int tryCount = 1);

    ServiceSignal *executedSignal()
    {
        return DriverProperty_p::settedSignal();
    }

};

template <typename T>
class DriverProperty: protected DriverProperty_p
{
public:
    DriverProperty(AbstractDriver_V2* parent, quint8 cmdGetter, quint8 cmdSetter)
        : DriverProperty_p(parent, cmdGetter, cmdSetter)
    {}
    virtual ~DriverProperty() override
    {}

    virtual void getValue()
    {
        readWrite(cmdGetter(), QByteArray());
    }

    virtual T getValueSync(bool *ok = nullptr, const int &tryCount = 1)
    {
        return fromByteArray(readWriteSync(cmdGetter(), QByteArray(), ok, tryCount));
    }

    virtual void setCurrentValue(const T & value)
    {
        setBufferData(toByteArray(value));
    }

    virtual T currentValue()
    {
        return fromByteArray(data());
    }

    virtual void setValue(const T &value)
    {
        readWrite(cmdSetter(), toByteArray(value));
    }

    virtual void setValueSync(const T &value, bool *ok = nullptr, const int &tryCount = 1)
    {
        readWriteSync(cmdSetter(), toByteArray(value), ok, tryCount);
    }

    virtual T lastSettedValue() const
    {
        return fromByteArray(lastSettedData());
    }

    virtual ServiceSignal *gettedSignal() override
    {
        return DriverProperty_p::gettedSignal();
    }

    virtual ServiceSignal *settedSignal() override
    {
        return DriverProperty_p::settedSignal();
    }


protected:
    static QByteArray toByteArray(const T &data);
    static T fromByteArray(const QByteArray &ba);
};

template <typename T>
class DriverPropertyReadOnly: protected DriverProperty<T>
{
public:
    DriverPropertyReadOnly(AbstractDriver_V2* parent, quint8 cmdGetter)
        : DriverProperty<T>(parent, cmdGetter, 0xFF)
    {}

    virtual void getValue() override
    {
        DriverProperty<T>::getValue();
    }

    virtual T getValueSync(bool *ok = nullptr, const int &tryCount = 1) override
    {
        return DriverProperty<T>::getValueSync(ok, tryCount);
    }

    virtual T currentValue() override
    {
        return DriverProperty<T>::currentValue();
    }

    virtual void setCurrentValue(const T & value)
    {
        DriverProperty<T>::setCurrentValue(value);
    }

    virtual ServiceSignal *gettedSignal() override
    {
        return DriverProperty<T>::gettedSignal();
    }
};

template <typename T>
class DriverPropertyWriteOnly: protected DriverProperty<T>
{
public:
    DriverPropertyWriteOnly(AbstractDriver_V2* parent, quint8 cmdSetter)
        : DriverProperty<T>(parent, 0xFF, cmdSetter)
    {}

    virtual void setValue(const T &value) override
    {
        DriverProperty<T>::setValue(value);
    }

    virtual void setValueSync(const T &value, bool *ok = nullptr, const int &tryCount = 1) override
    {
        DriverProperty<T>::setValueSync(value, ok, tryCount);
    }

    virtual T lastSettedValue() const override
    {
        return DriverProperty<T>::lastSettedValue();
    }

    virtual ServiceSignal *settedSignal() override
    {
        return DriverProperty<T>::settedSignal();
    }
};

template<typename T> inline
QByteArray DriverProperty<T>::toByteArray(const T &data)
{
    return QByteArray(reinterpret_cast<const char*>(&data), sizeof(T));
}

template<typename T> inline
T DriverProperty<T>::fromByteArray(const QByteArray &ba)
{
    auto tmpValue = reinterpret_cast<const T*>(ba.data());
    return *tmpValue;
}

template<> inline
cUDID DriverProperty<cUDID>::fromByteArray(const QByteArray &ba)
{
    cUDID tmpUDID(reinterpret_cast<const quint8*>(ba.data()));
    return tmpUDID;
}

template<> inline
QString DriverProperty<QString>::fromByteArray(const QByteArray &ba)
{
    return QString(ba);
}

#endif // DRIVERPROPERTY_H
