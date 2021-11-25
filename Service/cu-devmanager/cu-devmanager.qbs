import qbs

QtApplication {
    condition: project.target !== "android"
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
            "gui"
        ]
    }

    Depends { name: "qCustomLib" }
    Depends {
        name: "RaspPiMMap"
        required: false
    }

    cpp.cxxLanguageVersion: "c++14"

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
        condition: project.target === "raspberryPi"
        cpp.defines: commonDefines.concat("RASPBERRY_PI");
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
    cpp.defines: commonDefines;

    Group {
        condition: project.target === "raspberryPi"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installPrefix: "/home/pi"
        qbs.installDir: "Software"
    }
}
