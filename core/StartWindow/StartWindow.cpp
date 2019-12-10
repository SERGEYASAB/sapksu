#include "StartWindow.h"
#include "ActionStyledFlowLayout.h"
#include "ActionStyledWidget.h"
#include "ActionStyledGroupMenuWidget.h"
#include "ActionControlWidgetsShow.h"
#include "Authentication/LoginDialog.h"
#include "Authentication/SystemUser.h"
#include "DatabaseDirectory/Users/Users.h"
#include "DatabaseDirectory/Classifier.h"
#include "TestManager/TestCreatorControlWindow/TestCreatorControlWindow.h"
#include "TestManager/TestKitConfigureWindow/TestKitConfigureWindow.h"
#include "SelectKsuWindow/SelectKsuWindow.h"
#include "Scene/SceneControlPanel.h"
#include "IOServer/IOServer.h"
#include "Devices/DevicesWindow.h"
#include "StartDiagnosticWindow.h"
#include "Attestation/AttestationWindow.h"
#include "ReportManager/ReportConfigure.h"
#include "ReportManager/OperatorClickReport.h"
#include "StartWindow/StartWindowTesting.h"
#include "Attestation/AttestationMechosWidget.h"
#include "SelectKsuWindow/KsuInfoWindow.h"

StartWindow::StartWindow(QWidget *parent)  : QWidget(parent)
{
    m_currentUserAction = new QAction(this);
    m_configUsersAction = new QAction(this);
    m_referenceSettingsKSUAction = new QAction(this);
    m_processSqliteAction = new QAction(this);
    m_testEditorAction = new QAction(this);
    m_testKitControlAction = new QAction(this);
    m_selectKsuAction = new QAction(this);
    m_graphicPanelAction = new QAction(this);
    m_devicesAction = new QAction(this);
    m_setupStylesheetAction = new QAction(this);

#ifdef SAP_KSU
    m_attestationAction = new QAction(this);
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
    m_attestationAction = new QAction(this);
#endif

    m_reportConfigureAction = new QAction(this);
    m_uploadConfigAction = new QAction(this);

    connect(m_currentUserAction, &QAction::triggered, this, &StartWindow::currentUser);
    connect(m_configUsersAction, &QAction::triggered, this, &StartWindow::configUsers);
    connect(m_referenceSettingsKSUAction, &QAction::triggered, this, &StartWindow::referenceSettingsKSU);
    connect(m_processSqliteAction, &QAction::triggered, this, &StartWindow::processSqlite);
    connect(m_testEditorAction, &QAction::triggered, this, &StartWindow::testCreatorControl);
    connect(m_testKitControlAction, &QAction::triggered, this, &StartWindow::testKitControl);
    connect(m_selectKsuAction, &QAction::triggered, this, &StartWindow::selectKsu);
    connect(m_graphicPanelAction, &QAction::triggered, this, &StartWindow::graphicsPanel);
    connect(m_devicesAction, &QAction::triggered, this, &StartWindow::devices);
    connect(m_setupStylesheetAction, &QAction::triggered, this, &StartWindow::setupStylesheet);

#ifdef SAP_KSU
    connect(m_attestationAction, &QAction::triggered, this, &StartWindow::openAttestationWidget);
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
    connect(m_attestationAction, &QAction::triggered, this, &StartWindow::openAttestationWidget);
#endif

    connect(m_reportConfigureAction, &QAction::triggered, this, &StartWindow::reportConfigure);
    connect(m_uploadConfigAction, &QAction::triggered, this, &StartWindow::uploadConfig);

    m_currentUserWidget = new ActionStyledWidget(ACCESSLEVEL_ZERO,
                                                 m_currentUserAction,
                                                 new QLabel("Login/Logout"),
                                                 QIcon(":/Images/access_user.png"));

    m_configUsersWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                 m_configUsersAction,
                                                 new QLabel("Управление \r\n пользователями"),
                                                 QIcon(":/Images/configUsers.png"));

    m_referenceSettingsKSUWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                          m_referenceSettingsKSUAction,
                                                          new QLabel("Справочники \r\n КСУ"),
                                                          QIcon(":/Images/referenceSettingsKSU.png"));
    m_processSqliteWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                   m_processSqliteAction,
                                                   new QLabel("База данных \r\n SQLITE"),
                                                   QIcon(":/Images/processSqlite.png"));

    m_testCreatorControlWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                        m_testEditorAction,
                                                        new QLabel("Редактор \r\n тестирования"),
                                                        QIcon(":/Images/testEditor.png"));

    m_testKitControlWidget = new ActionStyledWidget(ACCESSLEVEL_ENGINEER,
                                                    m_testKitControlAction,
                                                    new QLabel("Управление \r\n наборами тестов"),
                                                    QIcon(":/Images/testKitControl.png"));

    m_selectKsuWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                               m_selectKsuAction,
                                               new QLabel("Тестирование"),
                                               QIcon(":/Images/testSettings.png"));

    m_devicesWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                             m_devicesAction,
                                             new QLabel("Оборудование \r\n стенда"),
                                             QIcon(":/Images/devices.png"));


    m_setupStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                     m_setupStylesheetAction,
                                                     new QLabel("Настройка \r\n стилей"),
                                                     QIcon(":/Images/stylesheets.png"));

#ifdef SAP_KSU
    m_attestationWidget = new ActionStyledWidget(ACCESSLEVEL_ENGINEER,
                                                 m_attestationAction,
                                                 new QLabel("Аттестация"),
                                                 QIcon(":/Images/attestation.png"));

    m_graphicPanelWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                  m_graphicPanelAction,
                                                  new QLabel("Редактор \r\n графических \r\n панелей"),
                                                  QIcon(":/Images/graphicPanelEditor.png"));

#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
    m_attestationWidget = new ActionStyledWidget(ACCESSLEVEL_ENGINEER,
                                                 m_attestationAction,
                                                 new QLabel("Аттестация"),
                                                 QIcon(":/Images/attestation.png"));
#endif

    m_reportConfigureWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                     m_reportConfigureAction,
                                                     new QLabel("Отчеты"),
                                                     QIcon(":/Images/report.png"));

    m_uploadConfiWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                 m_uploadConfigAction,
                                                 new QLabel("Загрузка конфигурации"),
                                                 QIcon(":/Images/report.png"));

    ActionStyledFlowLayout *group1Layout = new ActionStyledFlowLayout;
    group1Layout->addWidget(m_devicesWidget);
    group1Layout->addWidget(m_currentUserWidget);
    group1Layout->addWidget(m_configUsersWidget);
    group1Layout->addWidget(m_setupStylesheetWidget);

    ActionStyledFlowLayout *group2Layout = new ActionStyledFlowLayout;
    group2Layout->addWidget(m_referenceSettingsKSUWidget);
    group2Layout->addWidget(m_processSqliteWidget);
    group2Layout->addWidget(m_selectKsuWidget);
    group2Layout->addWidget(m_testKitControlWidget);
    group2Layout->addWidget(m_testCreatorControlWidget);

#ifdef SAP_KSU
    group2Layout->addWidget(m_graphicPanelWidget);
    group2Layout->addWidget(m_attestationWidget);
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
    group2Layout->addWidget(m_attestationWidget);
#endif

    group2Layout->addWidget(m_reportConfigureWidget);

#ifdef ST_BP_M5
    group2Layout->addWidget(m_uploadConfiWidget);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Главное меню")));
    mainLayout->addLayout(group1Layout);
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Настройка оборудования")));
    mainLayout->addLayout(group2Layout);
    mainLayout->addStretch();

#ifdef SAP_KSU
    mainLayout->addWidget(new QLabel(titleString));
#endif
#ifdef SAP_SU
    mainLayout->addWidget(new QLabel(titleString));
#endif
#ifdef MECHOS
    mainLayout->addWidget(new QLabel(titleString));
#endif


    setLayout(mainLayout);
}

void StartWindow::currentUser()
{
    close();

    LoginDialog *loginDialog = new LoginDialog;
    loginDialog->setAttribute(Qt::WA_DeleteOnClose);
    loginDialog->setWindowFlags(Qt::WindowTitleHint);
    loginDialog->setWindowModality(Qt::ApplicationModal);
    loginDialog->setWindowTitle("Авторизация пользователя");
    loginDialog->setWindowIcon(QIcon(":/Images/access_user.png"));
    loginDialog->setFixedSize(QSize(370, 370));
    loginDialog->move(QApplication::desktop()->screenGeometry().center() - loginDialog->rect().center());
    loginDialog->showNormal();
    OperatorClickReport::instance()->write(QStringLiteral("Авторизация: открытие окна"));
    ActionControlWidgetsShow::showAdjustWidget(this, loginDialog, QSize(370, 370));
}

void StartWindow::configUsers()
{
    setWindowState(Qt::WindowMinimized);

    Users *configUsers = new Users();
    configUsers->setAttribute(Qt::WA_DeleteOnClose);
    configUsers->setWindowModality(Qt::ApplicationModal);
    configUsers->setWindowTitle("Управление пользователями");
    configUsers->setWindowIcon(QIcon(":/Images/access_user.png"));

    ActionControlWidgetsShow::showAdjustWidget(this, configUsers, QSize(800, 600));
}

void StartWindow::referenceSettingsKSU()
{
    setWindowState(Qt::WindowMinimized);

    Classifier *referenceSettings = new Classifier();
    referenceSettings->setAttribute(Qt::WA_DeleteOnClose);
    referenceSettings->setWindowModality(Qt::ApplicationModal);
    referenceSettings->setWindowTitle("Справочники КСУ");
    referenceSettings->setWindowIcon(QIcon(":/Images/referenceSettingsKSU.png"));
    OperatorClickReport::instance()->write(QStringLiteral("Справочники КСУ: открытие окна"));
    ActionControlWidgetsShow::showAdjustWidget(this, referenceSettings, QSize(1024, 768));

}

void StartWindow::processSqlite()
{
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Нажата кнопка \"Открыть базу данных SQLite\""));
    QProcess *process = new QProcess(this);
    QString file = SQLITE_PATH;
    process->start(file);

    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &StartWindow::showNormal);
    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), process, &QProcess::deleteLater);
    connect(process, &QProcess::errorOccurred, this, &StartWindow::showNormal);
    connect(process, &QProcess::errorOccurred, this, &StartWindow::deleteLater);
}

void StartWindow::testCreatorControl()
{
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Редактор тестирования: открытие окна"));
    TestCreatorControlWindow *testCreatorControlWidget = new TestCreatorControlWindow();
    testCreatorControlWidget->setAttribute(Qt::WA_DeleteOnClose);
    testCreatorControlWidget->setWindowModality(Qt::ApplicationModal);
    testCreatorControlWidget->setWindowTitle("Редактор тестирования");
    testCreatorControlWidget->setWindowIcon(QIcon(":/Images/testEditor.png"));

    ActionControlWidgetsShow::showAdjustWidget(this, testCreatorControlWidget, QSize(800, 600));
}

void StartWindow::testKitControl()
{
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Управление наборами тестов: открытие окна"));
    TestKitConfigureWindow *testKitControl = new TestKitConfigureWindow();
    testKitControl->setAttribute(Qt::WA_DeleteOnClose);
    testKitControl->setWindowModality(Qt::ApplicationModal);
    testKitControl->setWindowTitle("Управление наборами");
    testKitControl->setWindowIcon(QIcon(":/Images/testKitControl.png"));

    ActionControlWidgetsShow::showAdjustWidget(this, testKitControl, QSize(800, 600));
}

void StartWindow::selectKsu()
{
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Окно настройки тестирования: открытие окна"));
    SelectKsuWindow *selectKsu = new SelectKsuWindow();
    selectKsu->setAttribute(Qt::WA_DeleteOnClose);
    selectKsu->setWindowModality(Qt::ApplicationModal);
    selectKsu->setFixedSize(QSize(340, 600));
    selectKsu->setWindowTitle("Настройка тестирования");
    selectKsu->setWindowIcon(QIcon(":/Images/testSettings.png"));
    selectKsu->move(QApplication::desktop()->screenGeometry().center() - selectKsu->rect().center());
    if (selectKsu->exec() == QDialog::Rejected) {
        showNormal();
        activateWindow();
        return;
    }

    KsuInfoWindow *infoWindow = new KsuInfoWindow();
    infoWindow->setAttribute(Qt::WA_DeleteOnClose);
    infoWindow->setWindowTitle(tr("Информация об оборудовании"));
    infoWindow->setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    infoWindow->activateWindow();
    infoWindow->exec();

    StartWindowTesting *testWindow = new StartWindowTesting;
    testWindow->setAttribute(Qt::WA_DeleteOnClose);
    testWindow->setWindowModality(Qt::NonModal);
    testWindow->setWindowTitle(titleString);
    testWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    testWindow->resize(QSize(1024, 768));
    testWindow->move(QApplication::desktop()->screenGeometry().center() - testWindow->rect().center());
    testWindow->show();
    testWindow->activateWindow();
    close();
}

void StartWindow::graphicsPanel()
{
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Окно редактора графических панелей: открытие окна"));
    SceneControlPanel *graphicPanel = new SceneControlPanel(SceneControlPanel::RunMode::Editing);
    graphicPanel->setAttribute(Qt::WA_DeleteOnClose);
    graphicPanel->setWindowModality(Qt::ApplicationModal);
    graphicPanel->setWindowTitle("Редактор графических панелей");
    graphicPanel->setWindowIcon(QIcon(":/Images/graphicPanelEditor.png"));
    graphicPanel->resize(800, 600);
    graphicPanel->move(QApplication::desktop()->screenGeometry().center() - graphicPanel->rect().center());
    graphicPanel->show();
    graphicPanel->activateWindow();

}

void StartWindow::devices()
{

    QMessageBox msgBox;

    msgBox.setWindowTitle("Питание контроллера КСУ");
    msgBox.setText("ВНИМАНИЕ!");

#ifdef SAP_KSU
    msgBox.setInformativeText(QString("Включение оборудования стенда\n"
                                      "Вы действительно хотите продолжить?"));
#endif
#ifdef SAP_SU
    msgBox.setInformativeText(QString("На станцию управления будет подано питающее напряжение.\n"
                                      "Убедитесь в правильности подключения станции управления.\n"
                                      "Вы действительно хотите продолжить?"));
#endif
#ifdef MECHOS
    msgBox.setInformativeText(QString("На станцию управления будет подано питающее напряжение.\n"
                                      "Убедитесь в правильности подключения станции управления.\n"
                                      "Вы действительно хотите продолжить?"));
#endif

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(400);
    msgBox.setDefaultButton(QMessageBox::No);

    QPushButton *okButton = msgBox.addButton("Да", QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == noButton) {
        return;
    }

    close();

    IOServer::createIOServer();

    DevicesWindow *devicesWindow = new DevicesWindow;
    devicesWindow->setAttribute(Qt::WA_DeleteOnClose);
    devicesWindow->setWindowModality(Qt::NonModal);

    devicesWindow->setWindowTitle(titleString);

    devicesWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    //    devicesWindow->resize(QSize(1024, 768));
    //    devicesWindow->move(QApplication::desktop()->screenGeometry().center() - devicesWindow->rect().center());
    //    devicesWindow->show();
    devicesWindow->showMaximized();
    devicesWindow->activateWindow();
    OperatorClickReport::instance()->write(QStringLiteral("Окно оборудования стенда: открытие окна"));
}

void StartWindow::setupStylesheet()
{
    StartDiagnosticWindow::instance()->setupStyleSheet();
}

void StartWindow::openAttestationWidget()
{
#ifdef SAP_KSU
    close();
    IOServer::createIOServer();

    AttestationWindow *attestationWindow = new AttestationWindow;
    attestationWindow->setAttribute(Qt::WA_DeleteOnClose);
    attestationWindow->setWindowModality(Qt::NonModal);
    attestationWindow->setWindowTitle("Аттестация");
    attestationWindow->setWindowIcon(QIcon(":/Images/attestation.png"));
    attestationWindow->resize(QSize(1024, 768));
    attestationWindow->move(QApplication::desktop()->screenGeometry().center() - attestationWindow->rect().center());
    attestationWindow->show();
    attestationWindow->activateWindow();
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: открытие окна"));
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Окно настройки тестирования: открытие окна"));
    SelectKsuWindow *selectKsu = new SelectKsuWindow();
    selectKsu->setAttribute(Qt::WA_DeleteOnClose);
    selectKsu->setWindowModality(Qt::ApplicationModal);
    selectKsu->setFixedSize(QSize(340, 600));
    selectKsu->setWindowTitle("Настройка тестирования");
    selectKsu->setWindowIcon(QIcon(":/Images/testSettings.png"));
    selectKsu->move(QApplication::desktop()->screenGeometry().center() - selectKsu->rect().center());
    if (selectKsu->exec() == QDialog::Rejected) {
        showNormal();
        activateWindow();
        return;
    }

    AttestationMechosWidget *attestationWindow = new AttestationMechosWidget;
    attestationWindow->setAttribute(Qt::WA_DeleteOnClose);
    attestationWindow->setWindowModality(Qt::NonModal);
    attestationWindow->setWindowTitle("Аттестация");
    attestationWindow->setWindowIcon(QIcon(":/Images/attestation.png"));
    attestationWindow->resize(QSize(1500, 800));
    attestationWindow->move(QApplication::desktop()->screenGeometry().center() - attestationWindow->rect().center());
    attestationWindow->show();
    attestationWindow->activateWindow();
    OperatorClickReport::instance()->write(QStringLiteral("Аттестация: открытие окна"));
#endif



}

void StartWindow::reportConfigure()
{
    setWindowState(Qt::WindowMinimized);
    OperatorClickReport::instance()->write(QStringLiteral("Окно отчетов: открытие окна"));
    ReportConfigure *reportConfig = new ReportConfigure();
    reportConfig->setAttribute(Qt::WA_DeleteOnClose);
    reportConfig->setWindowModality(Qt::ApplicationModal);
    reportConfig->setWindowTitle("Отчеты");
    reportConfig->setWindowIcon(QIcon(":/Images/report.png"));

    ActionControlWidgetsShow::showAdjustWidget(this, reportConfig, QSize(800, 600), true);
}

void StartWindow::uploadConfig()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    tr("Config (*.xml)"));
    IOServer::createIOServer(fileName);

    close();

    StartWindowTesting *testWindow = new StartWindowTesting;
    testWindow->setAttribute(Qt::WA_DeleteOnClose);
    testWindow->setWindowModality(Qt::NonModal);

    testWindow->setWindowTitle(titleString);

    testWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    testWindow->resize(QSize(1024, 768));
    testWindow->move(QApplication::desktop()->screenGeometry().center() - testWindow->rect().center());
    testWindow->show();
    testWindow->activateWindow();
}
