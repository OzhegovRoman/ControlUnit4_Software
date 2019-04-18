import qbs

StaticLibrary{
    name: "StarProtocol"
    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    cpp.cxxLanguageVersion: "c++14"

    Group{
        name: "Source"
        files: [
            "cstarprotocol.cpp",
            "cstarprotocol.h",
            "cstarprotocolpc.cpp",
            "cstarprotocolpc.h",
            "star_prc_commands.h",
            "star_prc_structs.h",
        ]
    }
}
