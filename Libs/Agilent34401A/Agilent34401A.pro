#-------------------------------------------------
#
# Project created by QtCreator 2016-02-26T12:29:22
#
#-------------------------------------------------

QT       -= gui

TARGET = Agilent34401A
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    cagilent34401a.cpp \
    cagilent34401agpibinterface.cpp

HEADERS += \
    cagilent34401a.h \
    cagilent34401agpibinterface.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#GPIB
unix|win32: LIBS += $$PWD/../GPIB/libgpib.a

INCLUDEPATH += $$PWD/../GPIB
DEPENDPATH += $$PWD/../GPIB
