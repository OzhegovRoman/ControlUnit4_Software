#include "savedialog.h"
#include "ui_savedialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>

SaveDialog::SaveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaveDialog)
{
    ui->setupUi(this);
    QSettings settings("Scontel", "ControlUnit4_Calibration");
    ui->leFilePath->setText(settings.value("FilePath",QString()).toString());
    ui->lePrivateKey->setText(settings.value("PrivateKeyPath",QString()).toString());
    ui->leWinScp->setText(settings.value("WinScp", QString()).toString());
    ui->gbSaveFile->setChecked(settings.value("SaveToFile",false).toBool());
    ui->gbSaveFtp->setChecked(settings.value("SaveToFtp",false).toBool());
}

SaveDialog::~SaveDialog()
{
    delete ui;
}

void SaveDialog::setJsonData(const QJsonObject &jsonData)
{
    mJsonData = jsonData;
}

void SaveDialog::on_tbFilePath_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Path to Directory containing calibration...", ui->leFilePath->text());
    if (!dir.isEmpty()) ui->leFilePath->setText(dir);
}

void SaveDialog::on_tbPrivateKeyPath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Private key file...",
                                                    ui->lePrivateKey->text(),
                                                    ("Putty private key files (*.ppk)"));
    if (!fileName.isEmpty()) ui->lePrivateKey->setText(fileName);
}

void SaveDialog::on_tbWinScp_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "WinScp Utility...",
                                                    ui->leWinScp->text(),
                                                    ("WinScp Utility (winscp.com)"));
    if (!fileName.isEmpty()) ui->leWinScp->setText(fileName);
}

void SaveDialog::on_buttonBox_accepted()
{
    if (ui->gbSaveFile->isChecked())
        if (!saveToFile()) return;

    if (ui->gbSaveFtp->isChecked())
        if (!sendToFtp()) return;

    QSettings settings("Scontel", "ControlUnit4_Calibration");
    settings.setValue("FilePath",ui->leFilePath->text());
    settings.setValue("PrivateKeyPath",ui->lePrivateKey->text());
    settings.setValue("SaveToFile",ui->gbSaveFile->isChecked());
    settings.setValue("SaveToFtp",ui->gbSaveFtp->isChecked());
    settings.setValue("WinScp", ui->leWinScp->text());

    accept();
}

bool SaveDialog::saveToFile()
{
    //сохраняем данные в файл
    QString fileName = ui->leFilePath->text();
    if (fileName.isEmpty()){
        QMessageBox::critical(this,"Error","Directory string is empty");
        return false;
    }

    QString tmp = mJsonData["DeviceType"].toString();
    if (tmp.isEmpty()){
        QMessageBox::critical(this,"Error","Something wrong at preparing calibration data. Please try again");
        reject();
        return false;
    }

    fileName += '/'+tmp+'/';
    QDir dir;
    if (!dir.mkpath(fileName)){
        QMessageBox::critical(this,"Error","Couldn't create target path.");
        return false;
    }

    tmp =  mJsonData["UDID"].toString();
    if (tmp.isEmpty()){
        QMessageBox::critical(this,"Error","Something wrong at preparing calibration data. Please try again");
        reject();
        return false;
    }

    fileName += tmp+QDateTime::currentDateTime().toString("_yyyy_MM_dd_hh_mm")+".json";

    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error","Couldn't open save file.");
        return false;
    }

    saveFile.write(QJsonDocument(mJsonData).toJson());

    return true;
}

bool SaveDialog::sendToFtp()
{
    bool result = true;

    // проверяем наличие ключа и установленной программы winSCP
    QFileInfo fileinfo(ui->lePrivateKey->text());
    if (!(ui->lePrivateKey->text().toLower().contains(".ppk") && fileinfo.exists())){
        QMessageBox::critical(this,"Error","Thre is some problem with private key filename.");
        return false;
    }

    fileinfo.setFile(ui->leWinScp->text());
    if (!(ui->leWinScp->text().toLower().contains("winscp.com") && fileinfo.exists())){
        QMessageBox::critical(this,"Error","Thre is some problem with WinScp location.");
        return false;
    }

    QString tmp =  mJsonData["UDID"].toString();
    if (tmp.isEmpty()){
        QMessageBox::critical(this,"Error","Something wrong at preparing calibration data. Please try again");
        reject();
        return false;
    }

    QString tmpFileName = qApp->applicationDirPath()+"\\"+tmp+QDateTime::currentDateTime().toString("_yyyy_MM_dd_hh_mm")+".json";

    QFile saveFile(tmpFileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error","Couldn't open save file.");
        return false;
    }

    saveFile.write(QJsonDocument(mJsonData).toJson());
    saveFile.flush();
    saveFile.close();

    // а теперь отправляем этот файл по месту назначения

    tmp = mJsonData["DeviceType"].toString();
    if (tmp.isEmpty()){
        QMessageBox::critical(this,"Error","Something wrong at preparing calibration data. Please try again");
        result = false;
    }

    if (result) {
        QString tmpWinScpScript = qApp->applicationDirPath()+"\\script.tmp";
        QFile script(tmpWinScpScript);
        script.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&script);
        out<<QString("open sftp://ozhegov@rplab.ru -privatekey=%1 -hostkey=\"ssh-ed25519 256 c3:91:45:ad:6d:d4:ee:b1:3a:c1:b9:e3:16:d0:93:d1\"\r\n")
             .arg(ui->lePrivateKey->text());
        out<<QString("put %1 /home/ozhegov/public_html/ControlUnit4/Calibration/%2/\r\n")
             .arg(tmpFileName.replace("/","\\"))
             .arg(mJsonData["DeviceType"].toString());
        out<<QString("exit");
        script.flush();
        script.close();
        QProcess process;
        process.setEnvironment(process.systemEnvironment());
        process.start(ui->leWinScp->text(), QStringList() << "/ini=nul"<<"/script="+qApp->applicationDirPath()+"\\script.tmp");
        process.waitForFinished(10000);

        qDebug()<<process.exitCode();

        script.remove();
    }

    // удаляем временный файл
    saveFile.remove();
    return result;


}
