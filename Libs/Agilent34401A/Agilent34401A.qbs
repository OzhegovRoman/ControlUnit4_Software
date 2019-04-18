import qbs

StaticLibrary{
    name: "Agilent34401A"
    condition: isWindows
    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    cpp.defines: [
        "WIN32"
    ]

    cpp.includePaths: [
        "../GPIB/",
        "../VISA/"
    ]

    cpp.libraryPaths: [
        "../GPIB/",
    ]

    cpp.staticLibraries: "gpib"

    Group{
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
