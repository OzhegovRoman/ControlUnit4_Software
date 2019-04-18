import qbs
import qbs.File
import qbs.Process

CppApplication{
    condition: !(project.isWindows || project.isAndroid)
    name: "cu-embeddeddisplay"
    type: ["application","deployData"]

    consoleApplication: true

    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/RaspPiMMap/",
    ]

    Depends { name: "CuPlugins" }
    Depends { name: "RaspPiMMap" }
    Depends { name: "cu-displaycalibration"}
    Depends { name: "qCustomLib" }
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

    cpp.cxxLanguageVersion: "c++11"
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
        condition: qbs.architecture.contains("arm")
        cpp.defines: outer.concat(["VERSION=\""+project.softwareVersion+"\""])
    }

    Group {
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
            "*.ui",
        ]
    }
    // деплой проекта для raspberryPi
    Group {
        condition: qbs.architecture.contains("arm")
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/pi/Software"
    }
}
