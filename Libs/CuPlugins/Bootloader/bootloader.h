#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <QObject>
#include <QSerialPort>

#ifndef ANY_DEVICE_ADDRESS
#define ANY_DEVICE_ADDRESS 0xFF
#endif

class Bootloader: public QObject
{
    Q_OBJECT
public:
    explicit Bootloader(QObject *parent = Q_NULLPTR);
    void program(const QString& fileName);
    QString portName() const;
    void setPortName(const QString &portName);

    QString getBootloaderInfo();

    bool initializeLoader();
    void startApplication();

    bool isHotPlugFlagSetted() const;
    void setHotPlugFlag(bool hotPlugFlag);

signals:
    void loaderProgressChanged(int);
    void loaderFinished();
    void restart();
    void infoLoader(QString);
    void errorLoader(QString);

private:
    QString mPortName;
    QSerialPort *mSerialPort;
    int mProgress;
    bool mHotPlugFlag;
    void rebootDevice();
    bool openSerialPort();
    void delayMs(int value);

    void startTransmitte();
    void startReceive();

    bool writeData(const QByteArray& data);

    void clearProgram();
    bool writeProgram(const QString& fileName);
};

#endif // BOOTLOADER_H
