import qbs

QtApplication {
    condition: project.isWindows
    consoleApplication: false
    name: "cu-bootloader"

    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    Depends { name: "CuPlugins" }
    Depends {
        name: "Qt";
        submodules: [
            "core",
            "gui",
            "serialport",
            "widgets"
        ]
    }

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
    Group {
        name: "Forms"
        files: [
            "*.ui",
        ]
    }
}
