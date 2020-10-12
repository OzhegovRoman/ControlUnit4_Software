#ifndef COMMONWIDGET_H
#define COMMONWIDGET_H

#include <QWidget>

class CommonWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CommonWidget(QWidget *parent = nullptr);

    virtual void closeWidget();
    virtual void openWidget();
    virtual void updateWidget();

signals:

};

#endif // COMMONWIDGET_H
