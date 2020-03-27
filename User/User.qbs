import qbs

Project {
    condition: project.SoftwareEnableList.contains("User") || buildAll
    references: [
        "QeMeasurer/QeMeasurer.qbs",
        "SimpleApplication/SimpleApplication.qbs",
        "embeddedDisplay/EmbeddedDisplay.qbs",
        "embeddedDisplay_V2/EmbeddedDisplay_V2.qbs",
        "displayCalibration/displayCalibration.qbs",
        "SimpleApp_QML/SimpleApp_QML.qbs"
    ]
}
