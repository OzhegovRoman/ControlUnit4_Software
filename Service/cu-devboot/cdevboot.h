#ifndef CDEVBOOT_H
#define CDEVBOOT_H

#include <QObject>
#include <QMap>
#include <QDebug>

class cDevBoot : public QObject
{
    Q_OBJECT
public:
    explicit cDevBoot(QObject *parent = nullptr);
    QString getPortName() const;
    void setPortName(const QString &portName);

    int address() const;
    void setAddress(int address);

    bool isLoadFromURL() const;
    void setLoadFromURL(bool loadFromURL);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString getUrl() const;
    void setUrl(const QString &url);

    bool isForce() const;
    void setForce(bool force);

    QString devType() const;
    void setDevType(const QString &value);

    QString fwVersion() const;
    void setFwVersion(const QString &value);

    void startProcess();

    bool isHotPlug() const;
    void setHotPlug(bool hotPlug);

private:
    QString mPortName;
    int mAddress;
    bool mLoadFromURL;
    bool mForce;
    bool mHotPlug;
    QString mFileName;
    QString mUrl;
    QString mDevType;
    QString mFirmwareVersion;
    QMap<QString, QStringList> mVersionType;
    QMap<QString, QStringList> mTypeVersion;

    QStringList getFileList(QUrl url);
    void downloadFile(QUrl url);
    static bool compareVersion(const QString &str1, const QString &str2);
    void prepareVersionTypeMaps();
    void enumerateDevice();
    bool isDeviceTypeCorrect();
    bool isFirmwareVersionCorrect();
    bool isFirmwareVersionAndTypeCorrectTogether();
    void prepareOptions();
    void disableAllDEvices();
    void bootloaderStart();
    void rebootAllDevices();
signals:

public slots:
    void bootloaderProgressChanged(int progress);
    void bootloaderFinished();
    void bootloaderInfo(QString str);
};


#endif // CDEVBOOT_H
