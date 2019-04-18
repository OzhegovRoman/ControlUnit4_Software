TEMPLATE = lib

QT += core serialport network widgets

CONFIG += staticlib

INCLUDEPATH += ../StarProtocol/

HEADERS += \
    Bootloader/bootloader.h \
    cudid.h \
    cuiodevice.h \
    cuiodeviceimpl.h \
    Devices/AbstractDevices/acudevice.h \
    Devices/AbstractDevices/acusdm0.h \
    Devices/AbstractDevices/acutdm0.h \
    Devices/AbstractDevices/devicestructs.h \
    Devices/FakeDevices/fcudevice.h \
    Devices/FakeDevices/fcusdm0.h \
    Devices/FakeDevices/fcutdm0.h \
    Drivers/adriver.h \
    Drivers/ccu4sdm0driver.h \
    Drivers/ccu4tdm0driver.h \
    Interfaces/aointerface.h \
    Interfaces/cuiointerface.h \
    Interfaces/cuiointerfaceimpl.h \
    Interfaces/curs485iointerface.h \
    Interfaces/cutcpsocketiointerface.h \
    Interfaces/fiointerface.h \
    Server/cuserver.h \
    Server/servercommands.h \
    ../StarProtocol/cstarprotocol.h \
    ../StarProtocol/cstarprotocolpc.h \
    ../StarProtocol/star_prc_commands.h \
    ../StarProtocol/star_prc_structs.h

SOURCES += \
    Bootloader/bootloader.cpp \
    cuiodeviceimpl.cpp \
    Devices/AbstractDevices/acudevice.cpp \
    Devices/AbstractDevices/acusdm0.cpp \
    Devices/AbstractDevices/acutdm0.cpp \
    Devices/FakeDevices/fcudevice.cpp \
    Devices/FakeDevices/fcusdm0.cpp \
    Devices/FakeDevices/fcutdm0.cpp \
    Drivers/adriver.cpp \
    Drivers/ccu4sdm0driver.cpp \
    Drivers/ccu4tdm0driver.cpp \
    Interfaces/cuiointerfaceimpl.cpp \
    Interfaces/curs485iointerface.cpp \
    Interfaces/cutcpsocketiointerface.cpp \
    Interfaces/fiointerface.cpp \
    Server/cuserver.cpp \
    ../StarProtocol/cstarprotocol.cpp \
    ../StarProtocol/cstarprotocolpc.cpp

RESOURCES += \
    Devices/FakeDevices/fdevicesresources.qrc
