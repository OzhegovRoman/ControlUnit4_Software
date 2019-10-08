QT += quick
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RESOURCES += qml.qrc

defineReplace(droidVersionCode) {
        segments = $$split(1, ".")
        for (segment, segments): vCode = "$$first(vCode)$$format_number($$segment, width=3 zeropad)"
        contains(ANDROID_TARGET_ARCH, arm64-v8a): \
            suffix = 1
        contains(ANDROID_TARGET_ARCH, armeabi-v7a): \
            suffix = 0
        # add more cases as needed
        return($$first(vCode)$$first(suffix))
}

VERSION = 1.1.6

ANDROID_VERSION_NAME = $$VERSION

ANDROID_VERSION_CODE = $$droidVersionCode($$ANDROID_VERSION_NAME)


DEFINES += \
     VERSION=\\\"1.1.6\\\"

HEADERS += \
    appcore.h \
    sspddatamodel.h \
    tcpipvalidator.h

SOURCES += \
        main.cpp \
        appcore.cpp \
        sspddatamodel.cpp \
        tcpipvalidator.cpp
win32:{
    CONFIG(debug, debug|release){
        LIBS += \
            -L../../Libs/CuPlugins/debug -lCuPlugins \
            -L../../Libs/qCustomLib/debug -lqCustomLib
    }
    CONFIG(release, debug|release){
        LIBS += \
            -L../../Libs/CuPlugins/release -lCuPlugins \
            -L../../Libs/qCustomLib/release -lqCustomLib
    }
}
else: {
        LIBS += \
            -L../../Libs/CuPlugins -lCuPlugins \
            -L../../Libs/qCustomLib -lqCustomLib
}

DEPENDPATH += \
    ../../Libs/CuPlugins/ \
    ../../Libs/StarProtocol/
#warning()

INCLUDEPATH += \
    ../../Libs/CuPlugins/ \
    ../../Libs/StarProtocol/


# Additional import path used to resolve QML modules in Qt Creator's code model
# QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
# QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG += mobility
MOBILITY =

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    WorkForms/ChangableItem.qml \
    WorkForms/SetttingsModel.qml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
