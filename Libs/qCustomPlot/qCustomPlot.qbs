import qbs

StaticLibrary{
    name: "qCustomPlot"

    Depends { name: "cpp"}

    cpp.cxxLanguageVersion: "c++14"

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "widgets",
            "printsupport"
        ]
    }

    Group{
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
