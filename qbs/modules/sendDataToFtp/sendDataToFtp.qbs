import qbs
import qbs.File
import qbs.Process
import qbs.ModUtils
import qbs.TextFile

Module {
    // деплой релизной сборки
    Rule {
        id: idSender
        inputs: ["prepared_toSending"]
        Artifact {
            fileTags: "DataToFtpSended"
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "send "+ input.fileName+" to "+project.ftpLogin+":"+project.ftpRootPath;
            cmd.silent = false;
            cmd.highlight = "compiler";
            cmd.sourceCode = function() {
                //размещаем файл на сервере
                var resPath = input.filePath;
                resPath = resPath.replace(':','');
                resPath = "/cygdrive/"+resPath;


                var rsyncProcess = new Process();
                var exCode = rsyncProcess.exec(project.rsyncUtility,
                                               ["-avz", "-e",  project.sshUtility+" -i "+project.privateKeyPath,
                                                "--chmod=Dugo=rwX,Fugo=rw,Fugo-x", resPath,
                                                project.ftpLogin+':'+project.ftpRootPath]);
                if (exCode) console.error("Error at deployment "+input.fileName);
            };
            return cmd;
        }
    }
}
