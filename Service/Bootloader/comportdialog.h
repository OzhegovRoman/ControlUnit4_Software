#ifndef COMPORTDIALOG_H
#define COMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ComPortDialog;
}

class ComPortDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ComPortDialog(QWidget *parent = 0);
    ~ComPortDialog();

    QString portName(void);

    int exec();

private:
    Ui::ComPortDialog *ui;
};

#endif // COMPORTDIALOG_H
