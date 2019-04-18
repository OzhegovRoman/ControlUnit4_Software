import qbs
import qbs.File
import qbs.Process
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "Installer_offline_x86"

    type: ["offline_x86", "DataToFtpSended"]

    condition: project.offlineInstaller

    Depends { name: "Qt.core" }
    Depends { name: "PreInstaller" }
    Depends { name: "sendDataToFtp" }

    Rule {
        id: idInstaller_Offline
        inputsFromDependencies: ["preinstalled"]
        Artifact {
            filePath: "CU4Installer_offline_win_x86_"+project.softwareVersion+".exe"
            fileTags: [
                "prepared_toSending",
                "offline_x86"
            ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "make offline Installer";
            cmd.silent = false;
            cmd.highlight = "compiler";
            cmd.sourceCode = function() {
                //запускаем изготовитель инсталятора
                var instProcess = new Process();
                var exCode = instProcess.exec(project.QtIFPaths+"binarycreator.exe",
                                              [
                                                  "--offline-only",
                                                  "-c", project.buildDirectory+"/install-root/config/config.xml",
                                                  "-p", project.buildDirectory+"/install-root/packages",
                                                  output.filePath
                                              ]
                                              );
                if (exCode) console.error("Error at making Installer: "+output.filePath);
            };
            return cmd;
        }
    }
}
