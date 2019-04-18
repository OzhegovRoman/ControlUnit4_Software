import qbs
import qbs.TextFile
import qbs.File

CppApplication{
    name: "cu-devcalibration"

    condition: project.isWindows

    consoleApplication: false

    // preparation RC_File
    property string description: "Calibration for Scontel's ControlUnits modules"
    property string internalName: "Calibrator"

    Depends {name: "RC_Prepare"}

    Rule {
        condition: project.isWindows
        multiplex: true
        alwaysRun: true
        Artifact {
            filePath: product.name+"_generated.rc"
            fileTags: "rc"
        }
        prepare: {
            var  cmd  =  new  JavaScriptCommand();
            cmd.description  =  "Processing  '"  +  product.name  +  "'";
            cmd.highlight  =  "codegen";
            cmd.sourceCode  =  function()  {
                var  file  =  new  TextFile(product.sourceDirectory+"/../../qbs/modules/RC_Prepare/simpleApp.rc.tmp");
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

    type: ["application"]

    cpp.cxxLanguageVersion: "c++17"

    cpp.includePaths: [
        "../../Libs/CuPlugins/",
        "../../Libs/StarProtocol/",
        "../../Libs/Agilent34401A/",
        "../../Libs/qCustomPlot/",
        "../../Libs/LineRegression/",
    ]

    Depends { name: "CuPlugins" }
    Depends { name: "Agilent34401A" }
    Depends { name: "qCustomPlot" }
    Depends { name: "LineRegression" }
    Depends { name: "qCustomLib" }

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "gui",
            "serialport",
            "network",
            "widgets",
            "printsupport"
        ]
    }

    cpp.libraryPaths: [
        "../../Libs/GPIB/",
        "../../Libs/VISA/"
    ]

    Group {
        name: "Headers"
        files: [
            "*.h",
        ]
    }

    Group {
        name: "Source"
        files: [
            "*.cpp",
        ]
    }

    Group {
        name: "Forms"
        files: [
            "*.ui",
        ]
    }

    // пока это не готово для кросс компиляции, поскольку непонятно, что делать с визой и gpib,
    // но я уже занес это все
    property stringList commonDefines: ["VERSION=\""+project.softwareVersion+"\""]
    Properties{
        condition: qbs.architecture.contains("arm");
        cpp.defines: commonDefines.concat("RASPBERRY_PI");
        cpp.linkerFlags:[
            "-z",
            "relro"
        ]
    }
    cpp.defines: commonDefines;

    Group {
        condition: project.isWindows
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: project.installDir
    }
}
