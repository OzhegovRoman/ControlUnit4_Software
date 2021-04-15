#include "opendialog.h"
#include "ui_opendialog.h"
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QDebug>
#include <QtNetwork>
#include <QString>
#include <QInputDialog>
#include <QStringList>

OpenDialog::OpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenDialog)
{
    ui->setupUi(this);
}

OpenDialog::~OpenDialog()
{
    delete ui;
}

QJsonObject OpenDialog::jsonData() const
{
    return mJsonData;
}

void OpenDialog::on_pbOpenFile_clicked()
{
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    QString tmpStr = settings.value("FilePath",QString()).toString()+"\\"+mDeviceType;

    qDebug()<<tmpStr;

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open file...",
                                                    tmpStr,
                                                    QString("DEVICE (%1*.json)").arg(mDeviceName));
    if (fileName.isEmpty()) return;
    QFile loadFile(fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Couldn't open save file.");
        return;
    }
    QJsonDocument loadDoc(QJsonDocument::fromJson(loadFile.readAll()));
    mJsonData = loadDoc.object();
    accept();
}

void OpenDialog::setDeviceType(const QString &deviceType)
{
    mDeviceType = deviceType;
}

void OpenDialog::setDeviceName(const QString &deviceName)
{
    mDeviceName = deviceName;
}

void OpenDialog::on_pbReceiveFromFtp_clicked()
{
    this->setEnabled(false);
    qApp->processEvents();

    QNetworkAccessManager manager;
    manager.setProxy(QNetworkProxy::NoProxy);
    QNetworkRequest request(QString("http://software.scontel.ru/controlUnit4/Calibration/%1/").arg(mDeviceType));

    QNetworkReply *reply = manager.get(request);

    connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error), [=](QNetworkReply::NetworkError code){
        qDebug()<<"Error"<<(int) code;
    });
    connect(reply, &QNetworkReply::sslErrors, this,[=](const QList<QSslError> &sslErrors){
        for (int i = 0; i < sslErrors.size(); i++) {
                qDebug() << "Error " << i << sslErrors.at(i).errorString();
            }
    });

    QElapsedTimer timer;
    timer.start();
    while ((!reply->isFinished()) && (timer.elapsed()<10000))
        qApp->processEvents();
    if (timer.elapsed()>10000){
        reject();
        QMessageBox::warning(this, "Error","Can't download calibration list");
        return;
    }
    QString str = QString(reply->readAll());
    qDebug()<<str;
    QStringList list;

    QStringList tmpstr = str.split(".json\">");
    tmpstr.removeFirst();
    for (QString substr : tmpstr) {
        if (substr.contains(mDeviceName))
            list.append(substr.left(substr.indexOf("</a>")));
    }

    std::sort(list.begin(), list.end(), qGreater<QString>());
    if (list.isEmpty()){
        this->setEnabled(true);
        return;
    }

    bool ok;

    QString fileName = QInputDialog::getItem(this, "Get file...",
                                             "Please download correct version of calibrations",
                                             list,
                                             0,
                                             false,
                                             &ok);

    if ((fileName.isEmpty() || !ok)){
        this->setEnabled(true);
        return;
    }

    QNetworkRequest request2(QString("http://software.scontel.ru/controlUnit4/Calibration/%1/%2")
                             .arg(mDeviceType)
                             .arg(fileName));
    reply = manager.get(request2);
    while (!reply->isFinished())
        qApp->processEvents();
    str = QString(reply->readAll());
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    if (jsonDoc.isObject())
        mJsonData = jsonDoc.object();
    accept();
}
