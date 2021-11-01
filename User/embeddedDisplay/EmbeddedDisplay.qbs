import qbs
import qbs.File
import qbs.Process

CppApplication{
    condition: project.target === "raspberryPi"
    name: "cu-embeddeddisplay"
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

    // зависимости для сборки под RaspberryPi
    Properties{
        condition: qbs.buildVariant.contains("debug")
        cpp.defines:
            outer.concat(["VERSION=\""+project.softwareVersion+"\"",
                          "DEBUG"]);
    }
    Properties{
        cpp.defines:
            outer.concat(["VERSION=\""+project.softwareVersion+"\""]);
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
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installPrefix: "/home/pi"
        qbs.installDir: "Software"
    }
}
