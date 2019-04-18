import qbs

CppApplication{
    condition: !(project.isAndroid || project.isWindows) // только под RaspberryPi
    name: "cu-tcpipserver"
    consoleApplication: true

    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    cpp.cxxLanguageVersion: "c++11"

    Depends { name: "CuPlugins" }
    Depends {
        name: "RaspPiMMap"
        condition: !qbs.targetOS.contains("windows")
    }
    Depends {
        name: "Qt";
        submodules: [
            "core",
            "serialport",
            "network",
        ]
    }

    Group {
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
            "*.ui",
        ]
    }

    Group {
        condition: qbs.architecture.contains("arm")
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/pi/Software"
    }

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
}
