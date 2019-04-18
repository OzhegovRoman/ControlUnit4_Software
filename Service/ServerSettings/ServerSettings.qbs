import qbs
import qbs.File
import qbs.Process
import qbs.TextFile


QtApplication {
    property bool isStatic: Qt.core.staticBuild;

    name: "cu-serverconf"
    cpp.cxxLanguageVersion: "c++11"

    // предназначено только для Windows (пока)
    condition: qbs.targetOS.contains("windows")

    type: ["application"]

    // preparation RC_File
    property string description: "Setting up Scontel's ControlUnit server"
    property string internalName: "ServerSettings"
//    Depends { name: "RC_Prepare"}
//    Group {
//        name: "Resources_tmp"
//        fileTags: "RC_TMPL"
//        prefix: "../../modules/**/"
//        files:[
//            "*.rc.tmp"
//        ]
//    }


    Rule {
        condition: project.isWindows
//        inputs: "RC_TMPL"
        multiplex: true
        alwaysRun: true
        Artifact {
            filePath: product.name+"_generated.rc"
            fileTags: "rc"
        }
        prepare: {
            var  cmd  =  new  JavaScriptCommand();
            cmd.description  =  "Generate resouces for '"  +  product.name  +  "'";
            cmd.highlight  =  "codegen";
            cmd.sourceCode  =  function()  {
                var  file  =  new  TextFile(product.sourceDirectory+"/../../qbs/modules/RC_Prepare/simpleApp.rc.tmp");
//                var  file  =  new  TextFile(input.filePath);
                var  content  =  file.readAll();
                file.close();

                content  =  content.replace(/%MainIconPath%/g, product.sourceDirectory+"/MainIcon.ico");
                content  =  content.replace(/%FILEVERSION%/g, project.softwareVersion.replace(/\./g,","));
                content  =  content.replace(/%PRODUCTVERSION%/g, project.softwareVersion.replace(/\./g,","));
                content  =  content.replace(/%FILEDESCRIPTION%/g, product.description);
                content  =  content.replace(/%FILEVERSION_STR%/g, project.softwareVersion);
                content  =  content.replace(/%PRODUCTVERSION_STR%/g, project.softwareVersion);
                content  =  content.replace(/%COMPANY_STR%/g, project.company);
                content  =  content.replace(/%LEGALCOPYRIGHT_STR%/g, "Copyright (C) "+ (new (Date)).getFullYear()+", "+project.company);
                content  =  content.replace(/%ORIGINALFILENAME_STR%/g, product.name+".exe");
                content  =  content.replace(/%PRODUCTNAME_STR%/g, project.productName);
                content  =  content.replace(/%INTERNALNAME_STR%/g, product.internalName);

                file  =  new  TextFile(output.filePath,  TextFile.WriteOnly);
                file.truncate();
                file.write(content);
                file.close();
            }
            return  cmd;
        }
    }



    consoleApplication: false
    cpp.includePaths: [
        "../../Libs/CuPlugins/"
    ]

    Depends {name: "CuPlugins"}
    Depends {name: "qCustomLib"}

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "gui",
            "network",
            "widgets",
        ]
    }

    // зависимости для статической сборки
    Depends {
        name: "Qt.qwindows";
        condition: isStatic && project.isWindows
    }
    // Правила линковки для статичекой сборки под Windows
    Properties {
        condition: isStatic && project.isWindows
        cpp.driverFlags: [
            "-static",
            "-static-libgcc",
        ]
    }

    Group {
        name: "Source"
        files: [
            "*.h",
            "*.cpp",
            "*.ui"
        ]
    }
    Group {
        condition: project.isWindows
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: project.installDir
    }

}
