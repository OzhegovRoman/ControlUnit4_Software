import qbs

Project {
    //minimumQbsVersion: "1.9.0"
    property string softwareVersion: "1.1.3"
    property string company: "Scontel"
    property string productName: "Scontel's ControlUnit software"

    // Installation preferences
    property bool makeInstaller:    false
    property bool onlineInstaller:  false
    property bool offlineInstaller: false
    property string baseInstallPackegeName: "com.scontel.controlunit"
    property string QtIFPaths: "c:/Qt/Tools/QtInstallerFramework/3.0/bin/"
    property string installDir: "packages/"+baseInstallPackegeName+"/data"
    // Настройки сборки
    property bool buildAll:         false
    property bool buildLibraries:   true
    property bool buildService:     true
    property bool buildSystem:      false
    property bool buildUser:        true

    property bool isWindows: qbs.targetOS.contains("windows");
    property bool isAndroid: qbs.targetPlatform.contains("android")

    //some code for deployment
    property string ftpLogin: "ozhegov@rplab.ru";
    property string privateKeyPath: "/home/Roman/.ssh/rplab_sftp";
    property string ftpRootPath: "public_html/ControlUnit4/";
    property string rsyncUtility: "e:/DevTools/cwRsync_5.5.0_x86_Free/bin/rsync.exe"
    property string sshUtility: "e:/DevTools/cwRsync_5.5.0_x86_Free/bin/ssh.exe"

    property string buildDirectory: "../Build"

    qbsSearchPaths: "qbs"

    references: [
        "Installer/Installer.qbs",
        "Libs/Libs.qbs",
        "Service/Service.qbs",
        "System/System.qbs",
        "User/User.qbs",
    ]
}

/*
    История версий.
    0.0.1 - начальная версия программного обеспечения
    0.0.2 - переход на Qt5.9.0 и Qbs1.9.0
    0.0.3 - переход на Qt5.10.1
            устранены ряд проблем (CU-17, CU-24)
            добавлена утилита Service/Calibration.
            утилиты Service/CU4SDM0V1_Calibration и Service/CU4TDM0V1_Calibration потеряли актуальность
            и в следующей версии будут удалены
    0.0.4 - кумулятивный апдейт.
            Добавлены пакеты для debian.
            Убрано SimpleApplicationTcpIp.
            SimpleApplication сделана более универсальным приложением
    0.1.0 - Повышена версия софта. Переведено в состояние "почти готово"
            QeMeasurer:
                - значения тока в микроамперах
    0.1.1 - Cu4SDxx_Calibration:
                - утилина удалена из проекта
            cu-simpleapp:
                - изменения в стартовом диалоге в утилите cu-smpeapp
                - виджете для работы с однофотонным детектором добавил кнопку включения
                режима автоподдержания рабочего тока.
            Изменения в API:
                - удален класс CuRaspRs485IoInterface. Сейчас необходимо в самом проекте прописать в дефайнах RASPBERYY_PI
                для конкретизации платформы
                - расширены возможности cuDeviceParam.getValueSequence и cuDeviceParam_settable.setValueSequence.
                теперь можно установить количество попыток.
                - исправлена проблема в типе данных CU4SDM0_Status_t, покоторой для Stm32 эта структура занимает 1 байт, для Linux систем - 4 байта.
                Все приведено к длине в 1 байт.
    1.0.0 - собираю нормальный инсталлер для всех утилит
    1.0.1 - Cu4TDxx_Calibration:
                - утилита удалена из проекта
            cu-devcalibration:
                - добавлена функциональность для калибровки драйверов температуры
    1.0.2 - cu-devcalibration:
                - теперь запускается без Visa32, установленной на компьютере и позволяет менять,
                обновлять калибровки
                - (Исправлено) При нажатии на крест закрытия окна калибровки не останавливался процесс калибровки
    1.0.3 - переходим на Qt5.11.1
    1.0.4 - незначительно доработан QEMeasurer. Задачи CU-92, CU-99, CU-101
    1.1.0 - Решенные проблемы:
            CU-106 - добавлен в комплект поставки для windows программа SimpleApp QML
            CU-110 - добавлено описание иполняемых файлов, вызываемое через нажатие правой клавиши и свойства файла
            CU-109 - версия библиотек qt для raspberry обновлена до Qt5.11.2
            CU-59  - добавлена утилита cu-devmngr
            CU-111 - решена проблема с cu-displaycalibration, когда после завершения калибровки программа зависала
            CU-100 - внесены изменения в утилиту cu-embeddeddisplay. Теперь после простоя в 20 минут программа переходит в спящий режим.
            CU-56  - решена проблема, возникающая при работе 2х и более устройств с температурным модулем. При отключении программ происходило выключение модуля.
                    сейчас программа каждый раз проверяет статус подключения кабеля к термометру. При необходимости кабель подключается самой программой
            CU-32  - решена проблема с работой cu-tcpipserver, ранее формат значений, посылаемых серверу, типа 1e-5 приводил к ошибкам.
            CU-20  - проблема отменена так как в будущем планируется уход от стандартных устройств. Для калибровки будет использован самопальный стенд
            СU-98  - к программам для Windows, имеющим графический интерфейс, добавлены иконки
            СU-107 - проблема перенесена на решение в версии 1.2.0, поскольку требуется проверка пользователями: какая утилита лучше: serversettongs или cu-devmngr
    1.1.2 - Решенные проблемы:
            CU-124 - Добавление поля "All Channels" для программы cu-simpleapp
            CU-123 - Исправление ошибки в cu-simpleapp
    1.1.3 - Рефакторинг кода, разделение на открытую и закрытую часть

    //TODO: сделать отдельное правило для всех raspberryPi приложений для правильного деплоя
    //TODO: проверить код для распберри
    //TODO: Проверить инсталятор

*/
