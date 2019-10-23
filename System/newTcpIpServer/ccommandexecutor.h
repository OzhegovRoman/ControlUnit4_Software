#ifndef CCOMMANDEXECUTER_H
#define CCOMMANDEXECUTER_H

#include <QObject>

class cCommandExecutor : public QObject
{
    Q_OBJECT
public:
    explicit cCommandExecutor(QObject *parent = nullptr);

signals:
    void finished();

public slots:
    void doWork();
    void stop();
private:
    bool mStopFlag;
};

#endif // CCOMMANDEXECUTER_H
