import qbs

StaticLibrary{
    name: "Agilent34401A"
    // собираем только под windows
    condition: (project.target === "win_x86") ||
               (project.target === "win_x86_64")
    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    Properties{
        condition: project.target === "win_x86"
        cpp.defines: "WIN32"
        cpp.includePaths: [
            "../GPIB/",
            "../VISA/"
        ]

        cpp.libraryPaths: [
            "../GPIB/",
        ]

        cpp.staticLibraries: "gpib"
    }

    Properties{
        condition: project.target === "win_x86_64"
        cpp.defines: "WIN64"
        cpp.includePaths: [
            "../VISA/"
        ]
    }
    Group{
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
        excludeFiles: project.target === "win_x86_64" ? "*gpib*":"---"
    }
}
