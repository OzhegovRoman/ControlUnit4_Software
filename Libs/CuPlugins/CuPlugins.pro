TEMPLATE = lib

QT += core serialport network widgets

CONFIG += staticlib

INCLUDEPATH += ../StarProtocol/

HEADERS += \
    Bootloader/bootloader.h \
    Drivers/abstractdriver.h \
    Drivers/commondriver.h \
    Drivers/driverproperty.h \
    Drivers/sspddriverm0.h \
    Drivers/sspddriverm1.h \
    Drivers/tempdriverm0.h \
    Drivers/tempdriverm1.h \
    Interfaces/abstractiointerface.h \
    Interfaces/cuiointerface.h \
    Interfaces/cuiointerfaceimpl.h \
    Interfaces/curs485iointerface.h \
    Interfaces/cutcpsocketiointerface.h \
    crelaysstatus.h \
    cudid.h \
    cuiodevice.h \
    cuiodeviceimpl.h

SOURCES += \
    Bootloader/bootloader.cpp \
    Drivers/abstractdriver.cpp \
    Drivers/commondriver.cpp \
    Drivers/driverproperty.cpp \
    Drivers/sspddriverm0.cpp \
    Drivers/sspddriverm1.cpp \
    Drivers/tempdriverm0.cpp \
    Drivers/tempdriverm1.cpp \
    Interfaces/cuiointerfaceimpl.cpp \
    Interfaces/curs485iointerface.cpp \
    Interfaces/cutcpsocketiointerface.cpp \
    crelaysstatus.cpp \
    cuiodeviceimpl.cpp
