import qbs
import qbs.File
import qbs.Process

CppApplication{
    name: "cu-simpleapp"
    consoleApplication: false

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
        required: false
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

    cpp.cxxLanguageVersion: "c++14"

    cpp.defines: [
        "VERSION=\""+project.softwareVersion+"\""
    ]

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
            "/home/roman/raspi/sysroot/opt/vc/lib/"
        ]
        cpp.defines: outer.concat("RASPBERRY_PI")
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
