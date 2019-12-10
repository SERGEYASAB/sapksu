#include <QApplication>
#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QLocale>

#include <QSerialPort>
#include <QSerialPortInfo>

#include <QVariant>
#include <QtGlobal>
#include <QSettings>

#include "Authentication/LoginDialog_copy.h"
#include "Authentication/LoginDialog.h"
#include "Logger/LoggerWidget.h"
#include "Logger/WindowsEventLogger.h"
#include "Logger/FileLogger.h"
#include "Logger/LoggerTaskManager.h"

#include "IOServer/nsVariable/VariableValue/DeviceVariableValue/ModbusDeviceVariableValue.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "IOServer/nsVariable/VariableValue/VariableValueTypeTraits.h"
#include "IOServer/nsVariable/VariableValue/ServerVariableValue.h"

#include "DBConfig/DBConfig.h"
//#include <bitset>

#include "IOServer/nsVariable/VariableValue/VariableValueStrategy/BitsetVariableValueStrategy.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "Ksu735Crc.h"
#include "EnergoFormCrc16.h"
#include "IOServer/EnergoForm/EnergoFormWordVariable.h"
#include "IOServer/EnergoForm/EnergoFormDWordVariable.h"
#include "IOServer/EnergoForm/EnergoFormNode.h"
#include "StartWindow/StartDiagnosticWindow.h"
#include "StartWindow/StartDiagnosticWindow_copy.h"
#include "TestManager/ExecutorWindow/ExecutorWindow_copy.h"

//#include "Common/FKValidatorSpinBox/FKValidatorHelper.h"
//#include "Common/FKValidatorSpinBox/FKUInt64Validator.h"
#include "Common/FKValidatorSpinBox/FKCP866StringValidator.h"
#include <QTextCodec>

#include "Logger/Logger2/LoggerTaskManager2.h"

#include <QtQuickControls2>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "Stylesheets/QmlStyle.h"
#include "MainEngine.h"
#include <QtQml>
//#include "TextFieldDoubleValidator.h"

class LoggerWidget;
class WindowsEventLogger;
class FileLogger;
class LoggerTaskManager;

// TODO: использовать до 5.6
extern Q_CORE_EXPORT QBasicAtomicInt qt_qhash_seed;

void testKsuNode()
{
    //    Ksu0x10Parametrs735Node *node = new Ksu0x10Parametrs735Node(6);

    //    QByteArray tx = QByteArray::fromHex("0001200A000102030405060708095800");
    //    QByteArray rx = QByteArray::fromHex("0001201E0000000100000200000300000413000500000600000700000800000954653801");
    ////    rx.append(Ksu735Crc::getInstance()->calcCrc(rx, 0, rx.size()));

    //    node->parseResponse(std::shared_ptr<IDataTask>(), tx, rx);

    QByteArray asciiBytes;
    for (int i = 32; i < 127; i++) {
        asciiBytes.append(char(i));
    }

    QByteArray cp866SpecialBytes = "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдежзийклмноп"
                                   "░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀"
                                   "рстуфхцчшщъыьэюяЁёЄєЇїЎў°∙·√№¤■ ";

    QByteArray cp866BytesTest = asciiBytes.append(cp866SpecialBytes);

    QByteArray cp866Bytes;
    for (int i = 0; i < 256; i++) {
        cp866Bytes.append(char(i));
    }

    QTextDecoder *decoder = QTextCodec::codecForName("cp866")->makeDecoder(QTextCodec::IgnoreHeader | QTextCodec::ConvertInvalidToNull);
    QString cp866String = decoder->toUnicode(cp866Bytes);

    qDebug() << "cp866 string";
    qDebug() << cp866String;
    qDebug() << cp866Bytes;

    qDebug() << "cp866SpecialBytes";
    qDebug() << cp866SpecialBytes;

    qDebug() << "cp866SpecialBytes string \r\n";
    qDebug() << QString(cp866SpecialBytes);
    qDebug() << "contains" << QString(cp866SpecialBytes).contains("▓") << "\r\n";

    qDebug() << "last try";
    qDebug() << decoder->toUnicode(cp866SpecialBytes);



    qDebug() << "test";
    qDebug() << QString(QChar(0x2593));

    QLineEdit *le = new QLineEdit();
    le->setText(QString(cp866BytesTest));
    le->show();
}

int main(int argc, char *argv[])
{
    // TODO: использовать до 5.6
//    qt_qhash_seed.store(5);

    // TODO: ипользовтать в версиях 5.6 и старше
    qSetGlobalQHashSeed(0);

    QLocale c(QLocale::c());
    c.setNumberOptions(QLocale::RejectGroupSeparator);
    QLocale::setDefault(c);

    //    BitsetVariableValueStrategy strategy;
    //    QMap<quint64, QString> map;
    //    map[1 << 0] = "1 << 0";
    //    map[1 << 1] = "1 << 1";
    //    map[1 << 2] = "1 << 2";
    //    map[1 << 3] = "1 << 3";
    //    map[1 << 4] = "1 << 4";
    //    strategy.setBitsValues(map);

    //    QVariant test = strategy.variantValueFromDevice(31);
    //    qDebug() << "test" << test;

    //    quint16 test1 = 1242;
    //    QVariant varTest = QVariant::fromValue(test1);
    //    qDebug() << "test variant" << varTest.canConvert<void>();


    //    quint16 num = 1610;
    //    quint16 res = QString::number(num).toUShort(0, 16);
    //    QByteArray controlTest = bytesFromValue(res);
    //    QByteArray result = QByteArray::number(num, 10);
    //    qDebug() << "test QByteArray::number" << res << result << result.toUShort(nullptr, 10) << controlTest << controlTest.toHex().toUpper();
    //    qDebug() << "exp" << QByteArray(result.data(), result.size()).toHex() << QByteArray::fromHex(result.toHex());

    //    int x = VariableValueTypeTraits<10>::value;
    //    qDebug() << x << typeid(VariableValueTypeTraits<IVariable::Float32>::type).name() << typeid(VariableValueTypeTraits<IVariable::Int8>::type).name();

    //    IServerVariableValue *vv = new ServerVariableValue<int>();
    //    vv->setValue(QVariant(2.45));
    //    qDebug() << "vv" << vv->value();

    //    quint32 test = 0xC0F40000;
    //    qint32 val = static_cast<qint32>(test);
    //    qDebug() << "val" << test << val;

    //    IVariable *var = new ModbusVariable();
    //    IDeviceVariableValue *varVal = new ModbusDeviceVariableValue<float>(*var);

    //    QByteArray bytes = bytesFromValue(test);
    //    bool result = varVal->setValueBytes(bytes);
    //    qDebug() << "conModB" << test << varVal->variantValue();

    //    QByteArray convertBytes = varVal->bytesFromValue(varVal->variantValue());
    //    qDebug() << "bytes" << bytes.toHex().toUpper() << convertBytes.toHex().toUpper() << (bytes == convertBytes);

    //    qDebug() << "convert" << convertBytesToType<qint16>(bytes);



    //    IVariable var;
    //    IVariableValue *val = new VariableValue<double, void>(var);
    //    QByteArray floatbytes;
    //    floatbytes[0] = 0x40;
    //    floatbytes[1] = 0xd6;
    //    floatbytes[2] = 0x9f;
    //    floatbytes[3] = 0xbe;
    //    val->setValueBytes(floatbytes);
    //    qDebug() << val->variantValue();

    //    //эту хрень можно показать Тарасу
    //    char c = -127;
    //    qDebug() << "127" << 127 << QString(std::bitset<8>(127).to_string().c_str());
    //    qDebug() << "-127" << -127 << QString(std::bitset<8>(-127).to_string().c_str());
    //    quint8 t = quint8(c);
    //    qDebug() << "t" << t;
    //    qint32 tt = static_cast<qint32>(quint8(c));
    //    qDebug() << "tt" << tt << QString(std::bitset<32>(tt).to_string().c_str());
    //    qDebug() << "120" << 120 << QString(std::bitset<32>(120).to_string().c_str());
    //    tt <<= 8;
    //    qDebug() << "tt" << tt << QString(std::bitset<32>(tt).to_string().c_str());

    //    qDebug() << "65535" << 65535 << QString(std::bitset<32>(65535).to_string().c_str());

    //    quint32 x = quint32(c) << 8;
    //    qDebug() << "x" << x;

    //    std::bitset<32> bitset = x;
    //    qDebug() << "bitset" << QString(bitset.to_string().c_str());

    //    std::bitset<32> test = c;
    //    qDebug() << "bitset" << QString(test.to_string().c_str());
    //    test <<= 8;
    //    qDebug() << "bitset" << QString(test.to_string().c_str());





    //    IVariableValue *val = new VariableValue<int>();
    //    IVariableValue *vcl = new VariableValue<bool>();
    //    qDebug() << "test" << val->isBoolean() << vcl->isBoolean();

    // TODO: Прокачать логгер

    //    test();




   //qInstallMessageHandler(appMessageOutputHandler);



    //    testKsuNode();

    //    MainWindow window;

#ifdef QML_DESIGN

    QApplication app(argc, argv);
    qApp->setQuitOnLastWindowClosed(false);
    app.setAttribute(Qt::AA_EnableHighDpiScaling);

    QQmlApplicationEngine *engine = &MainEngine::instance()->engine();

    qmlRegisterType<QmlStyle>("QmlStyle", 1, 0, "QmlStyle");
    qmlRegisterType<StartDiagnosticWindow_copy>("StartDiagnosticWindow_copy", 1, 0, "StartDiagnosticWindow_copy");
    qmlRegisterType<LoginDialog_copy>("LoginDialog_copy", 1, 0, "LoginDialog_copy");
//    qmlRegisterType<TextFieldDoubleValidator>("TextFieldDoubleValidator", 1,0,
//                                              "TextFieldDoubleValidator");

    const QUrl url(QStringLiteral("qrc:/StartWindow/StartDiagnosticWindow.qml"));
    QObject::connect(engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
        engine->load(url);
    LoginDialog_copy:: instance();

//    StartDiagnosticWindow_copy *diagnosticWindow = StartDiagnosticWindow_copy::instance();

//    diagnosticWindow -> startMainDialog();

#else
    QApplication app(argc, argv);

    qmlRegisterType<QmlStyle>("QmlStyle", 1, 0, "QmlStyle");
    qmlRegisterType<StartDiagnosticWindow_copy>("StartDiagnosticWindow_copy", 1, 0, "StartDiagnosticWindow_copy");
    qmlRegisterType<LoginDialog_copy>("LoginDialog_copy", 1, 0, "LoginDialog_copy");

    app.setWindowIcon(QIcon(":/Images/irz_logo.png"));
    StartDiagnosticWindow *diagnosticWindow = StartDiagnosticWindow::instance();
    diagnosticWindow->setWindowFlags(Qt::FramelessWindowHint);
    diagnosticWindow->setFixedSize(QSize(840, 412));
    diagnosticWindow->move(QApplication::desktop()->screenGeometry().center() - diagnosticWindow->rect().center());
    diagnosticWindow->showNormal();
#endif


    //    ModbusVaribleForm *testModbusForm = new ModbusVaribleForm();
    ////    test->resize(800, 800);
    //    testModbusForm->show();

    //    testXmlWrite();


    //    QByteArray bytes = QByteArray::fromHex("00020000");
    //    QByteArray crc = EnergoFormCrc16::calcCrc(bytes);
    //    qDebug() << "EnergoFormCrc16" << crc.toHex().toUpper() << "origin" << "6E60";

    //    QByteArray rx = QByteArray::fromHex("02000C0000000000000000000000000056A7");
    //    qDebug() << "EnergoFormCrc16 exactMatch" << EnergoFormCrc16::exactMatch(rx);

    //    QVariant val = -9000;


    //    qDebug() << "testuint" << val.canConvert<quint16>() << val.value<quint16>() << static_cast<quint16>(-9000);
    //    qDebug() << "test qvar" << (QVariant(9000.0f) > QVariant());

    //    EnergoFormWordVariable var;
    //    var.setMultiplier(100);
    //    var.setValue(50);
    //    qDebug() << "EnergoFormWordVariable" << var.valueBytes().toHex().toUpper() << var.value() << var.multiplier();

    //    EnergoFormNode node;
    //    node.uploadSignalProfile();

    //    QLineEdit *line = new QLineEdit();

    //    FKUInt64Validator *validator = new FKUInt64Validator(line);
    //    validator->setTop(65524);
    //    validator->setHexValidation(true);

    //    line->setValidator(validator);
    //    line->show();

//    QByteArray bytes;

    return app.exec();
}

//void test() {
//    QByteArray buff;
//    QStringList list;
//    list << "123456789" << "qwjlefnqkwf" << "123.12" << "--432" << "+4+312" << "013303" << "0123f";

//    for (QString str : list) {
//        bool result = FKValidatorHelper::validateChars(str, FKValidatorHelper::Notation::Dec, &buff);
//        qDebug() << str << result << buff;
//    }

//    qDebug() << "--------- hex ----------";

//    list.clear();
//    list << "0xff231f" << "+1237" << "+0x12353" << "-664618" << "0xaabbccddeeff" << "accddff" << "0xtrdes"
//         << "0X123fe34" << "0xFfAaBbCcDdEe123564";

//    for (QString str : list) {
//        bool result = FKValidatorHelper::validateChars(str, FKValidatorHelper::Notation::Hex, &buff);
//        qDebug() << str << result << buff;
//    }
//}

// TODO:
// 1) Функции проверки КСУ (5.1.1)
// 2) Диагностика связи, запрет на продолжение работы
// 3) Чтение параметров КСУ выбор применяемого протокола

// 4) Уставка между запросами на одно устройство(5.2.3):
// 1) Мереем processevent
// 2) Создаем параметр в IConnection (придумать название)
// 3) в msleep (передаем этот параметр - время портаченное на processevent)
// 4) Интервал опроса < чем суммарное время задержек между опросами. Что делать?

// 5) Формирование документа "Протокол испытаний" (6.1.1)
// 6)  Уровни доступа (оператор, технолог, инженер АСУП) (6.1.11)
// 7) Настройка подключения к БД в конфигурационном файле (6.2.5)
// 8) Настройка Iconnection (6.2.6)
// 9) Проверк всех функций протоколов обмеа Modbus RTU
// 10) Согласовать использование SVN для хранения скриптов
// 11) Настройка INode (6.2.12-13)


