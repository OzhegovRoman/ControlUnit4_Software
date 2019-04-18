import qbs
import qbs.File
import qbs.Process
import qbs.ModUtils
import qbs.TextFile

Project {
    condition: (qbs.buildVariant != "debug") && project.isWindows && project.makeInstaller
    references: [
        "Deployer.qbs",
        "PreInstaller.qbs",
        "Installer_offline.qbs",
        "Installer_online.qbs",
    ]
}
