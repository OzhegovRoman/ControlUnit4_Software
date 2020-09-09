import qbs
import qbs.TextFile
import qbs.File

CppApplication{
    files: [
        "resources.qrc",
    ]
    condition: (project.target === "win_x86") ||
               (project.target === "win_x86_64")
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

    Group {
        name: "Calibrator"
        prefix: "Calibrator/**/"
        files: [
            "*.cpp",
            "*.h",
        ]
    }

    cpp.defines: ["VERSION=\""+project.softwareVersion+"\""];
}
