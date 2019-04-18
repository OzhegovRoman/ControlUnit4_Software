import qbs

StaticLibrary{
    name: "LineRegression"
    Depends { name: "cpp"}
    Depends { name: "Qt.core" }

    cpp.cxxLanguageVersion: "c++14"

    Depends {
        name: "Android.ndk"
        condition: project.isAndroid
    }

    Properties {
        condition: Android.ndk.present
         Android.ndk.appStl: "gnustl_shared"
    }

    Group{
        name: "Source"
        files: [
            "*.cpp",
            "*.h",
        ]
    }
}
