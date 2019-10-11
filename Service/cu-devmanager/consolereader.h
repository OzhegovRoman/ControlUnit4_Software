#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H
#include <QObject>
#include <iostream>


#ifdef Q_OS_WIN
#include <QWinEventNotifier>
#include <windows.h>
#else
#include <QSocketNotifier>
#endif

class ConsoleReader : public QObject
{
    Q_OBJECT
public:
    explicit ConsoleReader();
    void run();
    void newCommand();
    void write(QString str);

signals:
    void add(int);
    void remove(int);
    void list();
    void clear();
    void save();
    void quit();
    void description(int);

private slots:
    void readCommand();


private:

#ifdef Q_OS_WIN
    QWinEventNotifier *m_notifier;
#else
    QSocketNotifier *m_notifier;
#endif
};

#endif // CONSOLEREADER_H
