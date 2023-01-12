import qbs
import qbs.File
import qbs.Process

CppApplication{
    name: "cu-measurer"
    consoleApplication: false

    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/qCustomPlot/",
        "../../Libs/RaspPiMMap/",
    ]

    // preparation RC_File
    Depends {name: "RC_Prepare"}
    RC_Prepare.outputFileName:  name
    RC_Prepare.description:     "Quantum efficiecy measurement application"
    RC_Prepare.internalName:    "QEMeasurer"
    RC_Prepare.isDefaultIcon: false

    Group {
        name: "Resources_tmp"
        fileTags: "RC_TMPL"
        prefix: "../../qbs/modules/**/"
        files:[
            "*.rc.tmp"
        ]
    }

    Depends { name: "CuPlugins" }
    Depends { name: "qCustomPlot" }
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
            "network",
            "serialport",
            "widgets",
            "printsupport"
        ]
    }


    cpp.defines: [
//        "RS485",
         "TCPIP_SOCKET_INTERFACE",
    ]

    Group {
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
            "*.ui",
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
