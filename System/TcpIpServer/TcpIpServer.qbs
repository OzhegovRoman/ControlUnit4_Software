import qbs

CppApplication{
    condition: !(project.isAndroid) // только под RaspberryPi || Windows
    name: "cu-tcpipserver"
    consoleApplication: true

    cpp.cxxLanguageVersion: "c++14"

    Depends { //Fake Devices in AdditionalLibs with NonFreeSoftware
        name: "FakeDevices"
        required: false
    }

    property stringList commonDefines: ["VERSION=\""+project.softwareVersion+"\""]
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
        qbs.installPrefix: "/home/pi"
        qbs.installDir: "Software"
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
