#include "StartWindowTesting.h"
#include "StartWindow/ActionStyledFlowLayout.h"
#include "StartWindow/ActionStyledWidget.h"
#include "StartWindow/ActionStyledGroupMenuWidget.h"
#include "StartWindow/ActionControlWidgetsShow.h"
#include "StartWindow/StartWindow.h"
#include "Devices/DevicesTabWidget.h"
#include "Devices/EnergoFormDevice/EnergoFormRunningControl.h"
#include "ManualTesting/KsuManualTestingWidget.h"
#include "TestManager/TestKitConfigureWindow/TestKitConfigureWindow.h"
#include "Scene/SceneControlPanel.h"
#include "TestManager/TestKitSelectWindow/TestKitSlectWindow.h"
#include "TestManager/TestCreatorControlWindow/TestCreatorControlWindow.h"
#include "IOServer/IOServer.h"
#include "Authentication/SystemUser.h"
#include "ReportManager/OperatorClickReport.h"
#include "UpdateKsuWizard/UpdateKsuWizard.h"
#include "ReportManager/ReportConfigure.h"
#include "DatabaseDirectory/Classifier.h"

StartWindowTesting::StartWindowTesting(QWidget *parent) : QWidget(parent)
{
    m_directlogicAction = new QAction(this);
    m_voltmetrAction = new QAction(this);
    m_frecurencyConvertorAction = new QAction(this);
    m_nlRizAction = new QAction(this);
    m_mktnAction = new QAction(this);
    m_energoFormAction = new QAction(this);
    m_upsAction = new QAction(this);
    m_manualTestModeAction = new QAction(this);
    m_stepTestModeAction = new QAction(this);
    m_autoTestModeAction = new QAction(this);
    m_graphicPanelAction = new QAction(this);
    m_updateKsuAction = new QAction(this);
    m_reportConfigureAction = new QAction(this);
    m_testEditorAction = new QAction(this);
    m_testKitControlAction = new QAction(this);
    m_referenceSettingsKSUAction = new QAction(this);

    connect(m_directlogicAction, &QAction::triggered, [this]() { devicesTabWidget(0); });
    connect(m_voltmetrAction, &QAction::triggered, [this]() { devicesTabWidget(2); });
    connect(m_frecurencyConvertorAction, &QAction::triggered, [this]() { devicesTabWidget(1); });
    connect(m_nlRizAction, &QAction::triggered, [this]() { devicesTabWidget(3); });
    connect(m_mktnAction, &QAction::triggered, [this]() { devicesTabWidget(4); });
    connect(m_upsAction, &QAction::triggered, [this]() { devicesTabWidget(5); });
    connect(m_testEditorAction, &QAction::triggered, this, &StartWindowTesting::testCreatorControl);
    connect(m_energoFormAction, &QAction::triggered, this, &StartWindowTesting::energoForm);
    connect(m_manualTestModeAction, &QAction::triggered, this, &StartWindowTesting::manualTestMode);
    connect(m_stepTestModeAction, &QAction::triggered, this, &StartWindowTesting::stepTestMode);
    connect(m_autoTestModeAction, &QAction::triggered, this, &StartWindowTesting::autoTestMode);
    connect(m_graphicPanelAction, &QAction::triggered, this, &StartWindowTesting::graphicsPanel);
    connect(m_updateKsuAction, &QAction::triggered, this, &StartWindowTesting::updateksu);
    connect(m_reportConfigureAction, &QAction::triggered, this, &StartWindowTesting::reportConfigure);
    connect(m_testKitControlAction, &QAction::triggered, this, &StartWindowTesting::testKitControl);
    connect(m_referenceSettingsKSUAction, &QAction::triggered, this, &StartWindowTesting::referenceSettingsKSU);


    m_directLogicWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR, m_directlogicAction,
                                                 new QLabel("Контроллер \r\n Directlogic "),
                                                 QIcon(":/Images/gnome-network-properties.png"));

    m_voltmetrWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                              m_voltmetrAction,
                                              new QLabel("Вольтметры"),
                                              QIcon(":/Images/voltmetr.png"));

    m_frecurencyConvertorWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                         m_frecurencyConvertorAction,
                                                         new QLabel("Частотные \r\n преобразователи"),
                                                         QIcon(":/Images/frecurencyConvertor.png"));

    m_mktnWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                          m_mktnAction,
                                          new QLabel("Плата \r\n МКТН"),
                                          QIcon(":/Images/mktn.png"));

    m_nlRizActionWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                 m_nlRizAction,
                                                 new QLabel("NL-16DO \r\n R - изоляции"),
                                                 QIcon(":/Images/nl16d0.png"));

    m_energoFormWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                m_energoFormAction,
                                                new QLabel("Энергоформа "),
                                                QIcon(":/Images/energoForm.png"));

    m_upsWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                         m_upsAction,
                                         new QLabel("ИБП"),
                                         QIcon(":/Images/powerSupply.png"));

    m_manualTestModeWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                    m_manualTestModeAction,
                                                    new QLabel("Ручное \r\n тестирование"),
                                                    QIcon(":/Images/manual_testing.png"));

    m_stepTestModeWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                  m_stepTestModeAction,
                                                  new QLabel("Пошаговое \r\n тестирование"),
                                                  QIcon(":/Images/stepTesting.png"));

    m_autoTestModeWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                  m_autoTestModeAction,
                                                  new QLabel("Автоматическое \r\n тестирование"),
                                                  QIcon(":/Images/automaticTesting.png"));

    m_graphicPanelWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                  m_graphicPanelAction,
                                                  new QLabel("Редактор \r\n графических \r\n панелей"),
                                                  QIcon(":/Images/graphicPanelEditor.png"));

    m_updateKsuWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                               m_updateKsuAction,
                                               new QLabel("Проверка \r\n перепрограммирования \r\n КСУ"),
                                               QIcon(":/Images/updateksu.png"));

    m_reportConfigureWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                     m_reportConfigureAction,
                                                     new QLabel("Отчеты"),
                                                     QIcon(":/Images/report.png"));

    m_testCreatorControlWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                        m_testEditorAction,
                                                        new QLabel("Редактор \r\n тестирования"),
                                                        QIcon(":/Images/testEditor.png"));
    m_testKitControlWidget = new ActionStyledWidget(ACCESSLEVEL_ENGINEER,
                                                    m_testKitControlAction,
                                                    new QLabel("Управление \r\n наборами тестов"),
                                                    QIcon(":/Images/testKitControl.png"));

    m_referenceSettingsKSUWidget = new ActionStyledWidget(ACCESSLEVEL_ADMIN,
                                                          m_referenceSettingsKSUAction,
                                                          new QLabel("Справочники \r\n КСУ"),
                                                          QIcon(":/Images/referenceSettingsKSU.png"));

    ActionStyledFlowLayout *devicesFlowLayout = new ActionStyledFlowLayout();
    devicesFlowLayout->addWidget(m_directLogicWidget);
    devicesFlowLayout->addWidget(m_voltmetrWidget);
    devicesFlowLayout->addWidget(m_frecurencyConvertorWidget);
    devicesFlowLayout->addWidget(m_mktnWidget);

#ifdef SAP_KSU
    devicesFlowLayout->addWidget(m_nlRizActionWidget);
    devicesFlowLayout->addWidget(m_energoFormWidget);
    devicesFlowLayout->addWidget(m_upsWidget);
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
#endif

    ActionStyledFlowLayout *testingFlowLayout = new ActionStyledFlowLayout();

    testingFlowLayout->addWidget(m_manualTestModeWidget);
    testingFlowLayout->addWidget(m_autoTestModeWidget);
    testingFlowLayout->addWidget(m_updateKsuWidget);
    testingFlowLayout->addWidget(m_reportConfigureWidget);
    testingFlowLayout->addWidget(m_testCreatorControlWidget);
    testingFlowLayout->addWidget(m_testKitControlWidget);
    testingFlowLayout->addWidget(m_referenceSettingsKSUWidget);

    ActionStyledFlowLayout *graphicPanelFlowLayout = new ActionStyledFlowLayout();
    graphicPanelFlowLayout->addWidget(m_graphicPanelWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout();

    // Группа меню - оборудование стенда
    if (SystemUser::instance()->access() >= ACCESSLEVEL_ADMIN) {
        mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Оборудование стенда")));
        mainLayout->addLayout(devicesFlowLayout);
    }

    // Группа меню - тестирование стенда
#ifdef SAP_KSU
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Тестирование КСУ")));
#endif
#ifdef SAP_SU
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Тестирование СУ")));
#endif
#ifdef MECHOS
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Тестирование СУ")));
#endif

    mainLayout->addLayout(testingFlowLayout);

    // Группа меню - графические панели
#ifdef SAP_KSU
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Графические панели")));
    mainLayout->addLayout(graphicPanelFlowLayout);
#endif
#ifdef SAP_SU
    //mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Графические панели")));
    //mainLayout->addLayout(graphicPanelFlowLayout);
#endif
#ifdef MECHOS
    //mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Графические панели")));
    //mainLayout->addLayout(graphicPanelFlowLayout);
#endif

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

StartWindowTesting::~StartWindowTesting()
{
    delete m_devicesTabWidget;
    delete m_ksuManualTesting;
}

void StartWindowTesting::devicesTabWidget(int index)
{
    if (m_devicesTabWidget == nullptr) {
        m_devicesTabWidget = new DevicesTabWidget();
        m_devicesTabWidget->setWindowModality(Qt::NonModal);
        m_devicesTabWidget->setWindowTitle("Оборудование стенда");
        m_devicesTabWidget->setWindowIcon(QIcon(":/Images/gnome-network-properties.png"));
        m_devicesTabWidget->resize(QSize(1024, 768));
        m_devicesTabWidget->move(QApplication::desktop()->screenGeometry().center() - m_devicesTabWidget->rect().center());
    }

    m_devicesTabWidget->setCurrentIndex(index);
    m_devicesTabWidget->showMaximized();
    m_devicesTabWidget->activateWindow();
    OperatorClickReport::instance()->write(QStringLiteral("Окно оборудования стенда: открытие окна"));
}

void StartWindowTesting::energoForm()
{
    setWindowState(Qt::WindowMinimized);

    //    QProcess *process = new QProcess(this);
    //    QString file = ENERGOFORM_PATH;
    //    process->start(file);

    //    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &StartWindow::showNormal);
    //    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), process, &QProcess::deleteLater);
    //    connect(process, &QProcess::errorOccurred, this, &StartWindow::showNormal);
    //    connect(process, &QProcess::errorOccurred, this, &StartWindow::deleteLater);

    auto energoform = EnergoFormRunningControl::instance();
    energoform->runEnergoForm();

}

void StartWindowTesting::manualTestMode()
{
    if (m_ksuManualTesting == nullptr) {
        m_ksuManualTesting = new KsuManualTestingWidget(IOServer::instance()->ksuNode());
        m_ksuManualTesting->setWindowModality(Qt::NonModal);
        m_ksuManualTesting->setWindowTitle("Ручное тестирование");
        m_ksuManualTesting->setWindowIcon(QIcon(":/Images/manual_testing.png"));
        m_ksuManualTesting->readSettings();
    }

    m_ksuManualTesting->showNormal();
    m_ksuManualTesting->activateWindow();
    OperatorClickReport::instance()->write(QStringLiteral("Окно ручного тестирования: открытие окна"));
}

void StartWindowTesting::stepTestMode()
{

}

void StartWindowTesting::autoTestMode()
{
    TestKitSlectWindow *testKitSelectWindow = new TestKitSlectWindow;
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

void StartWindowTesting::graphicsPanel()
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

void StartWindowTesting::updateksu()
{
    setWindowState(Qt::WindowMinimized);

    UpdateKsuWizard *wizard = new UpdateKsuWizard;

    wizard->setAttribute(Qt::WA_DeleteOnClose);
    wizard->setWindowModality(Qt::ApplicationModal);
    wizard->setWindowTitle("Перепрограммирование ксу");
    wizard->setWindowIcon(QIcon(":/Images/updateksu.png"));
    wizard->resize(QSize(640, 480));
    wizard->move(QApplication::desktop()->screenGeometry().center() - wizard->rect().center());
    wizard->show();
    wizard->activateWindow();
    wizard->showNormal();

    OperatorClickReport::instance()->write(QStringLiteral("Окно перепрограммирование КСУ: запус визарда"));

}

void StartWindowTesting::reportConfigure()
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

void StartWindowTesting::testCreatorControl()
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

void StartWindowTesting::testKitControl()
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

void StartWindowTesting::referenceSettingsKSU()
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

void StartWindowTesting::closeEvent(QCloseEvent *event)
{
    IOServer::deleteIOServer();

    StartWindow *startWindow = new StartWindow;
    startWindow->setAttribute(Qt::WA_DeleteOnClose);
    startWindow->setWindowModality(Qt::NonModal);

    startWindow->setWindowTitle(titleString);

    startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    startWindow->resize(QSize(1024, 768));
    startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
    startWindow->show();
    startWindow->activateWindow();
    startWindow->showNormal();
    OperatorClickReport::instance()->write(QStringLiteral("Окно тестирования: закрытие окна"));
    event->accept();
    QWidget::closeEvent(event);
}
