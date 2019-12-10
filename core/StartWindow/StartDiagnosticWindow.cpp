#include "StartDiagnosticWindow.h"
#include "DBConfig/DBConfig.h"
#include "Authentication/SystemUser.h"
#include "AppSettings/AppSettings.h"
#include "Authentication/LoginDialog.h"
#include "ReportManager/FaultReport.h"

static QString DIRECTLOGIC_HOSTADDRESS = "192.168.1.100";
static quint16 DIRECTLOGIC_HOSTPORT = 502;

static inline QString styleSheetAppSettingsKey() { return QStringLiteral("color-scheme"); }

StartDiagnosticWindow::StartDiagnosticWindow(QWidget *parent)
    : QWidget(parent)
{
    DBConfig::instance();
    SystemUser::instance();
    AppSettings::settings();
    installStyleSheet();

    QPalette* palette = new QPalette();

#ifdef SAP_KSU
    palette->setBrush(QPalette::Background,*(new QBrush(*(new QPixmap(":/Images/splash.png")))));
    setPalette(*palette);
    directLogicLinkTest();
#endif
#ifdef SAP_SU
    palette->setBrush(QPalette::Background,*(new QBrush(*(new QPixmap(":/Images/splash2.png")))));
    setPalette(*palette);
#endif
#ifdef MECHOS
    palette->setBrush(QPalette::Background,*(new QBrush(*(new QPixmap(":/Images/splashMechos.png")))));
    setPalette(*palette);
#endif

    QTimer::singleShot(5000, this, SLOT(close()));
}

StartDiagnosticWindow::~StartDiagnosticWindow()
{

}

void StartDiagnosticWindow::closeEvent(QCloseEvent *event)
{
    LoginDialog *loginDialog = new LoginDialog;
    loginDialog->setAttribute(Qt::WA_DeleteOnClose);
    loginDialog->setWindowFlags(Qt::WindowTitleHint);
    loginDialog->setWindowModality(Qt::ApplicationModal);
    loginDialog->setWindowTitle("Авторизация пользователя");
    loginDialog->setWindowIcon(QIcon(":/Images/access_user.png"));
    loginDialog->setFixedSize(QSize(370, 370));
    loginDialog->move(QApplication::desktop()->screenGeometry().center() - loginDialog->rect().center());
    loginDialog->showNormal();

    event->accept();
}

StartDiagnosticWindow *StartDiagnosticWindow::instance()
{
    static StartDiagnosticWindow singletone;
    return &singletone;
}

void StartDiagnosticWindow::setupStyleSheet()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());

    QString path = settings->value(styleSheetAppSettingsKey()).toString();

    if (path.isEmpty())
        path = QDir ((QCoreApplication::applicationDirPath() + QDir::separator() + "StyleSheets")).absolutePath();

    QString caption = tr("Загрузить цветовую схему");
    QString filter = tr("Шаблон стилей приложения (*.qss)");

    QString fileName = QFileDialog::getOpenFileName(this, caption, path, filter);

    if (!fileName.isEmpty()) {
        settings->setValue(styleSheetAppSettingsKey(), fileName);
        installStyleSheet(fileName);
    }

    settings->endGroup();
    settings->sync();
    //OperatorClickReport::instance()->write(QStringLiteral("Изменение цветовой схемы графического интерфейса"));
}

void StartDiagnosticWindow::installStyleSheet()
{
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "StyleSheets");
    dir.mkpath(dir.path());

    QFile::copy(":/Stylesheets/dark-orange-green.qss", dir.path()+ QDir::separator() + "dark-orange-green.qss");
    QFile::copy(":/Stylesheets/dark-orange.qss", dir.path()+ QDir::separator() + "dark-orange.qss");
    QFile::copy(":/Stylesheets/default.qss", dir.path()+ QDir::separator() + "default.qss");
    QFile::copy(":/Stylesheets/light-green.qss", dir.path()+ QDir::separator() + "light-green.qss");

    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());
    QString path = settings->value(styleSheetAppSettingsKey()).toString();
    installStyleSheet(path);
    settings->endGroup();
    settings->sync();
}

void StartDiagnosticWindow::installStyleSheet(const QString &styleSheetPath)
{
    QFile styleFile(styleSheetPath);

    if (styleFile.open(QFile::ReadOnly)) {
        QByteArray line;
        QStringList names;
        QStringList values;

        quint64 pos = 0;
        while (!styleFile.atEnd()) {

            line = styleFile.readLine();

            if (line[0] == '@') {
                if (line == "@END*/\n") {
                    pos = styleFile.pos();
                    break;
                }

                names.append(line.left(line.indexOf('=')));
                values.append(line.left(line.lastIndexOf(';')).mid(line.lastIndexOf('=') + 1));
            }

            line.clear();
        }

        styleFile.seek(pos);

        QString styleSheet = QLatin1String(styleFile.readAll());

        for (quint64 i = 0; i < names.size(); i++) {
            styleSheet.replace(names[i], values[i]);
        }

        qApp->setStyleSheet(styleSheet);
    }
    else {
        qDebug() << tr("Не удалось загрузить цветовую схему");
    }

    styleFile.close();
}

qint8 StartDiagnosticWindow::directLogicLinkTest()
{
    float quality = 0;

    QHostAddress dlHost;
    dlHost.setAddress(DIRECTLOGIC_HOSTADDRESS);

    QTcpSocket *dlSocket = new QTcpSocket(this);

    connect(dlSocket, &QTcpSocket::readyRead, [dlSocket, &quality]() {

        if (!dlSocket->readAll().isEmpty())
            quality += float(100)/13;
    });

    connect(dlSocket, &QTcpSocket::bytesWritten, [dlSocket]() {
        dlSocket->waitForReadyRead();
    });

    connect(dlSocket, &QTcpSocket::connected, [dlSocket]() {

        // Read In/out from 2048 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000000000006010108000010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2064 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000100000006010108100010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2080 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000200000006010108200010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2096 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000300000006010108300010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2112 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000400000006010108400010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2272 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000500000006010208E00010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2288 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000600000006010208F00010").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read Register from 1088 for 8
        dlSocket->write(QByteArray::fromHex(QString("000700000006010304400008").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read Register from 1096 for 8
        dlSocket->write(QByteArray::fromHex(QString("000800000006010304480008").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read Register from 1104 for 8
        dlSocket->write(QByteArray::fromHex(QString("000900000006010304500008").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read Register from 1112 for 8
        dlSocket->write(QByteArray::fromHex(QString("000A00000006010304580008").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read Register from 1024 for 8
        dlSocket->write(QByteArray::fromHex(QString("000B00000006010404000008").toLatin1()));
        dlSocket->waitForBytesWritten();

        // Read In/out from 2256 for 16bits
        dlSocket->write(QByteArray::fromHex(QString("000C00000006010108D00010").toLatin1()));
        dlSocket->waitForBytesWritten();
    });

    connect(dlSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
            [dlSocket, &quality](QAbstractSocket::SocketError error)
    {
        QMessageBox msgBox;
        msgBox.setText(QString("Нет связи с контроллером Directlogic: \r\nОшибка %1 - смотри описание QAbstractSocket::SocketError").arg(error));
        msgBox.exec();

        FaultReport::instance()->write("Нет связи с контроллером Directlogic");

        qApp->closeAllWindows();
    });

    dlSocket->connectToHost(dlHost, DIRECTLOGIC_HOSTPORT);
    dlSocket->waitForConnected(3000);

    dlSocket->disconnectFromHost();
    dlSocket->deleteLater();

    return static_cast<qint8>(roundf(quality));
}

qint8 StartDiagnosticWindow::afcLinkTest()
{
    //    float quality = 0;

    //    QHostAddress dlHost;
    //    dlHost.setAddress(DIRECTLOGIC_HOSTADDRESS);

    //    QSerialPort *port = new QSerialPort;
    //    QTcpSocket *dlSocket = new QTcpSocket(this);

    //    connect(dlSocket, &QTcpSocket::readyRead, [dlSocket, &quality]() {

    //        if (!dlSocket->readAll().isEmpty())
    //            quality += float(100)/13;
    //    });

    //    connect(dlSocket, &QTcpSocket::bytesWritten, [dlSocket]() {
    //        dlSocket->waitForReadyRead();
    //    });

    //    connect(dlSocket, &QTcpSocket::connected, [dlSocket]() {

    //        // Read In/out from 2048 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000000000006010108000010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2064 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000100000006010108100010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2080 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000200000006010108200010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2096 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000300000006010108300010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2112 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000400000006010108400010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2272 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000500000006010208E00010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2288 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000600000006010208F00010").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read Register from 1088 for 8
    //        dlSocket->write(QByteArray::fromHex(QString("000700000006010304400008").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read Register from 1096 for 8
    //        dlSocket->write(QByteArray::fromHex(QString("000800000006010304480008").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read Register from 1104 for 8
    //        dlSocket->write(QByteArray::fromHex(QString("000900000006010304500008").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read Register from 1112 for 8
    //        dlSocket->write(QByteArray::fromHex(QString("000A00000006010304580008").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read Register from 1024 for 8
    //        dlSocket->write(QByteArray::fromHex(QString("000B00000006010404000008").toLatin1()));
    //        dlSocket->waitForBytesWritten();

    //        // Read In/out from 2256 for 16bits
    //        dlSocket->write(QByteArray::fromHex(QString("000C00000006010108D00010").toLatin1()));
    //        dlSocket->waitForBytesWritten();
    //    });

    //    connect(dlSocket, static_cast<void (QTcpSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
    //            [dlSocket, &quality](QAbstractSocket::SocketError error)
    //    {
    //        QMessageBox msgBox;
    //        msgBox.setText(QString("Нет связи с контроллером Directlogic: \r\nОшибка %1 - смотри описание QAbstractSocket::SocketError").arg(error));
    //        msgBox.exec();

    //        qApp->closeAllWindows();

    //    });

    //    dlSocket->connectToHost(dlHost, DIRECTLOGIC_HOSTPORT);
    //    dlSocket->disconnectFromHost();

    //    return static_cast<qint8>(roundf(quality));

}
