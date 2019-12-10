#include "QApplication"
#include "LoginDialog_copy.h"
#include "DBConfig/DBConfig.h"
#include "StartWindow/StartWindow.h"
#include "StartWindow/StartDiagnosticWindow_copy.h"
#include "ReportManager/OperatorClickReport.h"
#include "ReportManager/FaultReport.h"
#include "ReportManager/ReportConfigure.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "DatabaseDirectory/Users/Users.h"
#include "TestManager/TestKitConfigureWindow/TestKitConfigureWindow.h"
#include "TestManager/TestCreatorControlWindow/TestCreatorControlWindow.h"
#include "TestManager/TestKitSelectWindow/TestKitSlectWindow_copy.h"
#include "TestManager/ExecutorWindow/ExecutorWindow_copy.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "MainEngine.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "SystemUser_copy.h"
#include "Logger/Logger2/LoggerTaskManager2.h"

LoginDialog_copy::LoginDialog_copy(QObject *parent) : QObject(parent)
{
    m_userModel = new QSqlQueryModel(this);
    m_enablePasswordChange = false;
    m_passwordChangeErr = false;
    m_error = false;
    selectData();
    setupStylesheet(1);

    connect(LoggerTaskManager2::getInstance(), &LoggerTaskManager2::timeToLog, [this](QTime time, LoggerTaskManager2::MessageType type, const QString &text){
        QString prefix;
        switch (type) {
        case LoggerTaskManager2::Info:
            break;

        case LoggerTaskManager2::Rx:
            prefix = " Rx: ";
            break;

        case LoggerTaskManager2::Tx:
            prefix = " Tx: ";
            break;

        case LoggerTaskManager2::Error:
            prefix = " Error: ";
            break;

        default:
            break;
        }
        setLogRecord(time.toString("hh.mm.ss.zzz") + prefix + text);
        qDebug()<<m_logRecord<<"123321";
        emit logged();
    });

    //    const QIcon adminIcon = QIcon(":/Images/access_robot.png");
    //    const QIcon engineerIcon = QIcon(":/Images/access_electrical_engineer.png");
    //    const QIcon operatorIcon = QIcon(":/Images/access_graduated.png");
    //    const QIcon userIcon = QIcon(":/Images/access_user2.png");

    //    setFocusPolicy(Qt::StrongFocus);

    //    // Ввод пользователя

    //    m_nameLabel = new QLabel(tr("&Имя пользователя"));
    //    m_nameComboBox = new QComboBox();
    //    m_nameComboBox->setModel(m_userModel);
    //    m_nameComboBox->setModelColumn(1);
    //    m_nameComboBox->setCurrentText(SystemUser::instance()->user());
    //    m_nameLabel->setBuddy(m_nameComboBox);

    //    connect(m_nameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LoginDialog_copy::clearUserInputs);
    //    connect(m_nameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LoginDialog_copy::updateUserIconAccessLevel);

    //    // Ввод пароля

    //    m_passwordLabel = new QLabel(tr("&Текущий пароль"));
    //    m_passwordLineEdit = new QLineEdit();
    //    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    //#ifdef QT_DEBUG
    //    m_passwordLineEdit->setText("admin");
    //#endif
    //    m_passwordLabel->setBuddy(m_passwordLineEdit);

    //    m_newPasswordLabel = new QLabel(tr("&Новый пароль"));
    //    m_newPasswordLabel->setVisible(m_enablePasswordChange);
    //    m_newPasswordLineEdit = new QLineEdit();
    //    m_newPasswordLineEdit->setVisible(m_enablePasswordChange);
    //    m_newPasswordLineEdit->setEchoMode(QLineEdit::Password);
    //    m_newPasswordLabel->setBuddy(m_newPasswordLineEdit);

    //    m_confirmNewPasswordLabel = new QLabel(tr("&Повторить пароль"));
    //    m_confirmNewPasswordLabel->setVisible(m_enablePasswordChange);
    //    m_confirmNewPasswordLineEdit = new QLineEdit();
    //    m_confirmNewPasswordLineEdit->setVisible(m_enablePasswordChange);
    //    m_confirmNewPasswordLineEdit->setEchoMode(QLineEdit::Password);
    //    m_confirmNewPasswordLabel->setBuddy(m_confirmNewPasswordLineEdit);



    //    QFormLayout *formLayout = new QFormLayout;
    //    formLayout->addRow(m_nameLabel, m_nameComboBox);
    //    formLayout->addRow(m_passwordLabel, m_passwordLineEdit);
    //    formLayout->addRow(m_newPasswordLabel, m_newPasswordLineEdit);
    //    formLayout->addRow(m_confirmNewPasswordLabel, m_confirmNewPasswordLineEdit);

    //    m_mainVerticalLayout = new QVBoxLayout;
    //    m_mainVerticalLayout->addWidget(new QLabel(tr("Уровень доступа пользователя")), 0, Qt::AlignCenter);
    //    m_mainVerticalLayout->addSpacing(20);
    //    m_mainVerticalLayout->addSpacing(20);
    //    m_mainVerticalLayout->addLayout(formLayout);
    //    m_mainVerticalLayout->addSpacing(20);
    //    m_mainVerticalLayout->addStretch();
    //    updateUserIconAccessLevel();

    //    setLayout(m_mainVerticalLayout);
}

QSqlQueryModel* LoginDialog_copy::userModel()
{
    return m_userModel;
}

void LoginDialog_copy::setuserModel(QSqlQueryModel* userModel)
{
    if(userModel == m_userModel){
        return;
    }
    m_userModel = userModel;
    emit userModelChanged();
}

bool LoginDialog_copy::enablePasswordChange()
{
    return m_enablePasswordChange;
}

void LoginDialog_copy::setEnablePasswordChange(bool enablePasswordChange)
{
    if(enablePasswordChange == m_enablePasswordChange){
        return;
    }
    m_enablePasswordChange = enablePasswordChange;
    emit enablePasswordChanged();
}


bool LoginDialog_copy::passwordChangeErr(){
    return m_passwordChangeErr;
}

void LoginDialog_copy::setPasswordChangeErr(bool passwordChangeErr){
    if(passwordChangeErr == m_passwordChangeErr){
        return;
    }
    m_passwordChangeErr = passwordChangeErr;
    emit passwordChangeErrChanged();
}

bool LoginDialog_copy::error(){
    return m_error;
}

void LoginDialog_copy::setError(bool error){
    if(error == m_error){
        return;
    }
    m_error = error;
    emit errorChanged();
}

void LoginDialog_copy::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    // Изменять запрос нельзя, т.к. в данном классе все завязано на порядке следования столбцов!

    QString sqlString = QString("SELECT Users.*, AccessLevels.name, AccessLevels.access"
                                "   FROM `Users` "
                                "   INNER JOIN AccessLevels "
                                "       ON Users.accessId = AccessLevels.id "
                                "ORDER BY `name` ASC");

    QSqlQuery query(sqlString, db);

    m_userModel->setQuery(query);

}

void LoginDialog_copy::cancel(){
    if(m_enablePasswordChange == true){
        setEnablePasswordChange(false);
    }else{
        SystemUser_copy *systemuser = SystemUser_copy::instance();
        changePasswordCorrect(false);
    }
}

void LoginDialog_copy::changePasswordCorrect(bool passwordCorrect){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    systemuser->setPasswordCorrect(passwordCorrect);
}

bool LoginDialog_copy::getPasswordCorrect(){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    bool passwordCorrect = systemuser->passwordCorrect();
    return passwordCorrect;
}

int LoginDialog_copy::getTheme(){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    int theme =  systemuser->theme();
    return theme;
}

void LoginDialog_copy::changeTheme(int theme)
{
    setupStylesheet(theme);
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    systemuser->settheme(theme);
}

QString LoginDialog_copy::getAccess(){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    return systemuser->description();
}
void LoginDialog_copy::changeAccess(QString access){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    systemuser->setDescription(access);
}

QString LoginDialog_copy::getOpenedWindow(){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    return systemuser->openedWindow();
}

void LoginDialog_copy::changeOpenedWindow(QString openwindow){
    SystemUser_copy *systemuser = SystemUser_copy::instance();
    systemuser->setOpenedWindow(openwindow);
}

QString LoginDialog_copy::getLogRecord(){
    return m_logRecord;
}

void LoginDialog_copy::setLogRecord(const QString &logRecord)
{
    m_logRecord = logRecord;
}

bool LoginDialog_copy::authorization(QString password, int index){
    QString dbpass = m_userModel->index(index, 3).data().toString();
    QString md5pass = QString(QCryptographicHash::hash((password.toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());

    if(md5pass == dbpass){
        changePasswordCorrect(true);
        return true;
    }else{
        changePasswordCorrect(false);
        return false;
    }
}

void LoginDialog_copy::updatePassword(QString password, QString newPassword, QString confirmNewPassword,int index){

    //    setPasswordChangeErr(false);
    QString dbpass = m_userModel->index(index, 3).data().toString();
    QString md5pass = QString(QCryptographicHash::hash((password.toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
    if(md5pass == dbpass){
        if(newPassword == confirmNewPassword){
            int userId = m_userModel->index(index, 0).data().toInt();
            QString newMd5Password = QString(QCryptographicHash::hash((newPassword.toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());

            QString sqlString = QString("UPDATE `Users` SET password = '%1' WHERE id = %2;")
                    .arg(newMd5Password)
                    .arg(userId);
            QSqlDatabase db = DBConfig::instance()->configDatabase();
            QSqlQuery query(db);
            setPasswordChangeErr(false);
            db.transaction();
            if (query.exec(sqlString)) {
                if (!db.commit()){
                    db.rollback();
                }
                selectData();
                setPasswordChangeErr(false);
            }  else {
                ErrorClassifier::instance()->showError(501);
            }
        }else{
            setPasswordChangeErr(true);
            return;
        }
    }else{
        setPasswordChangeErr(true);
        return;
    }

}

void LoginDialog_copy::openReports(){
    ReportConfigure *reportConfig = new ReportConfigure();
    reportConfig->setAttribute(Qt::WA_DeleteOnClose);
    reportConfig->setWindowModality(Qt::ApplicationModal);
    reportConfig->setWindowTitle("Отчеты");
    reportConfig->setWindowIcon(QIcon(":/Images/report.png"));
    reportConfig->move(QApplication::desktop()->screenGeometry().center() - reportConfig->rect().center());
    reportConfig->showMaximized();
}

void LoginDialog_copy::openUsers(){
    Users *configUsers = new Users();
    configUsers->setAttribute(Qt::WA_DeleteOnClose);
    configUsers->setWindowModality(Qt::ApplicationModal);
    configUsers->setWindowTitle("Управление пользователями");
    configUsers->setWindowIcon(QIcon(":/Images/access_user.png"));
    configUsers->setFixedSize(QSize(800, 600));
    configUsers->move(QApplication::desktop()->screenGeometry().center() - configUsers->rect().center());
    configUsers->showNormal();
}

void LoginDialog_copy::openTestsManager(){
    OperatorClickReport::instance()->write(QStringLiteral("Управление наборами тестов: открытие окна"));
    TestKitConfigureWindow *testKitControl = new TestKitConfigureWindow();
    testKitControl->setAttribute(Qt::WA_DeleteOnClose);
    testKitControl->setWindowModality(Qt::ApplicationModal);
    testKitControl->setWindowTitle("Управление наборами");
    testKitControl->setWindowIcon(QIcon(":/Images/testKitControl.png"));
    testKitControl->setFixedSize(QSize(800, 600));
    testKitControl->move(QApplication::desktop()->screenGeometry().center() - testKitControl->rect().center());
    testKitControl->showNormal();
}

void LoginDialog_copy::openTestRedactor(){
    OperatorClickReport::instance()->write(QStringLiteral("Редактор тестирования: открытие окна"));
    TestCreatorControlWindow *testCreatorControlWidget = new TestCreatorControlWindow();
    testCreatorControlWidget->setAttribute(Qt::WA_DeleteOnClose);
    testCreatorControlWidget->setWindowModality(Qt::ApplicationModal);
    testCreatorControlWidget->setWindowTitle("Редактор тестирования");
    testCreatorControlWidget->setWindowIcon(QIcon(":/Images/testEditor.png"));
    testCreatorControlWidget->setFixedSize(QSize(800, 600));
    testCreatorControlWidget->move(QApplication::desktop()->screenGeometry().center() - testCreatorControlWidget->rect().center());
    testCreatorControlWidget->showNormal();
}

void LoginDialog_copy::openExecutorWindow(){

    IOServer::createIOServer(":/DBConfig/Locomotiv/example.xml");
    setBindings();

    TestKitSlectWindow_copy *testKitSelectWindow = new TestKitSlectWindow_copy;
    testKitSelectWindow->setAttribute(Qt::WA_DeleteOnClose);
    testKitSelectWindow->setWindowModality(Qt::ApplicationModal);
    testKitSelectWindow->setWindowTitle("Автоматическое тестирование");
    testKitSelectWindow->setWindowIcon(QIcon(":/Images/graphicPanelEditor.png"));
    testKitSelectWindow->resize(QSize(640, 480));
    testKitSelectWindow->move(QApplication::desktop()->screenGeometry().center() -
                              testKitSelectWindow->rect().center());
    testKitSelectWindow->show();
    testKitSelectWindow->activateWindow();
    testKitSelectWindow->showNormal();
    OperatorClickReport::instance()->write(QStringLiteral("Окно автоматического тестирования: открытие окна"));
}

void LoginDialog_copy::closeExecutorWindow(){

    IOServer::deleteIOServer();

    //widget.close();
}

int LoginDialog_copy:: getUserId(int i){
    int userId = m_userModel->index(i, 0).data().toInt();
    return userId;
};

QString LoginDialog_copy:: getUserName(int i){
    QString userName = m_userModel->index(i, 1).data().toString();
    return userName;
};

int LoginDialog_copy:: getAccessLevel(int i){
    int accessLevel = m_userModel->index(i, 5).data().toInt();
    return accessLevel;
};

QString LoginDialog_copy:: getDBPassword(int i){
    QString dbPassword = m_userModel->index(i, 3).data().toString();
    return dbPassword;
};

QString LoginDialog_copy::getDescription(int i){
    QString description = m_userModel->index(i,4).data().toString();
    return description;
};


LoginDialog_copy *LoginDialog_copy::instance()
{
    static LoginDialog_copy singletone;
    return &singletone;
}

void LoginDialog_copy::setQuitOnLastWindowClosed(bool parameter){
    qApp->setQuitOnLastWindowClosed(parameter);
}

void LoginDialog_copy::setupStylesheet(int theme)
{
    if(getTheme()==1){
         StartDiagnosticWindow_copy::instance()->installStyleSheet(":/Stylesheets/dark-orange.qss");
    }else{
         StartDiagnosticWindow_copy::instance()->installStyleSheet(":/Stylesheets/default.qss");
    }

}

void LoginDialog_copy::setBindings(){
    QObject *QMLObject = MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("startWindow");

    //DO 0

    IVariable *coil_0_0 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO0");

    if (!coil_0_0) {
        return;
    } else {
        connect(coil_0_0, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO0Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO0SwitchChanged(QVariant)), coil_0_0, SLOT(setValue(QVariant)));

    }
    IVariable *coil_0_1 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO1");

    if (!coil_0_1) {
        return;
    } else {
        connect(coil_0_1, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO1Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO1SwitchChanged(QVariant)), coil_0_1, SLOT(setValue(QVariant)));

    }
    IVariable *coil_0_2 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO2");

    if (!coil_0_2) {
        return;
    } else {
        connect(coil_0_2, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO2Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO2SwitchChanged(QVariant)), coil_0_2, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_3 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO3");
    if (!coil_0_3) {
        return;
    } else {
        connect(coil_0_3, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO3Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO3SwitchChanged(QVariant)), coil_0_3, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_4 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO4");
    if (!coil_0_4) {
        return;
    } else {
        connect(coil_0_4, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO4Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO4SwitchChanged(QVariant)), coil_0_4, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_5 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO5");
    if (!coil_0_5) {
        return;
    } else {
        connect(coil_0_5, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO5Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO5SwitchChanged(QVariant)), coil_0_5, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_6 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO6");
    if (!coil_0_6) {
        return;
    } else {
        connect(coil_0_6, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO6Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO6SwitchChanged(QVariant)), coil_0_6, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_7 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO7");
    if (!coil_0_7) {
        return;
    } else {
        connect(coil_0_7, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO7Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO7SwitchChanged(QVariant)), coil_0_7, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_8 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO8");
    if (!coil_0_8) {
        return;
    } else {
        connect(coil_0_8, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO8Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO8SwitchChanged(QVariant)), coil_0_8, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_9 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO9");
    if (!coil_0_9) {
        return;
    } else {
        connect(coil_0_9, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO9Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO9SwitchChanged(QVariant)), coil_0_9, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_10 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO10");
    if (!coil_0_10) {
        return;
    } else {
        connect(coil_0_10, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO10Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO10SwitchChanged(QVariant)), coil_0_10, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_11 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO11");
    if (!coil_0_11) {
        return;
    } else {
        connect(coil_0_11, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO11Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO11SwitchChanged(QVariant)), coil_0_11, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_12 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO12");
    if (!coil_0_12) {
        return;
    } else {
        connect(coil_0_12, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO12Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO12SwitchChanged(QVariant)), coil_0_12, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_13 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO13");
    if (!coil_0_13) {
        return;
    } else {
        connect(coil_0_13, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO13Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO13SwitchChanged(QVariant)), coil_0_13, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_14 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO14");
    if (!coil_0_14) {
        return;
    } else {
        connect(coil_0_14, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO14Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO14SwitchChanged(QVariant)), coil_0_14, SLOT(setValue(QVariant)));

    }

    IVariable *coil_0_15 = IOServer::instance()->variableByAlias("$MODULE_SLOT0_DO15");
    if (!coil_0_15) {
        return;
    } else {
        connect(coil_0_15, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot0DO15Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot0DO15SwitchChanged(QVariant)), coil_0_15, SLOT(setValue(QVariant)));

    }

    //DO 1

    IVariable *coil_1_0 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO0");

    if (!coil_1_0) {
        return;
    } else {
        connect(coil_1_0, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO0Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO0SwitchChanged(QVariant)), coil_1_0, SLOT(setValue(QVariant)));

    }


    IVariable *coil_1_1 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO1");

    if (!coil_1_1) {
        return;
    } else {
        connect(coil_1_1, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO1Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO1SwitchChanged(QVariant)), coil_1_1, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_2 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO2");

    if (!coil_1_2) {
        return;
    } else {
        connect(coil_1_2, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO2Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO2SwitchChanged(QVariant)), coil_1_2, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_3 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO3");

    if (!coil_1_3) {
        return;
    } else {
        connect(coil_1_3, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO3Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO3SwitchChanged(QVariant)), coil_1_3, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_4 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO4");

    if (!coil_1_4) {
        return;
    } else {
        connect(coil_1_4, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO4Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO4SwitchChanged(QVariant)), coil_1_4, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_5 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO5");

    if (!coil_1_5) {
        return;
    } else {
        connect(coil_1_5, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO5Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO5SwitchChanged(QVariant)), coil_1_5, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_6 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO6");

    if (!coil_1_6) {
        return;
    } else {
        connect(coil_1_6, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO6Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO6SwitchChanged(QVariant)), coil_1_6, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_7 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO7");

    if (!coil_1_7) {
        return;
    } else {
        connect(coil_1_7, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO7Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO7SwitchChanged(QVariant)), coil_1_7, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_8 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO8");

    if (!coil_1_8) {
        return;
    } else {
        connect(coil_1_8, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO8Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO8SwitchChanged(QVariant)), coil_1_8, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_9 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO9");

    if (!coil_1_9) {
        return;
    } else {
        connect(coil_1_9, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO9Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO9SwitchChanged(QVariant)), coil_1_9, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_10 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO10");

    if (!coil_1_10) {
        return;
    } else {
        connect(coil_1_10, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO10Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO10SwitchChanged(QVariant)), coil_1_10, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_11 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO11");

    if (!coil_1_11) {
        return;
    } else {
        connect(coil_1_11, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO11Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO11SwitchChanged(QVariant)), coil_1_11, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_12 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO12");

    if (!coil_1_12) {
        return;
    } else {
        connect(coil_1_12, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO12Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO12SwitchChanged(QVariant)), coil_1_12, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_13 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO13");

    if (!coil_1_13) {
        return;
    } else {
        connect(coil_1_13, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO13Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO13SwitchChanged(QVariant)), coil_1_13, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_14 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO14");

    if (!coil_1_14) {
        return;
    } else {
        connect(coil_1_14, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO14Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO14SwitchChanged(QVariant)), coil_1_14, SLOT(setValue(QVariant)));

    }

    IVariable *coil_1_15 = IOServer::instance()->variableByAlias("$MODULE_SLOT1_DO15");

    if (!coil_1_15) {
        return;
    } else {
        connect(coil_1_15, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot1DO15Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot1DO15SwitchChanged(QVariant)), coil_1_15, SLOT(setValue(QVariant)));

    }

    //DO 3
    IVariable *coil_3_0 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO0");

    if (!coil_3_0) {
        return;
    } else {
        connect(coil_3_0, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO0Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO0SwitchChanged(QVariant)), coil_3_0, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_1 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO1");

    if (!coil_3_1) {
        return;
    } else {
        connect(coil_3_1, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO1Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO1SwitchChanged(QVariant)), coil_3_1, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_2 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO2");

    if (!coil_3_2) {
        return;
    } else {
        connect(coil_3_2, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO2Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO2SwitchChanged(QVariant)), coil_3_2, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_3 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO3");

    if (!coil_3_3) {
        return;
    } else {
        connect(coil_3_3, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO3Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO3SwitchChanged(QVariant)), coil_3_3, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_4 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO4");

    if (!coil_3_4) {
        return;
    } else {
        connect(coil_3_4, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO4Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO4SwitchChanged(QVariant)), coil_3_4, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_5 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO5");

    if (!coil_3_5) {
        return;
    } else {
        connect(coil_3_5, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO5Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO5SwitchChanged(QVariant)), coil_3_5, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_6 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO6");

    if (!coil_3_6) {
        return;
    } else {
        connect(coil_3_6, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO6Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO6SwitchChanged(QVariant)), coil_3_6, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_7 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO7");

    if (!coil_3_7) {
        return;
    } else {
        connect(coil_3_7, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO7Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO7SwitchChanged(QVariant)), coil_3_7, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_8 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO8");

    if (!coil_3_8) {
        return;
    } else {
        connect(coil_3_8, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO8Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO8SwitchChanged(QVariant)), coil_3_8, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_9 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO9");

    if (!coil_3_9) {
        return;
    } else {
        connect(coil_3_9, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO9Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO9SwitchChanged(QVariant)), coil_3_9, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_10 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO10");

    if (!coil_3_10) {
        return;
    } else {
        connect(coil_3_10, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO10Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO10SwitchChanged(QVariant)), coil_3_10, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_11 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO11");

    if (!coil_3_11) {
        return;
    } else {
        connect(coil_3_11, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO11Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO11SwitchChanged(QVariant)), coil_3_11, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_12 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO12");

    if (!coil_3_12) {
        return;
    } else {
        connect(coil_3_12, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO12Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO12SwitchChanged(QVariant)), coil_3_12, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_13 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO13");

    if (!coil_3_13) {
        return;
    } else {
        connect(coil_3_13, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO13Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO13SwitchChanged(QVariant)), coil_3_13, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_14 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO14");

    if (!coil_3_14) {
        return;
    } else {
        connect(coil_3_14, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO14Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO14SwitchChanged(QVariant)), coil_3_14, SLOT(setValue(QVariant)));

    }

    IVariable *coil_3_15 = IOServer::instance()->variableByAlias("$MODULE_SLOT3_DO15");

    if (!coil_3_15) {
        return;
    } else {
        connect(coil_3_15, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot3DO15Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot3DO15SwitchChanged(QVariant)), coil_3_15, SLOT(setValue(QVariant)));

    }

    //DO4

    IVariable *coil_4_0 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO0");

    if (!coil_4_0) {
        return;
    } else {
        connect(coil_4_0, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO0Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO0SwitchChanged(QVariant)), coil_4_0, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_1 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO1");

    if (!coil_4_1) {
        return;
    } else {
        connect(coil_4_1, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO1Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO1SwitchChanged(QVariant)), coil_4_1, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_2 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO2");

    if (!coil_4_2) {
        return;
    } else {
        connect(coil_4_2, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO2Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO2SwitchChanged(QVariant)), coil_4_2, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_3 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO3");

    if (!coil_4_3) {
        return;
    } else {
        connect(coil_4_3, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO3Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO3SwitchChanged(QVariant)), coil_4_3, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_4 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO4");

    if (!coil_4_4) {
        return;
    } else {
        connect(coil_4_4, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO4Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO4SwitchChanged(QVariant)), coil_4_4, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_5 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO5");

    if (!coil_4_5) {
        return;
    } else {
        connect(coil_4_5, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO5Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO5SwitchChanged(QVariant)), coil_4_5, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_6 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO6");

    if (!coil_4_6) {
        return;
    } else {
        connect(coil_4_6, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO6Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO6SwitchChanged(QVariant)), coil_4_6, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_7 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO7");

    if (!coil_4_7) {
        return;
    } else {
        connect(coil_4_7, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO7Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO7SwitchChanged(QVariant)), coil_4_7, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_8 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO8");

    if (!coil_4_8) {
        return;
    } else {
        connect(coil_4_8, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO8Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO8SwitchChanged(QVariant)), coil_4_8, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_9 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO9");

    if (!coil_4_9) {
        return;
    } else {
        connect(coil_4_9, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO9Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO9SwitchChanged(QVariant)), coil_4_9, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_10 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO10");

    if (!coil_4_10) {
        return;
    } else {
        connect(coil_4_10, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO10Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO10SwitchChanged(QVariant)), coil_4_10, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_11 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO11");

    if (!coil_4_11) {
        return;
    } else {
        connect(coil_4_11, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO11Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO11SwitchChanged(QVariant)), coil_4_11, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_12 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO12");

    if (!coil_4_12) {
        return;
    } else {
        connect(coil_4_12, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO12Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO12SwitchChanged(QVariant)), coil_4_12, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_13 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO13");

    if (!coil_4_13) {
        return;
    } else {
        connect(coil_4_13, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO13Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO13SwitchChanged(QVariant)), coil_4_13, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_14 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO14");

    if (!coil_4_14) {
        return;
    } else {
        connect(coil_4_14, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO14Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO14SwitchChanged(QVariant)), coil_4_14, SLOT(setValue(QVariant)));

    }

    IVariable *coil_4_15 = IOServer::instance()->variableByAlias("$MODULE_SLOT4_DO15");

    if (!coil_4_15) {
        return;
    } else {
        connect(coil_4_15, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("moduleSlot4DO15Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("checked",variable->value());
        });

        QObject::connect(QMLObject, SIGNAL(moduleSlot4DO15SwitchChanged(QVariant)), coil_4_15, SLOT(setValue(QVariant)));

    }

    //DI 2
    IVariable *discret_2_0 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI0");

    if (!discret_2_0) {
        return;
    } else {
        connect(discret_2_0, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI0Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_1 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI1");

    if (!discret_2_1) {
        return;
    } else {
        connect(discret_2_1, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI1Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_2 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI2");

    if (!discret_2_2) {
        return;
    } else {
        connect(discret_2_2, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI2Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_3 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI3");

    if (!discret_2_3) {
        return;
    } else {
        connect(discret_2_3, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI3Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_4 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI4");

    if (!discret_2_4) {
        return;
    } else {
        connect(discret_2_4, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI4Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_5 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI5");

    if (!discret_2_5) {
        return;
    } else {
        connect(discret_2_5, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI5Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_6 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI6");

    if (!discret_2_6) {
        return;
    } else {
        connect(discret_2_6, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI6Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_7 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI7");

    if (!discret_2_7) {
        return;
    } else {
        connect(discret_2_7, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI7Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_8 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI8");

    if (!discret_2_8) {
        return;
    } else {
        connect(discret_2_8, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI8Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_9 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI9");

    if (!discret_2_9) {
        return;
    } else {
        connect(discret_2_9, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI9Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_10 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI10");

    if (!discret_2_10) {
        return;
    } else {
        connect(discret_2_10, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI10Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_11 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI11");

    if (!discret_2_11) {
        return;
    } else {
        connect(discret_2_11, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI11Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_12 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI12");

    if (!discret_2_12) {
        return;
    } else {
        connect(discret_2_12, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI12Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_13 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI13");

    if (!discret_2_13) {
        return;
    } else {
        connect(discret_2_13, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI13Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_14 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI14");

    if (!discret_2_14) {
        return;
    } else {
        connect(discret_2_14, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI14Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *discret_2_15 = IOServer::instance()->variableByAlias("$MODULE_SLOT2_DI15");

    if (!discret_2_15) {
        return;
    } else {
        connect(discret_2_15, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputSlot2DI15Switch");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }


    //AFC
    IVariable *delErr = IOServer::instance()->variableByAlias("$UZ1_CLR_ALARM");

    if(!delErr){
        return;
    }else{
        connect(delErr,&IVariable::valueChanged,([](IVariable *variable){
           QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("delErrSwitch");
           if(booleanSwitch)
                    booleanSwitch->setProperty("checked",variable->value());
                }));
        QObject::connect(QMLObject, SIGNAL(delErrChanged(QVariant)), delErr, SLOT(setValue(QVariant)));
    }


    IVariable *startStop = IOServer::instance()->variableByAlias("$UZ1_CTRL_DRIVE");

    if(!startStop){
        return;
    }else{
        connect(startStop,&IVariable::valueChanged,([](IVariable *variable){
           QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("afcStartStopSwitch");
           if(booleanSwitch)
                    booleanSwitch->setProperty("checked",variable->value());
                }));
        QObject::connect(QMLObject, SIGNAL(afcStartStopSwitchChanged(QVariant)), startStop, SLOT(setValue(QVariant)));
    }

    IVariable *installedFreequency= IOServer::instance()->variableByAlias("$UZ1_WRITE_FREQ");

    if(!installedFreequency){
        return;
    }else{
        connect(installedFreequency,&IVariable::valueChanged,([](IVariable *variable){
           QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("installedFreequencyInput");
           if(booleanSwitch)
                    booleanSwitch->setProperty("checked",variable->value());
                }));
        QObject::connect(QMLObject, SIGNAL(installedFreequencyInputChanged(QVariant)), installedFreequency, SLOT(setValue(QVariant)));
    }

    IVariable *installedUInput= IOServer::instance()->variableByAlias("$UZ1_WRITE_U");

    if(!installedUInput){
        return;
    }else{
        connect(installedUInput,&IVariable::valueChanged,([](IVariable *variable){
           QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("installedUInput");
           if(booleanSwitch)
                    booleanSwitch->setProperty("checked",variable->value());
                }));
        QObject::connect(QMLObject, SIGNAL(installedUInputChanged(QVariant)), installedUInput, SLOT(setValue(QVariant)));
    }


    //inputs

    IVariable *driveOperationIndicator = IOServer::instance()->variableByAlias("$UZ1_WORK_DRIVE");

    if (!driveOperationIndicator) {
        return;
    } else {
        connect(driveOperationIndicator, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("driveOperationIndicator");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *driveBlockIndicator = IOServer::instance()->variableByAlias("$UZ1_BLOCK_DRIVE");

    if (!driveBlockIndicator) {
        return;
    } else {
        connect(driveBlockIndicator, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("driveBlockIndicator");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *errEEPROMIndicator = IOServer::instance()->variableByAlias("$UZ1_CRC_ERROR");

    if (!errEEPROMIndicator) {
        return;
    } else {
        connect(errEEPROMIndicator, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("errEEPROMIndicator");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *crashCodeIndicator = IOServer::instance()->variableByAlias("$UZ1_CODE_ERROR");

    if (!crashCodeIndicator) {
        return;
    } else {
        connect(crashCodeIndicator, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("crashCodeIndicator");
            if(booleanSwitch)
                booleanSwitch->setProperty("active",variable->value());
        });
    }

    IVariable *crashValueLabel = IOServer::instance()->variableByAlias("$UZ1_ERROR_DESCRIPTION");

    if (!crashValueLabel) {
        return;
    } else {
        connect(crashValueLabel, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("crashValueLabel");
            if(booleanSwitch)
                booleanSwitch->setProperty("text",variable->value());
        });
    }

    IVariable *inputFreequencyLabel = IOServer::instance()->variableByAlias("$UZ1_READ_FREQ");

    if (!inputFreequencyLabel) {
        return;
    } else {
        connect(inputFreequencyLabel, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputFreequencyLabel");
            if(booleanSwitch)
                booleanSwitch->setProperty("text",floor(variable->value().toFloat()*10)/10);
        });
    }

    IVariable *inputULabel = IOServer::instance()->variableByAlias("$UZ1_READ_U");

    if (!inputULabel) {
        return;
    } else {
        connect(inputULabel, &IVariable::valueChanged, [](IVariable *variable){
            QObject *booleanSwitch= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("inputULabel");
            if(booleanSwitch)
                booleanSwitch->setProperty("text",variable->value());
        });
    }
//ещё вернусь
//    IVariable *hold_00 = IOServer::instance()->variableByAlias("$Holding_01");

//    if (!hold_00) {
//        return;
//    } else {

//        connect(hold_00, &IVariable::valueChanged, [](IVariable *variable) {
//            QObject *integerGauge= MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("Speedometer");
//            if (integerGauge)
//                integerGauge->setProperty("value", variable->value());
//        });

//        connect(hold_00,&IVariable::valueChanged,[](IVariable *variable){
//            QObject *integerSlider = MainEngine::instance()->engine().rootObjects().first()->findChild<QObject*>("speedSlider");
//            if(integerSlider)
//                integerSlider->setProperty("value",variable->value());
//        });

//        QObject::connect(QMLObject, SIGNAL(speedSliderChanged(QVariant)), hold_00, SLOT(setValue(QVariant)));
//    }

}

//void LoginDialog_copy::showMessage(const QString &title, const QString &text, const QString &informativeText, QMessageBox::Icon icon) const
//{
//    QMessageBox msgBox;
//    msgBox.setIcon(icon);
//    msgBox.setWindowTitle(title);
//    msgBox.setText(text);
//    msgBox.setInformativeText(informativeText);

//    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
//    if (layout)
//        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

//    msgBox.setMinimumWidth(400);
//    msgBox.setDefaultButton(QMessageBox::No);
//    msgBox.exec();
//}

//bool LoginDialog_copy::checkUnfilledFields()
//{
//    if (m_nameComboBox->currentIndex() == -1) {
//        ErrorClassifier::instance()->showError(1);
//        return false;
//    }

//    return true;
//}
//void LoginDialog_copy::save()
//{
//    //OperatorClickReport::instance()->write("Окно авторизации пользователя. Нажата клавиша ОК");

//    if (!checkUnfilledFields())
//        return;

//    switch (m_enablePasswordChange) {
//        case true:
//            updatePassword();
//            break;
//        case false:
//            authorization();
//            break;
//    }
//}

//void LoginDialog_copy::cancel()
//{
//    //OperatorClickReport::instance()->write("Окно авторизации пользователя. Нажата клавиша Отмена");

//    switch (m_enablePasswordChange) {
//        case true:

//            clearUserInputs();
//            break;
//        case false: {

//            SystemUser::instance()->setUser("Гость");
//            SystemUser::instance()->setAccess(ACCESSLEVEL_USER);

//            StartWindow *startWindow = new StartWindow;
//            startWindow->setAttribute(Qt::WA_DeleteOnClose);
//            startWindow->setWindowModality(Qt::NonModal);

//            startWindow->setWindowTitle(titleString);

//            startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
//            startWindow->resize(QSize(1024, 768));
//            startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
//            startWindow->show();
//            startWindow->activateWindow();

//           // close();

//        }
//    }
//}

//void LoginDialog_copy::authorization()
//{
//    QString lineEditPassword = QString(QCryptographicHash::hash((m_passwordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
//    QString databasePassword = m_userModel->index(m_nameComboBox->currentIndex(), 3).data().toString();

//    if (lineEditPassword != databasePassword) {
//        ErrorClassifier::instance()->showError(2);
//        return;
//    }

//    setSystemAccessLevel(m_userModel->index(m_nameComboBox->currentIndex(), 5).data().toInt());

//    StartWindow *startWindow = new StartWindow;
//    startWindow->setAttribute(Qt::WA_DeleteOnClose);
//    startWindow->setWindowModality(Qt::NonModal);

//    startWindow->setWindowTitle(titleString);

//    startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
//    startWindow->resize(QSize(1024, 768));
//    startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
//    startWindow->show();
//    startWindow->activateWindow();

//    //close();
//}

//void LoginDialog_copy::updatePassword()
//{
//    QString lineEditPassword = QString(QCryptographicHash::hash((m_passwordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
//    QString databasePassword = m_userModel->index(m_nameComboBox->currentIndex(), 3).data().toString();

//    if (lineEditPassword != databasePassword) {
//        ErrorClassifier::instance()->showError(2);
//        return;
//    }

//    if (m_newPasswordLineEdit->text() != m_confirmNewPasswordLineEdit->text()) {
//        ErrorClassifier::instance()->showError(3);
//        return;
//    }

//    if (m_newPasswordLineEdit->text().isEmpty()) {
//        QMessageBox msgBox;
//        msgBox.setWindowTitle("Ввод данных");
//        QString msgString = tr("Вы действительно хотите оставить поле пароль пустым?");
//        msgBox.setText(msgString);
//        msgBox.setIcon(QMessageBox::Warning);

//        QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
//        QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
//        msgBox.setDefaultButton(noButton);

//        msgBox.exec();

//        if (msgBox.clickedButton() == yesButton) {
//            OperatorClickReport::instance()->write("Окно авторизации пользователя. Вы действительно хотите оставить поле пароль пустым? Нажата клавиша ДА");
//            msgBox.close();
//        } else if (msgBox.clickedButton() == noButton) {
//            OperatorClickReport::instance()->write("Окно авторизации пользователя. Вы действительно хотите оставить поле пароль пустым? Нажата клавиша НЕТ");
//            return;
//        }
//    }

//    int userId = m_userModel->index(m_nameComboBox->currentIndex(), 0).data().toInt();
//    QString newPassword = QString(QCryptographicHash::hash((m_newPasswordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());

//    QString sqlString = QString("UPDATE `Users` SET password = '%1' WHERE id = %2;")
//                        .arg(newPassword)
//                        .arg(userId);

//    QSqlDatabase db = DBConfig::instance()->configDatabase();
//    QSqlQuery query(db);

//    db.transaction();

//    if (query.exec(sqlString)) {

//        if (!db.commit()){
//            db.rollback();
//        }

//        showMessage(tr("Ввод данных"), tr("Пароль успешно изменен авторизуйтесь повторно"));
//        m_nameComboBox->setCurrentIndex(-1);
//        selectData();
//        OperatorClickReport::instance()->write(QStringLiteral("Авторизация: изменение пароля"));
//        return;

//    }  else {
//        ErrorClassifier::instance()->showError(501);
//    }

//}

//void LoginDialog_copy::clearUserInputs()
//{
//    m_passwordLineEdit->clear();
//    m_newPasswordLineEdit->clear();
//    m_confirmNewPasswordLineEdit->clear();

//    m_enablePasswordChange = false;
//    m_newPasswordLabel->setVisible(m_enablePasswordChange);
//    m_newPasswordLineEdit->setVisible(m_enablePasswordChange);
//    m_confirmNewPasswordLabel->setVisible(m_enablePasswordChange);
//    m_confirmNewPasswordLineEdit->setVisible(m_enablePasswordChange);

//}

//void LoginDialog_copy::updateUserIconAccessLevel()
//{
//    int accessLevel = m_userModel->index(m_nameComboBox->currentIndex(), 5).data().toInt();

//}

//void LoginDialog_copy::setSystemAccessLevel(int accessLevel)
//{
//    SystemUser *sysUser = SystemUser::instance();
//    sysUser->setUser(m_userModel->index(m_nameComboBox->currentIndex(), 1).data().toString());
//    sysUser->setAccess(m_userModel->index(m_nameComboBox->currentIndex(), 5).data().toInt());
//    sysUser->setDescription(m_userModel->index(m_nameComboBox->currentIndex(), 4).data().toString());
//    OperatorClickReport::instance()->write(QString("Авторизация: вход пользователя %1").arg(sysUser->user()));
//}

//void LoginDialog_copy::closeEvent(QCloseEvent *event)
//{
//    OperatorClickReport::instance()->write(QStringLiteral("Авторизация: закрытие окна"));

//    QWidget::closeEvent(event);
//}

//void LoginDialog_copy::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_D && (event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))) {
//        m_nameComboBox->setCurrentText(tr("Администратор"));
//        m_passwordLineEdit->setText("admin");
//        authorization();
//    }
//    QWidget::keyPressEvent(event);
//}
