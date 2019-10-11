#include "consolereader.h"
#include <stdio.h>

ConsoleReader::ConsoleReader()
{
#ifdef Q_OS_WIN
    m_notifier = new QWinEventNotifier(GetStdHandle(STD_INPUT_HANDLE));
    connect(m_notifier, &QWinEventNotifier::activated
        #else
    m_notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated
        #endif
            , this, &ConsoleReader::readCommand);

}

void ConsoleReader::run()
{
    std::cout << "Device manager (utility for addition, delete and saving preferences)" << std::endl;
    newCommand();
}

void ConsoleReader::readCommand()
{
    std::string line;
    std::getline(std::cin, line);
    QString command = QString(line.c_str());

    command = command.trimmed();
    command.replace(QRegExp("[ ]{2,}")," ");

    QStringList cmdList = command.split(" ");

    if (std::cin.eof() || ((cmdList[0] == "exit" || cmdList[0] == "quit") && cmdList.size() == 1)){
        emit quit();
        return;
    }

    if (cmdList[0] == "help" && cmdList.size() == 1){
        std::cout << "  Short description of usage commands:" << std::endl;
        // Временно удалю это, поскольку это приводит к более сложному поведению программы
        //            qDebug()<<"  interface ? - getting information about prefered interface. Possible output: [serial, tcpip];";
        //            qDebug()<<"  interface <type> - setting up prefered interface. Possible input: [serial, tcpip];";
        //            qDebug()<<"  port ? - getting information about prefered COM port;";
        //            qDebug()<<"  port <portName> - setting up information about prefered COM port;";
        //            qDebug()<<"  tcpip ? - getting prefered tcpip address;";
        //            qDebug()<<"  tcpip <address> - setting up prefered tcpip address;";
        std::cout << "  devlist ? (or devlist? or devlist or list or list?)- request to get connected device list;" << std::endl;
        std::cout << "  add <device address> - add device with address to device list;" << std::endl;
        std::cout << "  delete <device address> - delete device with address from device list (This command available only with serial interface mode);" << std::endl;
        std::cout << "  clear - delete all devices from device list (This command available only with serial interface mode);" << std::endl;
        std::cout << "  save - save information about connected devices to the system;" << std::endl;
        std::cout << "  desc (or description) <device address> - get full description of device;" << std::endl;
        std::cout << "  exit - quit from this application;" << std::endl;
        std::cout << "  quit - quit from this application;" << std::endl;
        std::cout << "  help - this command." << std::endl;
        newCommand();
        return;
    }

    if (cmdList[0] == "add" && cmdList.size() == 2){
        bool ok;
        int tmp = cmdList[1].toInt(&ok);
        if (ok){
            emit add(tmp);
            return;
        }
    }

    if ((cmdList[0] == "desc" || cmdList[0] == "description") && cmdList.size() == 2){
        bool ok;
        int tmp = cmdList[1].toInt(&ok);
        if (ok){
            emit description(tmp);
            return;
        }
    }

    if ((cmdList[0] == "devlist?" || (cmdList[0] == "devlist")) && cmdList.size() == 1){
        emit list();
        return;
    }
    if (cmdList[0] == "devlist" && cmdList.size() == 2)
        if (cmdList[1] == "?"){
            emit list();
            return;
        }
    if ((cmdList[0] == "list?" || (cmdList[0] == "list")) && cmdList.size() == 1){
        emit list();
        return;
    }
    if (cmdList[0] == "list" && cmdList.size() == 2)
        if (cmdList[1] == "?"){
            emit list();
            return;
        }

    if (cmdList[0] == "delete" && cmdList.size() == 2){
        bool ok;
        int tmp = cmdList[1].toInt(&ok);
        if (ok){
            emit remove(tmp);
            return;
        }
    }

    if (command == "clear"){
        emit clear();
        return;
    }

    if (command == "save"){
        emit save();
        return;
    }

    std::cout << "Command UNDEFINED. Please use \"help\" command for getting detailing description or \"exit\", \"quit\" for exit."<<std::endl;
    newCommand();
}

void ConsoleReader::newCommand()
{
    std::cout << "> " << std::flush;
}

void ConsoleReader::write(QString str)
{
    std::cout<<str.toLatin1().data()<<std::endl;
}
