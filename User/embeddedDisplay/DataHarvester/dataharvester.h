#ifndef DATAHARVESTER_H
#define DATAHARVESTER_H

#include <QObject>
#include "Interfaces/cuiointerfaceimpl.h"
#include "Drivers/commondriver.h"

class DataHarvester: public QObject
{
    Q_OBJECT
public:
    enum harvesterMode{
        hmSingleUnit,
        hmAllUnits,
        hmSleep
    };

    DataHarvester(cuIOInterfaceImpl *interface = nullptr);

    void initialize();

    harvesterMode mode() const;
    void setMode(const harvesterMode &mode);

    QVector<CommonDriver *> drivers() const;

    cuIOInterfaceImpl *interface() const;
    void setInterface(cuIOInterfaceImpl *interface);

private:
    cuIOInterfaceImpl *mInterface;
    QVector<CommonDriver*> mDrivers;
    harvesterMode mMode;

    void initializeDriverList();

};

#endif // DATAHARVESTER_H
