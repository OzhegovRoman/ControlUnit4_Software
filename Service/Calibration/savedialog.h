#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QAbstractButton>

namespace Ui {
class SaveDialog;
}

class SaveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveDialog(QWidget *parent = nullptr);
    ~SaveDialog();

    void setJsonData(const QJsonObject &jsonData);

private slots:
    void on_tbFilePath_clicked();

    void on_tbPrivateKeyPath_clicked();

    void on_buttonBox_accepted();

    void on_tbWinScp_clicked();

private:
    Ui::SaveDialog *ui;
    QJsonObject mJsonData;

    bool saveToFile();
    bool sendToFtp();

};

#endif // SAVEDIALOG_H
