import qbs

Project {
    condition: buildService || buildAll
    references: [
        "Bootloader/Bootloader.qbs",
        "Calibration/Calibration.qbs",
        "cu-devboot/cu-devboot.qbs",
        "cu-devmanager/cu-devmanager.qbs",
        "ServerSettings/ServerSettings.qbs",
        "cu-test/cu-test.qbs",
    ]
}
