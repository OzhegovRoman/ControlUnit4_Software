import qbs
import qbs.TextFile
import qbs.File

CppApplication {
    name: "cu-simpleapp_qml"
    consoleApplication: false

//    condition: project.target === "win_x86"     ||
//               project.target === "win_x86_64"  ||
//               project.target === "android"

    Depends { name: "Qt.core" }
    Depends { name: "Qt.widgets"}
    Depends { name: "Qt.quick" }
    Depends { name: "CuPlugins" }
    Depends { name: "qCustomLib" }

    // preparation RC_File
    Depends {name: "RC_Prepare"}
    RC_Prepare.outputFileName:  name
    RC_Prepare.description:     "Simple Application for Scontel's ControlUnit"
    RC_Prepare.internalName:    "SimpleApp"
    Group {
        name: "Resources_tmp"
        fileTags: "RC_TMPL"
        prefix: "../../qbs/modules/**/"
        files:[
            "*.rc.tmp"
        ]
    }

    cpp.cxxLanguageVersion: "c++14"

    cpp.includePaths:[
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol"
    ]

    cpp.defines: [
        "VERSION=\""+project.softwareVersion+"\""
    ]

    Group {
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
    Group {
        name: "QML"
        files: [
            "*.qrc",
        ]
    }

    Properties {
        condition: project.target === "raspberryPi"
        cpp.driverFlags: [
            "-fPIE",
            "-pie"
        ]
        cpp.linkerFlags:[
            "-z",
            "relro",
            "-rpath",
            "/home/roman/Qt-CrossCompile/raspberrypi3/sysroot/usr/lib/arm-linux-gnueabihf/"
        ]
    }

    // деплой проекта для raspberryPi
    Group {
        condition: project.target === "raspberryPi"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installPrefix: "/home/pi"
        qbs.installDir: "Software"
    }
}


