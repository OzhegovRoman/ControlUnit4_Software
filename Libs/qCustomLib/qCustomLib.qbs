import qbs

StaticLibrary{
    name: "qCustomLib"

    Depends { name: "cpp"}

    cpp.cxxLanguageVersion: "c++14"

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "network"
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
