import qbs

Project {
    condition: buildUser || buildAll
    references: [
        "QeMeasurer/QeMeasurer.qbs",
        "SimpleApplication/SimpleApplication.qbs",
        "embeddedDisplay/EmbeddedDisplay.qbs",
        "displayCalibration/displayCalibration.qbs",
        "SimpleApp_QML/SimpleApp_QML.qbs"
    ]
}
