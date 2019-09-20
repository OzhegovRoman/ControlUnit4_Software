TEMPLATE = lib

QT += core serialport network widgets

CONFIG += staticlib

INCLUDEPATH += ../StarProtocol/

HEADERS += \
    Bootloader/bootloader.h \
    Drivers/adriver.h \
    Drivers/ccu4sdm0driver.h \
    Drivers/ccu4tdm0driver.h \
    Interfaces/abstractiointerface.h \
    Interfaces/cuiointerface.h \
    Interfaces/cuiointerfaceimpl.h \
    Interfaces/curs485iointerface.h \
    Interfaces/cutcpsocketiointerface.h \
    Server/cuserver.h \
    Server/servercommands.h \
    cudid.h \
    cuiodevice.h \
    cuiodeviceimpl.h

SOURCES += \
    Bootloader/bootloader.cpp \
    Drivers/adriver.cpp \
    Drivers/ccu4sdm0driver.cpp \
    Drivers/ccu4tdm0driver.cpp \
    Interfaces/cuiointerfaceimpl.cpp \
    Interfaces/curs485iointerface.cpp \
    Interfaces/cutcpsocketiointerface.cpp \
    Server/cuserver.cpp \
    cuiodeviceimpl.cpp
