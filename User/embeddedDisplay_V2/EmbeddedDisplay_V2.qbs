import qbs
import qbs.File
import qbs.Process

CppApplication{
    condition: project.target === "raspberryPi"
    name: "cu-embeddeddisplay_v2"
    type: ["application","deployData"]

    consoleApplication: true

    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/RaspPiMMap/",
        "../../Libs/RiverdiEve"
    ]

    Depends { name: "CuPlugins" }
    Depends { name: "RaspPiMMap" }
    Depends { name: "cu-displaycalibration"}
    Depends { name: "qCustomLib" }
    Depends { name: "RiverdiEve" }
    cpp.dynamicLibraries:[
        "wiringPi"
    ]

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "network",
            "serialport"
        ]
    }

    cpp.cxxLanguageVersion: "c++14"
    cpp.driverFlags: [
        "-fPIE",
        "-pie"
    ]
    cpp.linkerFlags:[
        "-z",
        "relro"
    ]

    // зависимости для сборки под RaspberryPi
    Properties{
        cpp.defines:
            outer.concat("VERSION=\""+project.softwareVersion+"\"");
    }

    Group {
        name: "Source"
        prefix: "**/"
        files: [
            "*.cpp",
            "*.h",
            "*.ui",
        ]
    }
    // деплой проекта для raspberryPi
    Group {
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installPrefix: "/home/pi"
        qbs.installDir: "Software"
    }
}
