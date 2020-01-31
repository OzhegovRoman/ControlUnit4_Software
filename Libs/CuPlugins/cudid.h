#ifndef CUDID_H
#define CUDID_H

#include <QtGlobal>
#include <QString>

class cUDID
{
public:
    cUDID() = default;
    cUDID(const quint8 *newUDID)
    {
        setUDID(newUDID);
    }
    cUDID(const cUDID &value)
    {
        setUDID(value.mUDID);
    }

    void setUDID (const quint8 *newUDID)
    {
        memcpy(mUDID, newUDID, 12);
    }

    quint8* UDID(){return mUDID;}
    QString toString() {
        QString str;
        QByteArray ba;
        ba.append(reinterpret_cast<const char*>(mUDID), 12);
        str = QString(ba.toHex()).toUpper();
        for (int i = 5; i>0; --i)
            str.insert(i*4,'-');
        return str;
    }

private:
    quint8 mUDID[12]{};
};

#endif // CUDID_H
