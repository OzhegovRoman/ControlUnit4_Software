QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#TARGET = androidtest
#TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
    VERSION=\\\"1.0.0\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += \
    -L../../Libs/CuPlugins -lCuPlugins \
    -L../../Libs/qCustomLib -lqCustomLib



INCLUDEPATH += \
    ../../Libs/CuPlugins/ \
    ../../Libs/StarProtocol/


SOURCES += \
    main.cpp \
    maindialog.cpp \
    sspdwidget.cpp \
    tcpipaddressdialog.cpp \
    tempwidget.cpp

HEADERS += \
    maindialog.h \
    sspdwidget.h \
    tcpipaddressdialog.h \
    tempwidget.h

FORMS += \
    maindialog.ui \
    sspdwidget.ui \
    tcpipaddressdialog.ui \
    tempwidget.ui

CONFIG += mobility
MOBILITY =
