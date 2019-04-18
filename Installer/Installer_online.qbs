import qbs
import qbs.File
import qbs.Process
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "Installer_online_x86"

    type: ["online_x86", "DataToFtpSended"]

    condition: project.onlineInstaller

    Depends { name: "Qt.core" }
    Depends { name: "PreInstaller" }
    Depends { name: "sendDataToFtp" }

    Rule {
        id: idInstaller_Offline
        inputsFromDependencies: ["preinstalled"]
        Artifact {
            filePath: "CU4Installer_online_win_x86.exe"
            fileTags: [
                "prepared_toSending",
                "online_x86"
            ]
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "make online Installer";
            cmd.silent = false;
            cmd.highlight = "compiler";
            cmd.sourceCode = function() {

                //запускаем изготовитель инсталятора

                //чистим место будущего репозитория
                File.remove(project.buildDirectory+"/install-root/repository");

                // Делаем новый репозиторий
                var repogenProcess = new Process();
                var exCode = repogenProcess.exec(project.QtIFPaths+"repogen.exe",
                                                 [
                                                     "-p", project.buildDirectory+"/install-root/packages",
                                                     project.buildDirectory+"/install-root/repository"
                                                 ]);
                if (exCode) console.error("Error at making Repository: "+output.filePath+". Error code: "+exCode);

                // отсылаем всю папку с репозиторием
                var resPath = project.buildDirectory+"/install-root/repository";
                resPath = resPath.replace(':','');
                resPath = "/cygdrive/"+resPath;
                var rsyncProcess = new Process();
                var exCode = rsyncProcess.exec(project.rsyncUtility,
                                               ["-avz", "-e", project.sshUtility + " -i "+project.privateKeyPath,
                                                "--chmod=Dugo=rwX,Fugo=rw,Fugo-x", resPath,
                                                project.ftpLogin+':'+project.ftpRootPath]);
                if (exCode) console.error("Error at sending repository. Error code: "+exCode);

                               // создаем инсталер
                var instProcess = new Process();
                exCode = instProcess.exec(project.QtIFPaths+"binarycreator.exe",
                                          [
                                              "--online-only",
                                              "-c", project.buildDirectory+"/install-root/config/config.xml",
                                              "-p", project.buildDirectory+"/install-root/packages",
                                              output.filePath
                                          ]
                                          );
                if (exCode) console.error("Error at making online installer: "+output.filePath+". Error code: "+exCode);
            }
            return cmd;
        }
    }
}
