import qbs

Project {
    condition: project.SoftwareEnableList.contains("System") || buildAll
    references: [
        "TcpIpServer/TcpIpServer.qbs",
    ]
}
