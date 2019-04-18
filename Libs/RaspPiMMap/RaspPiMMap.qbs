import qbs

StaticLibrary{

    name: "RaspPiMMap"

    Depends { name: "cpp"}

    condition: !(project.isWindows || project.isAndroid)
    cpp.cxxLanguageVersion: "c++14"
    Depends { name: "Qt.core" }
    Group{
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
