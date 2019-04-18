import qbs
import qbs.File
import qbs.Process
import qbs.ModUtils
import qbs.TextFile

Product {
    name: "PreInstaller"
    type: "preinstalled"

    Depends {name: "Deployer"}
    Depends {name: "Qt.core"}

    files: "package.xml"

    Rule {
        condition: (qbs.buildVariant != "debug") && project.isWindows && (!Qt.core.staticBuild) && project.makeInstaller
        id: preInstallData
        inputsFromDependencies: "deployed"
        Artifact{
            filePath: "config.xml"
            fileTags: "preinstalled"
        }
        prepare: {
            var cmd =  new JavaScriptCommand();
            cmd.description = "prepare install packages";
            cmd.highlight = "install";

            cmd.sourceCode = function(){
                //шаг первый переносим полностью каталог с установленными файлами
                //создаем конфиг файл.
                var configFile = new TextFile(output.filePath, TextFile.WriteOnly);
                configFile.writeLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
                configFile.writeLine("<Installer>");
                configFile.writeLine("    <Name>Scontel's control unit software</Name>");
                configFile.writeLine("    <Version>"+project.softwareVersion+"</Version>");
                configFile.writeLine("    <Title>Scontel's control unit software Installer</Title>");
                configFile.writeLine("    <Publisher>Scontel</Publisher>");
                configFile.writeLine("    <ProductUrl>http://rplab.ru/~ozhegov</ProductUrl>");
                configFile.writeLine("    <StartMenuDir>ControlUnit</StartMenuDir>");
                configFile.writeLine("    <TargetDir>@ApplicationsDir@/Scontel</TargetDir>");
                configFile.writeLine("    <MaintenanceToolName>Updater</MaintenanceToolName>");
                configFile.writeLine("    <InstallActionColumnVisible>true</InstallActionColumnVisible>");
                configFile.writeLine("    <RemoteRepositories>");
                configFile.writeLine("         <Repository>");
                configFile.writeLine("                 <Url>http://rplab.ru/~ozhegov/ControlUnit4/repository</Url>");
                configFile.writeLine("                 <Enabled>1</Enabled>");
                configFile.writeLine("                 <DisplayName>Example repository</DisplayName>");
                configFile.writeLine("         </Repository>");
                configFile.writeLine("    </RemoteRepositories>");
                configFile.writeLine("</Installer>");
                configFile.close();
                File.copy(output.filePath, project.buildDirectory+"/install-root/config/"+output.fileName);

                // Далее готовим и пишем basePackage
                var now = new Date();
                var date = now.getDate();
                if (date<10) date = "0"+date;
                var month = now.getMonth()+1;
                if (month<10) month = "0"+month;
                var outDate = now.getFullYear()+'-'+month+'-'+date;

                //читаем последюю версию package
                var packFile = new TextFile(project.path+"/package.xml", TextFile.ReadOnly);
                var buildVersion = 0;
                while (!packFile.atEof()){
                    var str = packFile.readLine();
                    if (str.indexOf("<Version>", 0) >= 0){
                        //удаляем лишние пробелы
                        str = str.replace(/\s/g,'');
                        //и теги Version
                        str = str.replace(/<Version>/,'');
                        str = str.replace(/<\/Version>/,'');
                        // сверяем softwareVersion
                        if (!str.search(project.softwareVersion)){
                            buildVersion = str.split('-')[1];
                        }
                    }
                }
                buildVersion++;
                packFile.close();

                packFile = new TextFile(project.path+"/package.xml", TextFile.WriteOnly);
                packFile.writeLine("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
                packFile.writeLine("<Package>");
                packFile.writeLine("    <DisplayName>Common apps</DisplayName>");
                packFile.writeLine("    <Description>Common components for Scontel's control unit.</Description>");
                packFile.writeLine("    <Version>"+project.softwareVersion+"-"+buildVersion+"</Version>");
                packFile.writeLine("    <ReleaseDate>"+outDate+"</ReleaseDate>");
                packFile.writeLine("    <ForcedInstallation>true</ForcedInstallation>");
                packFile.writeLine("    <Script>installscript.qs</Script>");
                packFile.writeLine("</Package>");
                packFile.close();

                File.copy(project.path+"/package.xml", project.buildDirectory+"/install-root/"+project.installDir+"/../meta/package.xml");

                // записываем installScript
                var installScriptFile = new TextFile( project.buildDirectory+"/install-root/"+project.installDir+"/../meta/installscript.qs", TextFile.WriteOnly);
                installScriptFile.writeLine("function Component()");
                installScriptFile.writeLine("{");
                installScriptFile.writeLine("}");
                installScriptFile.writeLine("");
                installScriptFile.writeLine("Component.prototype.createOperations = function()");
                installScriptFile.writeLine("{");
                installScriptFile.writeLine("    // call default implementation to actually install README.txt!");
                installScriptFile.writeLine("    component.createOperations();");
                installScriptFile.writeLine("    var maintenanceToolPath;");
                installScriptFile.writeLine("    var Path4AllUsers;");
                installScriptFile.writeLine("");
                installScriptFile.writeLine("    if (systemInfo.productType == \"windows\") {");
                installScriptFile.writeLine("        component.addOperation(\"CreateShortcut\", \"@TargetDir@/cu-simpleapp.exe\", \"@StartMenuDir@/User/Simple Application.lnk\");");
                installScriptFile.writeLine("        component.addOperation(\"CreateShortcut\", \"@TargetDir@/cu-simpleapp_qml.exe\", \"@StartMenuDir@/User/Simple Application with QML.lnk\");");
                installScriptFile.writeLine("        component.addOperation(\"CreateShortcut\", \"@TargetDir@/cu-measurer.exe\", \"@StartMenuDir@/User/Measurer.lnk\");");
                installScriptFile.writeLine("        component.addOperation(\"CreateShortcut\", \"@TargetDir@/cu-devcalibration.exe\", \"@StartMenuDir@/Service/Device Calibration.lnk\");");
                installScriptFile.writeLine("        component.addOperation(\"CreateShortcut\", \"@TargetDir@/cu-serverconf.exe\", \"@StartMenuDir@/Service/Server congfigurator.lnk\");");
                installScriptFile.writeLine("        component.addOperation(\"CreateShortcut\", \"@TargetDir@/\"+installer.value(\"MaintenanceToolName\")+\".exe\", \"@StartMenuDir@/Update&Uninstal.lnk\");");
                installScriptFile.writeLine("    }");
                installScriptFile.writeLine("}");
            }
            return cmd;
        }
    }
}
