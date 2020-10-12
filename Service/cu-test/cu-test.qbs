import qbs

QtApplication {
    consoleApplication: true
    condition: project.target !== "android"
    name: "cu-test"

    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    Depends { name: "CuPlugins" }
    Depends {
        name: "RaspPiMMap"
        required: false
    }
    Depends {
        name: "Qt";
        submodules: [
            "core",
            "serialport",
            "network",
        ]
    }
    Depends { name: "qCustomLib" }

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
            "/home/roman/raspi/sysroot/opt/vc/lib/"
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
