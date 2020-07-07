import qbs

Project {
    condition:  project.SoftwareEnableList.contains("Libs") || buildAll
    references: [
        "StarProtocol/StarProtocol.qbs",
        "CuPlugins/CuPlugins.qbs",
        "Agilent34401A/Agilent34401A.qbs",
        "LineRegression/LineRegression.qbs",
        "qCustomPlot/qCustomPlot.qbs",
        "RaspPiMMap/RaspPiMMap.qbs",
        "qCustomLib/qCustomLib.qbs",
        "RiverdiEve/RiverdiEve.qbs"
    ]
}
