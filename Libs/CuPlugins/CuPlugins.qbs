import qbs

StaticLibrary {
    name: "CuPlugins"

    cpp.includePaths: [
        "../StarProtocol/",
        "."
    ]

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion: "c++14"
    Depends {
        name: "Qt";
        submodules: [
            "core",
            "serialport",
            "widgets",
            "network"
        ]
    }
    Depends {name: "StarProtocol"}

    Depends {
        name: "RaspPiMMap"
        condition: project.isRaspberryPi
    }

    Group {
        name: "Bootloader"
        prefix: "Bootloader/"
        files: [
            "*.*",
        ]
    }

    Properties{
        condition: project.isRaspberryPi
        cpp.defines:  [
            "RASPBERRY_PI"
        ]
    }

    Group {
        name: "Drivers"
        prefix: "Drivers/**/"
        files: [
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "Interfaces"
        prefix: "Interfaces/**/"
        files: [
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "Server"
        prefix: "Server/**/"
        files: [
            "*.cpp",
            "*.h"
        ]
    }

    Group {
        name: "CommonFiles"
        files: [
            "*.h",
            "*.cpp",
        ]
    }
}
