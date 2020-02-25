import qbs

StaticLibrary{

    name: "RaspPiMMap"

    Depends { name: "cpp"}

    condition: project.target === "raspberryPi"
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
