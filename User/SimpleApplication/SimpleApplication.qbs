import qbs
import qbs.File
import qbs.Process

CppApplication{
    name: "cu-simpleapp"
    consoleApplication: !project.isWindows

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

    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/RaspPiMMap/",
    ]

    Depends { name: "CuPlugins" }
    Depends { name: "qCustomLib" }

    Depends {
        name: "RaspPiMMap"
        condition: !(project.isWindows || project.isAndroid)
    }

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "gui",
            "serialport",
            "widgets",
            "network",
        ]
    }

    cpp.cxxLanguageVersion: "c++11"

    cpp.defines: [
        "VERSION=\""+project.softwareVersion+"\""
    ]

    Properties {
        condition: qbs.architecture.contains("arm")
        cpp.driverFlags: [
            "-fPIE",
            "-pie"
        ]
        cpp.linkerFlags:[
            "-z",
            "relro"
        ]
    }

    Group {
        name: "Forms"
        files: [
            "*.ui"
        ]
    }

    Group {
        name: "Headers"
        files: [
            "*.h",
        ]
    }

    Group {
        name: "Source"
        files: [
            "*.cpp",
        ]
    }

    Group    {
        name: "Resources"
        files:[
            "*.qrc",
        ]
    }

    Group {
        condition: project.isWindows
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: project.installDir
    }

    // деплой проекта для raspberryPi
    Group {
        condition: !(project.isWindows || project.isAndroid)
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/pi/Software"
    }
}
