#include "DBConfig.h"

static const QString ACCESSLEVEL_ADMINISTRATION_PASSWORD = "21232f297a57a5a743894a0e4a801fc3";
static const QString ACCESSLEVEL_GUEST_PASSWORD = "d41d8cd98f00b204e9800998ecf8427e";

static const QString configDBFolder = "DBConfig";
static const QString configDBFileName = "KSUConfig.db";
static const QString parametrsTemplatesFolder = "ParamTemplates";
static const QString ksuParametrsFolder = "KsuParametrs";
static const QString sapParametrsFolder = "SapParametrs";
static const QString systemAliasesFolder = "SystemAliases";

static const QString ksuManual0x20TestingPrestsFolder = "Ksu0x20Presets";
static const QString ksuManualSelectiveTestingPrestsFolder = "KsuSelectivePresets";

static const QString sapParamsConstString = "Параметры САП: ";
static const QString defaultTemplate = "(Шаблон по умолчанию)";
static const QString ksuArgus = "КСУ - Аргус";
static const QString ksuBmk5Bmk8 = "КСУ - БМК5/БМК8";
static const QString ksuBmk7 = "КСУ - БМК7";
static const QString ksuBorezVD = "КСУ - Борец ВД";
static const QString ksuElecton08 = "КСУ - Электон 08";
static const QString ksuElecton091 = "КСУ - Электон 09.1";
static const QString ksuElecton10C1 = "КСУ - Электон 10С1";
static const QString ksuIrz500 = "КСУ - ИРЗ 500";
static const QString ksuKaskad2200 = "КСУ - Каскад 2-200";
static const QString ksuKaskadNT = "КСУ - Каскад НТ";
static const QString ksuUKSU = "КСУ - УКСУ";
static const QString sapsu = "(Шаблон по умолчанию для САП СУ)";
static const QString mechanic = "(Шаблон по умолчанию для Механик)";

DBConfig *DBConfig::instance()
{
    static DBConfig singletone;
    return &singletone;
}

QString DBConfig::dbFolderPath()
{
    return QCoreApplication::applicationDirPath() + QDir::separator() + configDBFolder;
}

QString DBConfig::parametrsTemplatesFolderPath()
{
    return dbFolderPath() + QDir::separator() + parametrsTemplatesFolder;
}

QString DBConfig::ksuParametrsFolderPath()
{
    return dbFolderPath() + QDir::separator() + ksuParametrsFolder;
}

QString DBConfig::sapParametrsFolderPath()
{
    return dbFolderPath() + QDir::separator() + sapParametrsFolder;
}

QString DBConfig::systemAliasesPath()
{
    return dbFolderPath() + QDir::separator() + systemAliasesFolder;
}

QString DBConfig::ksuManual0x20TestingPrestsPath()
{
    return dbFolderPath() + QDir::separator() + ksuManual0x20TestingPrestsFolder + QDir::separator();
}

QString DBConfig::ksuManualSelectiveTestingPresetsPath()
{
    return dbFolderPath() + QDir::separator() + ksuManualSelectiveTestingPrestsFolder + QDir::separator();
}

DBConfig::DBConfig() : QObject()
{
    //    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    //    QDir dir;
    //    dir.mkpath(dirPath);

    //    QString dirP = dir.path();
    //    dir.setPath(dirPath);
    QDir dir(dbFolderPath());
    dir.mkpath(dir.path());
    QString path = dir.absoluteFilePath(configDBFileName);

    _configDB = QSqlDatabase::addDatabase("QSQLITE");
    _configDB.setDatabaseName(path);
    _configDB.open();

    setupDatabase();
}

DBConfig::~DBConfig()
{
    _configDB.close();
}

void DBConfig::setupDatabase()
{
    QSqlQuery query(_configDB);
    query.exec("PRAGMA auto_vacuum = 2");
    query.exec("PRAGMA foreign_keys = 1");

    /// AccessLevels - уровни доступа пользователей
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `AccessLevels` "
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, "
               "`name`	TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE, "
               "`access` INTEGER NOT NULL DEFAULT 0 CHECK(`access` BETWEEN 0 AND 999) "
               ");");

    query.exec(QString("INSERT OR IGNORE INTO `AccessLevels`(id, name, access) VALUES(1, 'Администратор', %1);")
               .arg(ACCESSLEVEL_ADMIN));
    query.exec(QString("INSERT OR IGNORE INTO `AccessLevels`(id, name, access) VALUES(2, 'Инженер', %1);")
               .arg(ACCESSLEVEL_ENGINEER));
    query.exec(QString("INSERT OR IGNORE INTO `AccessLevels`(id, name, access) VALUES(3, 'Оператор', %1);")
               .arg(ACCESSLEVEL_OPERATOR));
    query.exec(QString("INSERT OR IGNORE INTO `AccessLevels`(id, name, access) VALUES(4, 'Гость', %1);")
               .arg(ACCESSLEVEL_USER));

    // #################################################################################

    /// Users - пользователи
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `Users`"
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE,"
               "`accessId`	INTEGER NOT NULL,"
               "`password` TEXT NOT NULL,"
               "FOREIGN KEY(`accessId`) REFERENCES `AccessLevels`(`id`) ON UPDATE CASCADE ON DELETE RESTRICT"
               ");");

    query.exec(QString("INSERT OR IGNORE INTO `Users`(name, accessId, password) VALUES('Администратор', 1, '%1');")
               .arg(ACCESSLEVEL_ADMINISTRATION_PASSWORD));

    query.exec(QString("INSERT OR IGNORE INTO `Users`(name, accessId, password) VALUES('Гость', 4, '%1');")
               .arg(ACCESSLEVEL_GUEST_PASSWORD));

    //##################################################################################

    // ParametrAliases - алиасы
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `ParametrAliases`" ///алиасы
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`alias`	TEXT NOT NULL CHECK(trim(alias) <> '') UNIQUE,"
               "`description`	TEXT, "
               "`flag` INTEGER NOT NULL DEFAULT 0 CHECK(`flag` BETWEEN 0 AND 1)" // 0 - user; 1 - system
               ");");

    query.exec("CREATE TRIGGER IF NOT EXISTS ParametrAliases_on_delete \r\n"
               "BEFORE DELETE ON ParametrAliases \r\n"
               "FOR EACH ROW WHEN OLD.flag = 1 \r\n"
               "BEGIN \r\n"
               "SELECT RAISE(ABORT, 'Системный алиас не может быть удален'); \r\n"
               "END;"
               );

    query.exec("CREATE TRIGGER IF NOT EXISTS ParametrAliases_on_update \r\n"
               "BEFORE UPDATE ON ParametrAliases \r\n"
               "FOR EACH ROW WHEN OLD.flag = 1 \r\n"
               "BEGIN \r\n"
               "SELECT RAISE(ABORT, 'Системный алиас не может быть изменен'); \r\n"
               "END;"
               );

    setupSystemAliases(query);
    setupKsuAliases(query);

    //##################################################################################

    /// SAPParametrs - параметры САП
    /// KsuTypes - типы КСУ
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `SAPParametrs` " //параметры САП
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`code`	INTEGER NOT NULL UNIQUE,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE,"
               "`description`	TEXT,"
               "`parametrsConfigPath` TEXT NOT NULL CHECK(trim(parametrsConfigPath) <> '')"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `KsuTypes`"
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`code` INTEGER NOT NULL CHECK(code > 0) UNIQUE,"
               "`ksuName` TEXT NOT NULL CHECK(trim(ksuName) <> '') UNIQUE,"
               "`manufacturer` TEXT,"
               "`sapParamId`	INTEGER NOT NULL,"
               "FOREIGN KEY(`sapParamId`) REFERENCES `SAPParametrs`(`id`) ON UPDATE CASCADE ON DELETE RESTRICT"
               ");");

#ifdef SAP_KSU
    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(1, 1, '%1', '%2', '%3');")
               .arg(sapParamsConstString + defaultTemplate)
               .arg(sapParamsConstString + defaultTemplate)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + defaultTemplate).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(1, 1, '%1', '%2', %3);")
               .arg(defaultTemplate)
               .arg("ООО \"Ижевский Радиозавод\"")
               .arg(1));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(2, 2, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuArgus)
               .arg(sapParamsConstString + ksuArgus)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuArgus).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(2, 2, '%1', '%2', %3);")
               .arg(ksuArgus)
               .arg("ОАО \"Борец\"")
               .arg(2));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(3, 3, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuBmk5Bmk8)
               .arg(sapParamsConstString + ksuBmk5Bmk8)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuBmk5Bmk8).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(3, 3, '%1', '%2', %3);")
               .arg(ksuBmk5Bmk8)
               .arg("ООО \"Ижевский Радиозавод\"")
               .arg(3));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(4, 4, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuBmk7)
               .arg(sapParamsConstString + ksuBmk7)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuBmk7).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(4, 4, '%1', '%2', %3);")
               .arg(ksuBmk7)
               .arg("ООО \"Ижевский Радиозавод\"")
               .arg(4));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(5, 5, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuBorezVD)
               .arg(sapParamsConstString + ksuBorezVD)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuBorezVD).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(5, 5, '%1', '%2', %3);")
               .arg(ksuBorezVD)
               .arg("ОАО \"Борец\"")
               .arg(5));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(6, 6, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuElecton08)
               .arg(sapParamsConstString + ksuElecton08)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuElecton08).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(6, 6, '%1', '%2', %3);")
               .arg(ksuElecton08)
               .arg("ЗАО \"Электон\"")
               .arg(6));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(7, 7, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuElecton091)
               .arg(sapParamsConstString + ksuElecton091)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuElecton091).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(7, 7, '%1', '%2', %3);")
               .arg(ksuElecton091)
               .arg("ЗАО \"Электон\"")
               .arg(7));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(8, 8, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuElecton10C1)
               .arg(sapParamsConstString + ksuElecton10C1)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuElecton10C1).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(8, 8, '%1', '%2', %3);")
               .arg(ksuElecton10C1)
               .arg("ЗАО \"Электон\"")
               .arg(8));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(9, 9, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuIrz500)
               .arg(sapParamsConstString + ksuIrz500)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuIrz500).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(9, 9, '%1', '%2', %3);")
               .arg(ksuIrz500)
               .arg("ООО \"Ижевский Радиозавод\"")
               .arg(9));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(10, 10, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuKaskad2200)
               .arg(sapParamsConstString + ksuKaskad2200)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuKaskad2200).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(10, 10, '%1', '%2', %3);")
               .arg(ksuKaskad2200)
               .arg("ОАО \"Борец\"")
               .arg(10));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(11, 11, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuKaskadNT)
               .arg(sapParamsConstString + ksuKaskadNT)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuKaskadNT).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(11, 11, '%1', '%2', %3);")
               .arg(ksuKaskadNT)
               .arg("ОАО \"Борец\"")
               .arg(11));

    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(12, 12, '%1', '%2', '%3');")
               .arg(sapParamsConstString + ksuUKSU)
               .arg(sapParamsConstString + ksuUKSU)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + ksuUKSU).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(12, 12, '%1', '%2', %3);")
               .arg(ksuUKSU)
               .arg("Универсальный контроллер СУ")
               .arg(12));
#endif
#ifdef SAP_SU
    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(13, 13, '%1', '%2', '%3');")
               .arg(sapParamsConstString + sapsu)
               .arg(sapParamsConstString + sapsu)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + sapsu).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(13, 13, '%1', '%2', %3);")
               .arg(sapsu)
               .arg("ООО ИРЗ ТЭК")
               .arg(13));
#endif
#ifdef MECHOS
    query.exec(QString("INSERT OR IGNORE INTO `SAPParametrs`(id, code, name, description, parametrsConfigPath) "
                       "VALUES(13, 13, '%1', '%2', '%3');")
               .arg(sapParamsConstString + mechanic)
               .arg(sapParamsConstString + mechanic)
               .arg(QString(QCryptographicHash::hash(((sapParamsConstString + mechanic).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex())));

    query.exec(QString("INSERT OR IGNORE INTO `KsuTypes`(id, code, ksuName, manufacturer, sapParamId) "
                       "VALUES(13, 13, '%1', '%2', %3);")
               .arg(mechanic)
               .arg("ООО ИРЗ ТЭК")
               .arg(13));
#endif

    setupSAPParametrs();

    /// SUTypes - типы СУ
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `SUTypes`"
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`code` INTEGER NOT NULL CHECK(code > 0) UNIQUE,"
               "`SUName` TEXT NOT NULL CHECK(trim(SUName) <> '') UNIQUE,"
               "`manufacturer` TEXT"
               ");");

    //##################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `Tests`" /// тесты
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE,"
               "`description`	TEXT,"
               "`path` TEXT NOT NULL CHECK(trim(path) <> ''),"
               "`buttonType` INTEGER NOT NULL DEFAULT 0 CHECK(`buttonType` BETWEEN 0 AND 2),"
               "`protocolType` INTEGER NOT NULL DEFAULT 0 CHECK(`protocolType` BETWEEN 0 AND 2),"
               "`executeType` INTEGER NOT NULL DEFAULT 0 CHECK(`executeType` BETWEEN 0 AND 1)"  // 0 - JS; 1 - C++
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `TestKit`" /// наборы из тестов
               "("
               "`id`    INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE,"
               "`description`	TEXT,"
               "`buttonType` INTEGER NOT NULL DEFAULT 0 CHECK(`buttonType` BETWEEN 0 AND 2),"
               "`protocolType` INTEGER NOT NULL DEFAULT 0 CHECK(`protocolType` BETWEEN 0 AND 2)"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `TestKitTests`" /// связка тестов с наборами
               "("
               "`id` INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
               "`testKitId` INTEGER NOT NULL,"
               "`testId` INTEGER NOT NULL,"
               "FOREIGN KEY(`testKitId`) REFERENCES `TestKit`(`id`) ON UPDATE CASCADE ON DELETE CASCADE,"
               "FOREIGN KEY(`testId`) REFERENCES `Tests`(`id`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `CommunicationProtocols`" /// протоколы обмена
               "("
               "`code`  INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`protocolType` INTEGER NOT NULL DEFAULT 0 CHECK(protocolType BETWEEN 0 AND 1),"
               "`name` TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `Return0x10FunctionParametrsFlags`" /// флаги возвращаемых значений по 10й функции
               "("
               "`comProtocolCode`   INTEGER NOT NULL,"
               "`code` INTEGER NOT NULL,"
               "`parametrName` TEXT NOT NULL CHECK(trim(parametrName) <> ''),"
               "`format` INTEGER NOT NULL DEFAULT 0 CHECK(format BETWEEN 0 AND 7),"
               "`alias`	TEXT,"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "UNIQUE (comProtocolCode, alias),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `KindOfLaunch`" /// вид запуска
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `WorkMode`" /// режим работы
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `StopReasonCodes`" /// коды первопричин останова
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `StatusWordInterpretationCodes`" /// расшифровка слова состояния
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `KSU753ErrorCodes`" /// коды ошибок протокола 7.35
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `ModbusArchivesEventClasses`" /// классы событий архивов Modbus
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `ModbusArchivesEventCodes`" /// коды событий архивов Modbus
               "("
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`code`	INTEGER NOT NULL,"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> ''),"
               "PRIMARY KEY(`comProtocolCode`,`code`),"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE CASCADE"
               ");");


    query.exec("CREATE TABLE IF NOT EXISTS `KSUClassifierParametersTemplates`" /// шаблоны классификаторов
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`code`	INTEGER NOT NULL UNIQUE,"
               "`protocolType`	INTEGER NOT NULL DEFAULT 0 CHECK(protocolType BETWEEN 0 AND 1),"
               "`name`	TEXT NOT NULL CHECK(trim(name) <> '') UNIQUE,"
               "`parametrsConfigPath` TEXT NOT NULL CHECK(trim(parametrsConfigPath) <> '')"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS `KsuSoftwareVersions` " /// версии ПО КСУ
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`softwareVersion`	TEXT NOT NULL CHECK(trim(`softwareVersion`) <> ''),"
               "`protocolType`	INTEGER NOT NULL DEFAULT 0 CHECK(`protocolType` BETWEEN 0 AND 1),"
               "`parametrsClassifierName` TEXT,"
               "`parametrsConfigPath`	TEXT NOT NULL CHECK(trim(`parametrsConfigPath`) <> '') UNIQUE,"
               "`ksuTypeId`	INTEGER NOT NULL,"
               "`comProtocolCode`	INTEGER NOT NULL,"
               "`parametrsTemplateId`  INT,"
               "UNIQUE(`softwareVersion`, `ksuTypeId`),"
               "FOREIGN KEY(`ksuTypeId`) REFERENCES `KsuTypes`(`id`) ON UPDATE CASCADE ON DELETE CASCADE,"
               "FOREIGN KEY(`comProtocolCode`) REFERENCES `CommunicationProtocols`(`code`) ON UPDATE CASCADE ON DELETE RESTRICT,"
               "FOREIGN KEY(`parametrsTemplateId`) REFERENCES `KSUClassifierParametersTemplates`(`id`) ON UPDATE CASCADE ON DELETE SET NULL"
               ");");

    query.exec("CREATE TRIGGER IF NOT EXISTS KSUClassifierParametersTemplates_on_delete \r\n"
               "BEFORE DELETE ON KSUClassifierParametersTemplates \r\n"
               "BEGIN \r\n"
               "UPDATE KsuSoftwareVersions \r\n"
               "SET parametrsClassifierName = old.name \r\n"
               "WHERE parametrsTemplateId = old.id AND \r\n"
               "parametrsClassifierName IS NULL; \r\n"
               "END;"
               );

    /// Отчет по проведенным тестам
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `TestHistoryReport`"
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`datetime` DATETIME DEFAULT (datetime('now', 'localtime')),"
               "`user` TEXT NOT NULL CHECK(trim(user) <> ''),"
               "`ksuName` TEXT NOT NULL CHECK(trim(ksuName) <> ''),"
               "`softwareVersion` TEXT NOT NULL CHECK(trim(`softwareVersion`) <> ''),"
               "`kitName` TEXT NOT NULL CHECK(trim(kitName) <> ''),"
               "`testName` TEXT NOT NULL CHECK(trim(testName) <> ''),"
               "`stepNumber` TEXT NOT NULL CHECK(trim(stepNumber) <> ''),"
               "`stepName` TEXT NOT NULL CHECK(trim(stepName) <> ''),"
               "`function` TEXT NOT NULL CHECK(trim(function) <> ''),"
               "`message` TEXT,"
               "`aliasName` TEXT,"
               "`aliasValue` TEXT,"
               "`result` TEXT"
               ");");
    // #################################################################################

    /// Отчет по действиям оператора
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `OperatorClickReport`"
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`datetime`	DATETIME DEFAULT (datetime('now', 'localtime')),"
               "`user` TEXT NOT NULL CHECK(trim(user) <> ''),"
               "`obectName` TEXT,"
               "`message` TEXT"
               ");");
    // #################################################################################


    /// Отчет по ошибкам
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `FaultReport`"
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`datetime`	DATETIME DEFAULT (datetime('now', 'localtime')),"
               "`user` TEXT NOT NULL CHECK(trim(user) <> ''),"
               "`objectName` TEXT,"
               "`message` TEXT"
               ");");
    // #################################################################################


    /// Отчет по устойчивости связи
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `LinkStabilityReport`"
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`begindatetime`	DATETIME,"
               "`enddatetime`	DATETIME,"
               "`user` TEXT NOT NULL CHECK(trim(user) <> ''),"
               "`ksuName` TEXT NOT NULL CHECK(trim(ksuName) <> ''),"
               "`softwareVersion` TEXT NOT NULL CHECK(trim(`softwareVersion`) <> ''),"
               "`portsettings` TEXT NOT NULL CHECK(trim(`portsettings`) <> ''),"
               "`txcount`	INTEGER NOT NULL,"
               "`rxcount` INTEGER NOT NULL,"
               "`errorcount` INTEGER NOT NULL,"
               "`quality` TEXT NOT NULL CHECK(trim(`quality`) <> ''),"
               "`spquality` TEXT NOT NULL CHECK(trim(`spquality`) <> ''),"
               "`result` TEXT,"
               "`message` TEXT"
               ");");
    // #################################################################################

    /// Отчет по параметрам КСУ, по изменениям параметров КСУ, отрицательным изменениям
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `ParamsReport`"
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`datetime`	DATETIME DEFAULT (datetime('now', 'localtime')),"
               "`user` TEXT NOT NULL CHECK(trim(user) <> ''),"
               "`ksuName` TEXT NOT NULL CHECK(trim(ksuName) <> ''),"
               "`softwareVersion` TEXT NOT NULL CHECK(trim(`softwareVersion`) <> ''),"
               "`typeSU` TEXT,"
               "`function` TEXT NOT NULL CHECK(trim(function) <> ''),"
               "`rx` TEXT,"
               "`tx` TEXT,"
               "`aliasName` TEXT,"
               "`aliasValue` TEXT,"
               "`result` TEXT"
               ");");
    // #################################################################################

    /// Отчет по соответствию параметров после перепрограммирования
    //###################################################################################

    query.exec("CREATE TABLE IF NOT EXISTS `UpdateKsuReport`"
               "("
               "`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
               "`datetime`	DATETIME DEFAULT (datetime('now', 'localtime')),"
               "`user` TEXT NOT NULL CHECK(trim(user) <> ''),"
               "`ksuName` TEXT NOT NULL CHECK(trim(ksuName) <> ''),"
               "`softwareVersion` TEXT,"
               "`aliasName` TEXT,"
               "`address` TEXT,"
               "`name` TEXT,"
               "`aliasOldValue` TEXT,"
               "`aliasNewValue` TEXT,"
               "`result` TEXT"
               ");");
    // #################################################################################

}

void DBConfig::setupSystemAliases(QSqlQuery &query)
{
#ifdef SAP_KSU
    QFile file(":/DBConfig/SystemAliases.txt");
#endif
#ifdef SAP_SU
    QFile file(":/DBConfig/SystemAliasesSAPSU.txt");
#endif
#ifdef MECHOS
    QFile file(":/DBConfig/SystemAliasesMechos.txt");
#endif

    if (!file.open(QFile::ReadOnly))
        return;

    QTextStream textStream(&file);

    _configDB.transaction();

    while (!textStream.atEnd()) {

        QString line = textStream.readLine();

        if (line.trimmed().isEmpty() || line.startsWith(QLatin1String("//")))
            continue;

        int index = line.indexOf(' ');

        QString key = line.left(index).trimmed();

        if (key.isEmpty())
            continue;

        QString value = line.remove(0, index).trimmed();

        query.exec(QString("INSERT OR IGNORE INTO `ParametrAliases`(alias, description, flag) "
                           "VALUES('%1', '%2', 1);")
                   .arg(key)
                   .arg(value));
    }

    _configDB.commit();

    file.close();
}

void DBConfig::setupKsuAliases(QSqlQuery &query)
{
    QFile file(":/DBConfig/KSUAliases.txt");

    if (!file.open(QFile::ReadOnly))
        return;

    QTextStream textStream(&file);

    _configDB.transaction();

    while (!textStream.atEnd()) {
        QString line = textStream.readLine();
        if (line.trimmed().isEmpty() || line.startsWith(QLatin1String("//")))
            continue;

        int index = line.indexOf(' ');
        QString key = line.left(index).trimmed();

        if (key.isEmpty())
            continue;

        QString value = line.remove(0, index).trimmed();
        query.exec(QString("INSERT OR IGNORE INTO `ParametrAliases`(alias, description, flag) "
                           "VALUES('%1', '%2', 0);")
                   .arg(key)
                   .arg(value));
    }

    _configDB.commit();
    file.close();
}

void DBConfig::setupSAPParametrs()
{
    //TODO: пересмотреть создание файлов и привязку их к сап параметрам в базе

    QStringList filePaths;
    QStringList templateFilePaths;

#ifdef SAP_KSU
    filePaths << sapParamsConstString + ksuArgus
              << sapParamsConstString + ksuBmk5Bmk8
              << sapParamsConstString + ksuBmk7
              << sapParamsConstString + ksuBorezVD
              << sapParamsConstString + defaultTemplate
              << sapParamsConstString + ksuElecton08
              << sapParamsConstString + ksuElecton091
              << sapParamsConstString + ksuElecton10C1
              << sapParamsConstString + ksuIrz500
              << sapParamsConstString + ksuKaskad2200
              << sapParamsConstString + ksuKaskadNT
              << sapParamsConstString + ksuUKSU
              << sapParamsConstString + sapsu;

    templateFilePaths << ":/DBConfig/SAPParametrs/KSU_Argus.xml"
                      << ":/DBConfig/SAPParametrs/KSU_BMK5_BMK8.xml"
                      << ":/DBConfig/SAPParametrs/KSU_BMK7.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Borez_VD.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Default_Template.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Electon_08.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Electon_091.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Electon_10C1.xml"
                      << ":/DBConfig/SAPParametrs/KSU_IRZ500.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Kaskad_2_200.xml"
                      << ":/DBConfig/SAPParametrs/KSU_Kaskad_NT.xml"
                      << ":/DBConfig/SAPParametrs/KSU_UKSU.xml"
                      << ":/DBConfig/SAPParametrs/SAPSU_Default_Template.xml";
#endif
#ifdef SAP_SU
    filePaths << sapParamsConstString + sapsu;
    templateFilePaths << ":/DBConfig/SAPParametrs/SAPSU_Default_Template.xml";
#endif
#ifdef MECHOS
    filePaths << sapParamsConstString + mechanic;
    templateFilePaths << ":/DBConfig/SAPParametrs/Mechos_Default_Template.xml";
#endif

    QDir dir(DBConfig::sapParametrsFolderPath());
    if (!dir.mkpath(dir.path()))
        return;

    for (int a = 0; a < filePaths.count(); a++) {

        QString filePath = dir.absoluteFilePath(QString(QCryptographicHash::hash((filePaths.at(a).toLocal8Bit().constData()), QCryptographicHash::Md5).toHex()));
        QString templateFilePath = templateFilePaths.at(a);

        if (templateFilePath.isEmpty() || !QFile::exists(templateFilePath))
            continue;

        if (!QFile::copy(templateFilePath, filePath))
            continue;

        QFile::setPermissions(filePath,
                              QFile::ReadOwner |
                              QFile::WriteOwner |
                              QFileDevice::WriteOther |
                              QFileDevice::ReadOther);

    }
}
