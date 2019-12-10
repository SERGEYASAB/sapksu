#include "StartDiagnosticWindow_copy.h"
#include "DBConfig/DBConfig.h"
#include "Authentication/SystemUser.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/FaultReport.h"

#include "StartWindow/StartWindow.h"
#include "ReportManager/OperatorClickReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

#include <QObject>
#include <QtQuickControls2>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSqlQueryModel>
#include <QCoreApplication>

StartDiagnosticWindow_copy::StartDiagnosticWindow_copy(QObject *parent)
    : QObject(parent)
{
    DBConfig::instance() ;
    SystemUser::instance();
    AppSettings::settings();
//    connect(Timer + Logindialog);

//    Timer run();
}

void StartDiagnosticWindow_copy::installStyleSheet(const QString &styleSheetPath)
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

StartDiagnosticWindow_copy::~StartDiagnosticWindow_copy()
{

}

//void StartDiagnosticWindow_copy :: startMainDialog()
//{

//    QObject::connect(engine, &QQmlApplicationEngine::objectCreated,
//                     QCoreApplication::instance())
//}

//void StartDiagnosticWindow_copy :: startLoginDialog()
//{
//    QCoreApplication::instance()->
//    QGuiApplication app(m_argc,m_argv);
//    QQmlApplicationEngine engine;
//    const QUrl url(QStringLiteral("qrc:/Authentication/LoginDialog.qml"));
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//                     &app, [url](QObject *obj, const QUrl &objUrl) {
//        if (!obj && url == objUrl)
//            QCoreApplication::exit(-1);
//    }, Qt::QueuedConnection);
//        engine.load(url);
//    app.exec();
//}

//void StartDiagnosticWindow_copy::closeEvent(QCloseEvent *event)
//{

//    QQmlComponent component(&engine,
//            QUrl::fromLocalFile("../SapKsuSurgut/Authentication/LoginDialog.qml"));
//    QObject *object = component.create();


//}
//void StartDiagnosticWindow_copy :: closeEvent(QCloseEvent *event)
//{
//    if (event->type() == QEvent::Close) {
//            QQmlEngine engine;
//            qDebug() << "Hello Start!";
//            QQuickView view;
//            view.setSource(QUrl::fromLocalFile("../SapKsuSurgut/Authentication/LoginDialog.qml"));
//            view.show();
//            QObject *object = view.rootObject();
//            delete object;
//            event->accept();
//    }

//    QObject::event(event);
//}

//void StartDiagnosticWindow_copy::setUserModel(QSqlQueryModel *userModel){
//    if( userModel == m_userModel){
//       return;
//    }
//    m_userModel =userModel;
//        emit userModelChanged();
//}

//void LoginDialog2::selectData()
//{
//    QSqlDatabase db = DBConfig::instance()->configDatabase();

//    // Изменять запрос нельзя, т.к. в данном классе все завязано на порядке следования столбцов!

//    QString sqlString = QString("SELECT Users.*, AccessLevels.name, AccessLevels.access"
//                                "   FROM `Users` "
//                                "   INNER JOIN AccessLevels "
//                                "       ON Users.accessId = AccessLevels.id "
//                                "ORDER BY `name` ASC");

//    QSqlQuery query(sqlString, db);

//    m_userModel->setQuery(query);
//}

StartDiagnosticWindow_copy *StartDiagnosticWindow_copy::instance()
{
    static StartDiagnosticWindow_copy singletone;
    return &singletone;
}

