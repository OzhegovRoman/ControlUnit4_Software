import qbs

Project {
    condition: buildService || buildAll
    references: [
        "Bootloader/Bootloader.qbs",
//        "cu-devboot/cu-devboot.qbs",
//        "cu-devmanager/cu-devmanager.qbs",
//        "ServerSettings/ServerSettings.qbs",
//        "Calibration/Calibration.qbs",
    ]
}
