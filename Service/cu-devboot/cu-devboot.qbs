import qbs

QtApplication {
    consoleApplication: true
    condition: !project.isAndroid
    name: "cu-devboot"

    type: ["application"]

    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    Depends { name: "CuPlugins" }
    Depends {
        name: "RaspPiMMap"
        condition: !project.isWindows
    }
    Depends {
        name: "Qt";
        submodules: [
            "core",
            "serialport",
            "network",
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
        condition: project.isWindows
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: project.installDir
    }

    Group {
        condition: qbs.architecture.contains("arm")
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "/home/pi/Software"
    }
}