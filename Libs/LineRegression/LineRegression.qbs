import qbs

StaticLibrary{
    name: "LineRegression"

    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    cpp.cxxLanguageVersion: "c++14"

    Group{
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
