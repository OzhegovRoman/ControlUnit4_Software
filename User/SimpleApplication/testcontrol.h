#ifndef TESTCONTROL_H
#define TESTCONTROL_H

#include <QDialog>
#include <Interfaces/cuiointerface.h>

namespace Ui {
class TestControl;
}

class TestControl : public QDialog
{
    Q_OBJECT

public:
    explicit TestControl(QWidget *parent = nullptr);
    ~TestControl();

    void setDeviceList(const QStringList &newDeviceList);

    void setInterface(cuIOInterface *newInterface);

private:
    Ui::TestControl *ui;

    QStringList mDeviceList;

    cuIOInterface *mInterface;

    // QDialog interface
public slots:
    virtual int exec() override;
private slots:
    void on_pushButton_2_clicked();
};

#endif // TESTCONTROL_H
