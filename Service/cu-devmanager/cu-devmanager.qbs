import qbs

QtApplication {
    condition: !project.isAndroid
    name: "cu-devmngr"
    consoleApplication: true

    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    Depends { name: "CuPlugins" }
    Depends {
        name: "Qt";
        submodules: [
            "core",
            "serialport",
            "network",
        ]
    }

    Depends { name: "qCustomLib" }
    Properties{
        condition: !project.isWindows
        Depends {
            name: "RaspPiMMap"
        }
        cpp.dynamicLibraries:[
            "wiringPi"
        ]
    }

    cpp.cxxLanguageVersion: "c++11"

    Group {
        name: "Source"
        files: [
            "*.cpp",
        ]
    }

    Group {
        name: "Headers"
        files: [
            "*.h",
        ]
    }

    property stringList commonDefines: ["VERSION=\""+project.softwareVersion+"\""]
    Properties{
        condition: qbs.architecture.contains("arm");
        cpp.defines: commonDefines.concat("RASPBERRY_PI");
        cpp.driverFlags: [
            "-fPIE",
            "-pie"
        ]
        cpp.linkerFlags:[
            "-z",
            "relro"
        ]
    }
    cpp.defines: commonDefines;

    Group {
        condition: qbs.architecture.contains("arm")
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/pi/Software"
    }
}
