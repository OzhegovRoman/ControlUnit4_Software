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
        condition: !(project.isWindows || project.isAndroid)
    }

    Group {
        name: "Bootloader"
        prefix: "Bootloader/"
        files: [
            "*.*",
        ]
    }

    Properties{
        condition: !(project.isWindows  || project.isAndroid)
        cpp.defines:  [
            "RASPBERRY_PI"
        ]
    }

//    Group {
//        name: "StarProtocol"
//        prefix: "../StarProtocol/"
//        files: [
//            "cstarprotocol.cpp",
//            "cstarprotocol.h",
//            "cstarprotocolpc.cpp",
//            "cstarprotocolpc.h",
//            "star_prc_commands.h",
//            "star_prc_structs.h",
//        ]
//    }

//    Group {
//        name: "Devices"
//        prefix: "Devices/**/"
//        files: [
//            "*.cpp",
//            "*.h"
//        ]
//    }

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
