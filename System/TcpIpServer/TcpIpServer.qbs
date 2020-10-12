import qbs

CppApplication{
    condition: project.target !== "android"// только под RaspberryPi || Windows
    name: "cu-tcpipserver"
    consoleApplication: true

    cpp.cxxLanguageVersion: "c++14"

    Depends { //Fake Devices in AdditionalLibs with NonFreeSoftware
        name: "FakeDevices"
        required: false
    }

    property stringList commonDefines: [
        "VERSION=\""+project.softwareVersion+"\"",
        "DEBUG_LOG"
    ]
    Properties {
        condition: FakeDevices.present
        cpp.defines: commonDefines.concat("FAKE_DEVICES");
        cpp.includePaths: [
            "../../Libs/CuPlugins/",
            "../../Libs/StarProtocol/",
            "../../../Libs/Devices/",
            "../../../Libs/FakeDevicesLib/",
        ]
    }
    cpp.defines: commonDefines;
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

    Group {
        name: "Source"
        prefix: "**/"
        files: [
            "*.cpp",
            "*.h",
            "*.ui",
        ]
    }

    Group {
        condition: project.target === "raspberryPi"
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installPrefix: "/home/pi"
        qbs.installDir: "Software"
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
            "/home/roman/raspi/sysroot/opt/vc/lib/"
        ]
    }
}
