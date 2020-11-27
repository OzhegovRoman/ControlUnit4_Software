import qbs

Project {
    condition: project.SoftwareEnableList.contains("Service") || buildAll
    references: [
        "Calibration/Calibration.qbs",
        "cu-devboot/cu-devboot.qbs",
        "cu-devmanager/cu-devmanager.qbs",
        "ServerSettings/ServerSettings.qbs",
        "cu-test/cu-test.qbs",
        "cu-GeneralSetup/cu-GeneralSetup.qbs",
    ]
}
