import qbs
import qbs.TextFile
import qbs.File

CppApplication{
    condition: project.isWindows
    name: "cu-devcalibration"
    consoleApplication: false

    // preparation RC_File
    Depends {name: "RC_Prepare"}
    RC_Prepare.outputFileName:  name
    RC_Prepare.description:     "Calibration for Scontel's ControlUnits modules"
    RC_Prepare.internalName:    "Calibrator"
    Group {
        name: "Resources_tmp"
        fileTags: "RC_TMPL"
        prefix: "../../qbs/modules/**/"
        files:[
            "*.rc.tmp"
        ]
    }

    cpp.cxxLanguageVersion: "c++14"
    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/Agilent34401A/",
        "../../Libs/qCustomPlot/",
        "../../Libs/LineRegression/",
    ]

    Depends { name: "CuPlugins" }
    Depends { name: "Agilent34401A" }
    Depends { name: "qCustomPlot" }
    Depends { name: "LineRegression" }
    Depends { name: "qCustomLib" }

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "gui",
            "serialport",
            "network",
            "widgets",
            "printsupport"
        ]
    }

    cpp.libraryPaths: [
        "../../Libs/GPIB/",
        "../../Libs/VISA/"
    ]

    Group {
        name: "Headers"
        files: [
            "*.h",
        ]
    }

    Group {
        name: "Source"
        files: [
            "*.cpp",
        ]
    }

    Group {
        name: "Forms"
        files: [
            "*.ui",
        ]
    }

    // пока это не готово для кросс компиляции, поскольку непонятно, что делать с визой и gpib,
    // но я уже занес это все
    property stringList commonDefines: ["VERSION=\""+project.softwareVersion+"\""]
    Properties{
        condition: qbs.architecture.contains("arm");
        cpp.defines: commonDefines.concat("RASPBERRY_PI");
        cpp.linkerFlags:[
            "-z",
            "relro"
        ]
    }
    cpp.defines: commonDefines;

}
