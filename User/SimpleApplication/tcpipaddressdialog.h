#ifndef TCPIPADDRESSDIALOG_H
#define TCPIPADDRESSDIALOG_H

#include <QDialog>
#include <QTcpSocket>
#include <QHostAddress>

namespace Ui {
class TcpIpAddressDialog;
}

class TcpIpAddressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TcpIpAddressDialog(QWidget *parent = nullptr);
    ~TcpIpAddressDialog() override;

    QHostAddress getAddress();
    QString getPortName();
    bool isSerialProtocol();

    void accept() override;

private slots:
    void on_cbType_activated(int index);

private:
    Ui::TcpIpAddressDialog *ui;
};

#endif // TCPIPADDRESSDIALOG_H
