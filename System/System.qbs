import qbs

Project {
    condition: buildSystem || buildAll
    references: [
        "TcpIpServer/TcpIpServer.qbs",
    ]
}
