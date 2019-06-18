import qbs
import qbs.File
import qbs.Process
import qbs.TextFile


QtApplication {
    condition: project.isWindows
    name: "cu-serverconf"
    consoleApplication: false
    cpp.cxxLanguageVersion: "c++14"

    // preparation RC_File
    Depends {name: "RC_Prepare"}
    RC_Prepare.outputFileName: name
    RC_Prepare.description: "Setting up Scontel's ControlUnit server"
    RC_Prepare.internalName: "ServerSettings"
    Group {
        name: "Resources_tmp"
        fileTags: "RC_TMPL"
        prefix: "../../qbs/modules/**/"
        files:[
            "*.rc.tmp"
        ]
    }

    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    Depends {name: "CuPlugins"}
    Depends {name: "qCustomLib"}

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "gui",
            "network",
            "widgets",
        ]
    }

    Group {
        name: "Source"
        files: [
            "*.h",
            "*.cpp",
            "*.ui"
        ]
    }
}
