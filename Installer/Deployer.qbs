import qbs
import qbs.File
import qbs.Process
import qbs.ModUtils
import qbs.TextFile
import qbs.Environment

Product {
    name: "Deployer"
    type: "deployed"

    Depends {name: "Qt.core" }
    Depends {
        condition: project.buildService || project.buildAll
        name: "cu-devboot"
    }
    Depends {
        condition: project.buildService || project.buildAll
        name: "cu-devcalibration"
    }
    Depends {
        condition: project.buildService || project.buildAll
        name: "cu-serverconf"
    }
//    Depends {
//        condition: project.buildSystem || project.buildAll
//        name: "cu-tcpipserver"
//    }
    Depends {
        condition: project.buildUser || project.buildAll
        name: "cu-simpleapp"
    }
    Depends {
        condition: project.buildUser || project.buildAll
        name: "cu-simpleapp_qml"
    }
    Depends {
        condition: project.buildUser || project.buildAll
        name: "cu-measurer"
    }

    Rule {
        condition: (qbs.buildVariant != "debug") && project.isWindows && (!Qt.core.staticBuild) && project.makeInstaller
        id: DeployData
        inputsFromDependencies: "installable"
        multiplex: true
        alwaysRun: true
        Artifact{
            filePath: "deploy_log.txt"
            fileTags: "deployed"
        }
        prepare: {
            var cmd =  new JavaScriptCommand();
            cmd.description = "Deploy all files";
            cmd.highlight = "DEPLOY";

            cmd.sourceCode = function(){
                var resPath = project.buildDirectory+"/install-root/"+project.installDir;
                var winDeploy = new Process();
                var depFile = new TextFile(output.filePath, TextFile.WriteOnly);
                var ExCode = 0;
                var winDeployPath = Environment.getEnv("QTDIR")+"/bin/windeployqt";
                var qmlDir = Environment.getEnv("QTDIR")+"/qml";
                winDeployPath = winDeployPath.replace(/\\/g,"/");
                for (i = 0; i < inputs["installable"].length; i++){
                    var dInput = inputs["installable"][i];
                    depFile.writeLine(dInput.fileName+"->"+resPath);
                    ExCode = winDeploy.exec(winDeployPath,
                                            ["--release",
                                             "--no-translations",
                                             "--no-system-d3d-compiler",
                                             "--compiler-runtime",
                                             "--no-opengl-sw",
                                             //"--libdir", deployPath+"/Libs",
                                             "--qmldir", qmlDir,
                                             "--dir", resPath,
                                             dInput.filePath]);
                    if (ExCode)
                        console.warn("Error at deploy " + dInput.fileName+". ResPath: "+resPath+". Error code: "+ExCode);
                }
            }
            return cmd;
        }
    }
}
