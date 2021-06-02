#include "savedialog.h"
#include "ui_savedialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QDateTime>
#include <QJsonDocument>
#include <QProcess>
#include <QDebug>
#include <QDesktopServices>
#include <QStandardPaths>
#include <QDir>

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

int SaveDialog::exec()
{
    ui->teLog->setVisible(false);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    resize(0,0);
    return QDialog::exec();
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
    setEnabled(false);
    ui->teLog->clear();
    ui->teLog->setVisible(true);
    resize(0,0);
    qApp->processEvents();

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

    setEnabled(true);
    if (ui->cbCloseAfterSaving->isChecked()) accept();
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

    ui->teLog->appendHtml("<font color=\"Blue\">Save data to file!!!</font>");
    ui->teLog->appendHtml(QString("Save data to the file: %1").arg(fileName));

    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error","Couldn't open save file.");
        return false;
    }

    saveFile.write(QJsonDocument(mJsonData).toJson());
    saveFile.flush();
    saveFile.close();

    ui->teLog->appendHtml(QString("Done!!!"));
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

    QString tmpFileName = QStandardPaths::standardLocations(QStandardPaths::TempLocation)[0]+"\\"+tmp+QDateTime::currentDateTime().toString("_yyyy_MM_dd_hh_mm")+".json";
    tmpFileName = tmpFileName.replace("/","\\");

    ui->teLog->appendHtml("<font color=\"Blue\">Save data to ftp!!!</font>");
    ui->teLog->appendHtml(QString("Preparing tmp_file: %1").arg(tmpFileName));
    QFile saveFile(tmpFileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this,"Error","Couldn't open save file.");
        return false;
    }

    saveFile.write(QJsonDocument(mJsonData).toJson());
    saveFile.flush();
    saveFile.close();
    ui->teLog->appendHtml(QString("Done!!!"));

    // а теперь отправляем этот файл по месту назначения

    tmp = mJsonData["DeviceType"].toString();
    if (tmp.isEmpty()){
        QMessageBox::critical(this,"Error","Something wrong at preparing calibration data. Please try again");
        result = false;
    }

    if (result) {
        QString tmpWinScpScript = QStandardPaths::standardLocations(QStandardPaths::TempLocation)[0]+"/script.tmp";

        ui->teLog->appendHtml(QString("Preparing script file: %1").arg(tmpWinScpScript));
        QFile script(tmpWinScpScript);
        script.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&script);

        out<<QString("open scp://root@software.scontel.ru/ "
                     "-privatekey=\"%1\" "
                     "-hostkey=\"ssh-ed25519 255 OR/XRcDp0G+9TxJ5qw68+a8o4tQoJoJsWNOKOlL2f6I=\" "
                     "-rawsettings Cipher=\"aes,blowfish,3des,chacha20,WARN,arcfour,des\" KEX=\"ecdh,dh-gex-sha1,dh-group14-sha1,dh-group1-sha1,rsa,WARN\"\r\n")
             .arg(ui->lePrivateKey->text());

        out<<QString("option batch continue\r\n");

        out<<QString("mkdir /var/www/html/controlUnit4/Calibration/%1/\r\n")
             .arg(mJsonData["DeviceType"].toString());

        out<<QString("option batch abort\r\n");

        out<<QString("put %1 /var/www/html/controlUnit4/Calibration/%2/\r\n")
             .arg(tmpFileName)
             .arg(mJsonData["DeviceType"].toString());
        out<<QString("exit");
        script.flush();
        script.close();
        ui->teLog->appendHtml(QString("Done!!!"));

        QProcess process;
        process.setEnvironment(process.systemEnvironment());
        QString tmpstr= QString("\"%1\" /ini=nul /script=\"%2\"")
                .arg(ui->leWinScp->text())
                .arg(tmpWinScpScript);
        ui->teLog->appendHtml(QString("Start WinScp process"));
        ui->teLog->appendHtml(tmpstr);
        process.start(tmpstr);

        if (!process.waitForStarted()){
            ui->teLog->appendHtml("<font color=\"Red\">Can't start WinScp process!!!</font>");
            return false;
        }

        do {
            QString tmpStr = process.readAllStandardOutput();
            if (!tmpStr.isEmpty())
                ui->teLog->appendHtml(tmpStr);
            tmpStr = process.readAllStandardError();
            if (!tmpStr.isEmpty())
                ui->teLog->appendHtml(QString("<font color=\"Red\">%1</font>").arg(tmpStr));
            qApp->processEvents();
        } while (!process.waitForFinished(10));

        QString tmpStr = process.readAllStandardOutput();
        if (!tmpStr.isEmpty())
            ui->teLog->appendHtml(tmpStr);
        tmpStr = process.readAllStandardError();
        if (!tmpStr.isEmpty())
            ui->teLog->appendHtml(QString("<font color=\"Red\">%1</font>").arg(tmpStr));
        qApp->processEvents();

        qDebug()<<process.exitCode();
        //script.remove();
    }

    // удаляем временный файл
    ui->teLog->appendHtml(QString("Delete tmp_file"));
    saveFile.remove();
    ui->teLog->appendHtml(QString("Done!!!"));
    return result;


}

void SaveDialog::on_tbDownload_clicked()
{
    QDesktopServices::openUrl(QUrl("https://winscp.net/eng/download.php"));
}
