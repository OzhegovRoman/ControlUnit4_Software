#ifndef CAGILENT34401A_H
#define CAGILENT34401A_H

#include <QObject>

class cAgilent34401A : public QObject
{
    Q_OBJECT
public:
    explicit cAgilent34401A();

    enum {
        Mode_I_DC   = 0,
        Mode_I_AC   = 1,
        Mode_U_DC   = 2,
        Mode_U_AC   = 3,
        Mode_Res    = 4,
        Mode_Res_4P = 5,
        Mode_Freq   = 6,
        Mode_Period = 7
    };

    enum {
        DefRange = 0,
        DefResolution = 0
    };

    quint8 mode() const;
    void setMode(const quint8 &mode);

    qreal range() const;
    void setRange(const qreal &range);

    qreal resolution() const;
    void setResolution(const qreal &resolution);

    bool isInited() const;

    qreal singleRead();

protected:
    virtual quint64 writeAgilent(QString str) = 0;
    virtual qreal readAgilent() = 0;
    virtual bool initialize() {return mInited = true;}

signals:
    void dataReaded(qreal);
    void errorMessage(QString);

public slots:

private:
    quint8 mMode;
    qreal mRange;
    qreal mResolution;
    bool mInited;
};

#endif // CAGILENT34401A_H
