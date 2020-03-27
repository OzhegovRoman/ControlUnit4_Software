import qbs

StaticLibrary{
    name: "FT801"

    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    cpp.cxxLanguageVersion: "c++14"

    Group{
        name: "Source"
        prefix:"**/"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
