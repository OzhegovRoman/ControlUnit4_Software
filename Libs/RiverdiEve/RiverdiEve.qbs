import qbs

StaticLibrary{
    name: "RiverdiEve"

    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    cpp.cxxLanguageVersion: "c++14"
    condition: project.target === "raspberryPi"
    cpp.defines: [
        "LINUX_PLATFORM",
        "BUFFER_OPTIMIZATION"
    ]

    cpp.dynamicLibraries:[
        "wiringPi"
    ]

    cpp.includePaths:[
        "app_layer",
        "eve_layer",
        "host_layer"
    ]
    Group{
        name: "Source"
        prefix:"**/"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
