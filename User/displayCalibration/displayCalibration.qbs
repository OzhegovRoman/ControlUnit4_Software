import qbs
import qbs.File
import qbs.Process

CppApplication{
    condition: !(project.isWindows || project.isAndroid)
    name: "cu-displaycalibration"
    type: ["application","deployData"]

    consoleApplication: true

    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/RaspPiMMap/",
    ]


    Depends { name: "CuPlugins" }
    Depends { name: "RaspPiMMap" }
    cpp.dynamicLibraries:[
            "wiringPi"
        ]

    Depends {
        name: "Qt";
        submodules: [
            "core",
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
        condition: !project.isWindows
        cpp.defines: outer.concat(
            "VERSION=\""+project.softwareVersion+"\""
        )
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
        condition: !project.isWindows
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/pi/Software"
    }
}
