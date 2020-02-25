import qbs

StaticLibrary{
    name: "Agilent34401A"
    // собираем только под windows
    condition: (project.target === "win_x86") ||
               (project.target === "win_x86_64")
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
