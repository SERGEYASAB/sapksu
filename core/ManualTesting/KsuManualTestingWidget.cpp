#include "KsuManualTestingWidget.h"

#include "IOServer/IOServer.h"
#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/ModbusKsuNode.h"
#include "ManualTesting/Common/ManualTestingConnectionParametrsWidget.h"
#include "AppSettings/AppSettings.h"


#include "Ksu735/Manual0x10Testing/Ksu735Manual0x10TestingWidget.h"
#include "Ksu735/Manual0x20Testing/Ksu735Manual0x20TestingWidget.h"
#include "Ksu735/ManualFullTesting/Ksu735ManualFullTestingWidget.h"
#include "Ksu735/Manual0x30Testing/Ksu735Manual0x30TestingWidget.h"

#include "StartWindow/ActionStyledWidget.h"
#include "StartWindow/ActionStyledBooleanWidget.h"
#include "StartWindow/ActionStyledFlowLayout.h"
#include "StartWindow/ActionStyledGroupMenuWidget.h"
#include "StartWindow/SwitchWidget.h"

#include "ModbusKsu/ModbusLinkTestingWidget/ModbusLinkTedtingWidget.h"
#include "ModbusKsu/ModbusKsuManualFullTesting/ModbusKsuManualFullTestingWidget.h"
#include "ModbusKsu/ModbusKsuManualSelectiveTesting/ModbusKsuManualSelectiveTestingWidget.h"
#include "ModbusKsu/ModbusKsuPasswordsWidget/ModbusKsuPasswordsWidget.h"

#include "ModbusKsu/ModbusKsuManualTestingArchives/ModbusKsuManualTestingChangeParametrsArchiveWidget.h"
#include "ModbusKsu/ModbusKsuManualTestingArchives/ModbusKsuManualTestingCurrentParametrsArchiveWidget.h"
#include "ModbusKsu/ModbusKsuManualTestingArchives/ModbusKsuManualTestingEventsArchiveWidget.h"
#include "ModbusKsu/ModbusKsuManualTestingArchives/ModbusKsuManualTestingLaunchParametrsArchiveWidget.h"

#include "Logger/KsuLogWidget/KsuLogWidget.h"
#include "ReportManager/OperatorClickReport.h"
#include "SelectKsuWindow/KsuTestResultDialog.h"
#include "Devices/IRZFC/IRZFCWidget.h"
#include "Devices/AFC/AFCWidget.h"
#include "ReportManager/ParamsReport.h"

SwitchWidget * KsuManualTestingWidget::createWorkModeRadioButton()
{
    QList<QAction*> actions {m_offModeAction, m_manualModeAction, m_autoModeAction};
    SwitchWidget *sw = new SwitchWidget("Режим работы КСУ", actions, false);
    sw->resetCheck();
    sw->check(m_offModeAction);
    return sw;
}

SwitchWidget *KsuManualTestingWidget::create232tmsRadioButton()
{
    QList<QAction*> actions {m_rs232TmsEmulator, m_rs232TmsReal, m_rs232TmsOff};
    SwitchWidget *sw = new SwitchWidget("Порт RS232", actions, false);
    sw->resetCheck();
    //sw->check(m_rs232TmsEmulator);
    sw->check(m_rs232TmsOff);
    return sw;
}

SwitchWidget *KsuManualTestingWidget::create485tmsRadioButton()
{
    QList<QAction*> actions {m_rs485TmsEmulator, m_rs485TmsReal, m_rs485TmsOff};
    SwitchWidget *sw = new SwitchWidget("Порт RS485", actions, false);
    sw->resetCheck();
    //sw->check(m_rs485TmsEmulator);
    sw->check(m_rs485TmsOff);
    return sw;
}

SwitchWidget *KsuManualTestingWidget::createPhasesModeRadioButton()
{
    QList<QAction *> actions {m_phaseAbcAction, m_phaseCbaAction};
    SwitchWidget *sw = new SwitchWidget("Черед. фаз", actions, false);
    sw->resetCheck();
    sw->check(m_phaseAbcAction);
    return sw;
}

void KsuManualTestingWidget::readSettings()
{
    auto settings = AppSettings::settings();
    restoreGeometry(settings->value(QString(metaObject()->className()) + "/geometry").toByteArray());
    restoreState(settings->value(QString(metaObject()->className()) + "/windowState").toByteArray());
}

void KsuManualTestingWidget::closeEvent(QCloseEvent *event)
{
    KsuTestResultDialog *result = new KsuTestResultDialog;
    result->setWindowTitle("Завершение тестирования");
    if (result->exec() == QDialog::Rejected) {
        delete result;
        event->ignore();
        return;
    }
    delete result;

    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->endGroup();
    settings->sync();

    QMainWindow::closeEvent(event);
}

KsuManualTestingWidget::KsuManualTestingWidget(INode *node, QWidget *parent) : QMainWindow(parent)
{
    m_tabWidget = new QTabWidget();

    if (Ksu735Node *ksu735Node = qobject_cast<Ksu735Node *>(node)) {
        testMode = TestMode::Ksu;

        m_connectionParametrsWidget = new ManualTestingConnectionParametrsWidget();
        m_tabWidget->addTab(m_connectionParametrsWidget, QStringLiteral("Параметры связи"));

        m_ksu735Manual0x10TestingWidget = new Ksu735Manual0x10TestingWidget(ksu735Node);
        m_tabWidget->addTab(m_ksu735Manual0x10TestingWidget, QStringLiteral("Запрос 0x10"));

        m_ksu735Manual0x20TestingWidget = new Ksu735Manual0x20TestingWidget(ksu735Node);
        m_tabWidget->addTab(m_ksu735Manual0x20TestingWidget, QStringLiteral("Запросы 0x20/0x40"));

        m_ksu735ManualFullTestingWidget = new Ksu735ManualFullTestingWidget(ksu735Node);
        m_tabWidget->addTab(m_ksu735ManualFullTestingWidget, QStringLiteral("Полное тестирование"));

        m_ksu735Manual0x30TestingWidget = new Ksu735Manual0x30TestingWidget(ksu735Node);
        m_tabWidget->addTab(m_ksu735Manual0x30TestingWidget, QStringLiteral("Запрос 0x30"));

    } else if (ModbusKsuNode *modbusKsuNode = qobject_cast<ModbusKsuNode *>(node)) {
        testMode = TestMode::Modbus;

        m_connectionParametrsWidget = new ManualTestingConnectionParametrsWidget();
        m_tabWidget->addTab(m_connectionParametrsWidget, QStringLiteral("Параметры связи"));

        //        m_modbusLinkTedtingWidget = new ModbusLinkTedtingWidget(modbusKsuNode);
        //        m_tabWidget->addTab(m_modbusLinkTedtingWidget, QStringLiteral("Диагностика связи"));

        m_modbusFullTestingWidget = new ModbusKsuManualFullTestingWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusFullTestingWidget, QStringLiteral("Полное тестирование"));

        m_modbusManualSelectiveTestingWidget = new ModbusKsuManualSelectiveTestingWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusManualSelectiveTestingWidget, QStringLiteral("Выборочное тестирование"));

        m_modbusManualCurrentParametrsArchiveWidget = new ModbusKsuManualTestingCurrentParametrsArchiveWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusManualCurrentParametrsArchiveWidget, QStringLiteral("Архив текущих параметров"));

        m_modbusManualChangeParametrsArchiveWidget = new ModbusKsuManualTestingChangeParametrsArchiveWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusManualChangeParametrsArchiveWidget, QStringLiteral("Архив изменения параметров"));

        m_modbusManualEventsArchiveWidget = new ModbusKsuManualTestingEventsArchiveWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusManualEventsArchiveWidget, QStringLiteral("Архив событий"));

        m_modbusManualLaunchParametrsArchiveWidget = new ModbusKsuManualTestingLaunchParametrsArchiveWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusManualLaunchParametrsArchiveWidget, QStringLiteral("Архив пусковых параметров"));

        m_modbusKsuPasswordsWidget = new ModbusKsuPasswordsWidget(modbusKsuNode);
        m_tabWidget->addTab(m_modbusKsuPasswordsWidget, QStringLiteral("Пароли"));
    }

    m_abortAction = new QAction(QIcon(":/Images/abortButton.png"), tr("&Аварийный стоп"), this);
    connect(m_abortAction, &QAction::triggered, this, &KsuManualTestingWidget::abortKsu);

    m_timeKsuAction = new QAction(QIcon(":/Images/timeButton.png"), tr("Время КСУ"), this);
    connect(m_timeKsuAction, &QAction::triggered, this, &KsuManualTestingWidget::ksuTime);

    m_startKsuAction = new QAction(QIcon(":/Images/startButton.png"), tr("Пуск"), this);
    connect(m_startKsuAction, &QAction::triggered, this, &KsuManualTestingWidget::startKsu);

    m_stopKsuAction = new QAction(QIcon(":/Images/stopButton.png"), tr("Стоп"), this);
    connect(m_stopKsuAction, &QAction::triggered, this, &KsuManualTestingWidget::stopKsu);

    m_deblocKsuAction = new QAction(QIcon(":/Images/deblocButton.png"), tr("Деблокировка"), this);
    connect(m_deblocKsuAction, &QAction::triggered, this, &KsuManualTestingWidget::deblocKsu);

    m_offModeAction = new QAction("Отключено");
    connect(m_offModeAction, &QAction::triggered, this, &KsuManualTestingWidget::setOffModeKsu);

    m_manualModeAction = new QAction("Ручной");
    connect(m_manualModeAction, &QAction::triggered, this, &KsuManualTestingWidget::setManualModeKsu);

    m_autoModeAction = new QAction("Автоматический");
    connect(m_autoModeAction, &QAction::triggered, this, &KsuManualTestingWidget::setAutoModeKsu);

    m_onPhasesABCAction = new QAction("Задействовать фазы питания ABC");
    connect(m_onPhasesABCAction, &QAction::triggered, this, &KsuManualTestingWidget::onPhasesABC);

    m_rs232TmsEmulator = new QAction("Эмулятор RS232");
    connect(m_rs232TmsEmulator, &QAction::triggered, this, &KsuManualTestingWidget::rs232TmsEmulator);

    m_rs232TmsReal = new QAction("Наземный блок RS232");
    connect(m_rs232TmsReal, &QAction::triggered, this, &KsuManualTestingWidget::rs232TmsReal);

    m_rs232TmsOff = new QAction("Отключено");
    connect(m_rs232TmsOff, &QAction::triggered, this, &KsuManualTestingWidget::rs232TmsOff);

    m_rs485TmsEmulator = new QAction("Эмулятор RS484");
    connect(m_rs485TmsEmulator, &QAction::triggered, this, &KsuManualTestingWidget::rs485TmsEmulator);

    m_rs485TmsReal = new QAction("Наземный блок RS485");
    connect(m_rs485TmsReal, &QAction::triggered, this, &KsuManualTestingWidget::rs485TmsReal);

    m_rs485TmsOff = new QAction("Отключено");
    connect(m_rs485TmsOff, &QAction::triggered, this, &KsuManualTestingWidget::rs485TmsOff);

    m_startKSUDirectLogicAction = new QAction("Запуск СУ");
    connect(m_startKSUDirectLogicAction, &QAction::triggered, this, &KsuManualTestingWidget::startKSUDirectLogic);

    m_stopKSUDirectLogicAction = new QAction("Останов СУ");
    connect(m_stopKSUDirectLogicAction, &QAction::triggered, this, &KsuManualTestingWidget::stopKSUDirectLogic);

    m_phaseAbcAction = new QAction("ABC");
    connect(m_phaseAbcAction, &QAction::triggered, this, &KsuManualTestingWidget::phaseAbc);

    m_phaseCbaAction = new QAction("CBA");
    connect(m_phaseCbaAction, &QAction::triggered, this, &KsuManualTestingWidget::phaseCba);

    m_phaseOffAction = new QAction("Отключено");
    connect(m_phaseOffAction, &QAction::triggered, this, &KsuManualTestingWidget::phaseOff);

    m_ksuSupply = new QAction("Питание \r\n контроллера");
    connect(m_ksuSupply, &QAction::triggered, this, &KsuManualTestingWidget::ksuSupply);

    m_1stepLoad = new QAction("1-я супень:\r\nI = 22A; cosf = 0,13");
    connect(m_1stepLoad, &QAction::triggered, this, &KsuManualTestingWidget::step1Load);

    m_2stepLoad = new QAction("2-я супень:\r\nI = 32A; cosf = 0,78");
    connect(m_2stepLoad, &QAction::triggered, this, &KsuManualTestingWidget::step2Load);

    m_3stepLoad = new QAction("3-я супень:\r\nI = 54A; cosf = 0,92");
    connect(m_3stepLoad, &QAction::triggered, this, &KsuManualTestingWidget::step3Load);

    m_stepOff = new QAction("Отключить все ступени");
    connect(m_stepOff, &QAction::triggered, this, &KsuManualTestingWidget::stepOffSUdirectStart);

    m_supplySUdirectStart = new QAction("Напрямую\r\nот сети");
    connect(m_supplySUdirectStart, &QAction::triggered, this, &KsuManualTestingWidget::supplySUdirectStart);

    m_supplySUfreqConverter = new QAction("Регулирование\r\nнапряжения питания");
    connect(m_supplySUfreqConverter, &QAction::triggered, this, &KsuManualTestingWidget::supplySUfreqConverter);

    switchRs485Action = new QAction("Смена проводов A и B для интерфейса RS485");
    connect(switchRs485Action, &QAction::triggered, this, &KsuManualTestingWidget::switch485AB);

    m_irzFCWidget = new IRZFCWidget(IRZFCWidget::IRZFCWidgetMode::Cut);
    m_irzFCWidget->setDisabled(true);

#ifdef SAP_KSU

    QSize iconSize = QSize(40, 40);

    ActionStyledWidget *m_abortStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                         m_abortAction,
                                                                         new QLabel("Аварийный \r\n СТОП"),
                                                                         QIcon(":/Images/abortButton.png"),
                                                                         iconSize);

    ActionStyledWidget *m_onPhasesABCStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_onPhasesABCAction,
                                                                               new QLabel("Задействовать фазы \r\n питания"),
                                                                               QIcon(":/Images/on_phases.png"),
                                                                               iconSize);

    ActionStyledBooleanWidget *m_doorKsuStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_DOOR_STATE"),
                                                                                                ACCESSLEVEL_USER,
                                                                                                new QLabel("Состояние \r\n двери"),
                                                                                                QIcon(":/Images/door.png"),
                                                                                                iconSize);

    ActionStyledBooleanWidget *m_tmsPowerStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_TMS_REAL_POWER"),
                                                                                                 ACCESSLEVEL_USER,
                                                                                                 new QLabel("Питание ТМС"),
                                                                                                 QIcon(":/Images/tmsSupply.png"),
                                                                                                 iconSize);

    ActionStyledBooleanWidget *m_phaseAStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_A_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза А"),
                                                                                               QIcon(":/Images/phase_a.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_phaseBStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_B_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза B"),
                                                                                               QIcon(":/Images/phase_b.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_phaseCStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_C_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза С"),
                                                                                               QIcon(":/Images/phase_c.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_turbineRotationStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_TURBINE_ROTATION"),
                                                                                                        ACCESSLEVEL_USER,
                                                                                                        new QLabel("Турбинное \r\n вращение"),
                                                                                                        QIcon(":/Images/turbine_rotation.png"),
                                                                                                        iconSize);

    ActionStyledBooleanWidget *m_externalDisableStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_EXTERNAL_LOCK"),
                                                                                                        ACCESSLEVEL_USER,
                                                                                                        new QLabel("Внешний \r\n запрет"),
                                                                                                        QIcon(":/Images/lockButton.png"),
                                                                                                        iconSize);

    ActionStyledBooleanWidget *m_manometrStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_MANOMETR"),
                                                                                                 ACCESSLEVEL_USER,
                                                                                                 new QLabel("ЭКМ"),
                                                                                                 QIcon(":/Images/voltmetr2.png"),
                                                                                                 iconSize);

    ActionStyledBooleanWidget *m_0TMPNStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_0TMPN"),
                                                                                              ACCESSLEVEL_USER,
                                                                                              new QLabel("Сигнал \r\n 0 - ТМПН"),
                                                                                              QIcon(":/Images/devices2.png"),
                                                                                              iconSize);
    ActionStyledBooleanWidget *m_0TMPNPlateStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_0TMPN_PLATE"),
                                                                                                   ACCESSLEVEL_USER,
                                                                                                   new QLabel("Наличие платы \r\n 0 - ТМПН"),
                                                                                                   QIcon(":/Images/mktn.png"),
                                                                                                   iconSize);

    ActionStyledWidget *m_timeKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                           m_timeKsuAction,
                                                                           new QLabel("Время КСУ"),
                                                                           QIcon(":/Images/timeButton.png"),
                                                                           iconSize);

    ActionStyledWidget *m_startKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                            m_startKsuAction,
                                                                            new QLabel("Пуск"),
                                                                            QIcon(":/Images/startButton.png"),
                                                                            iconSize);

    ActionStyledWidget *m_stopKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                           m_stopKsuAction,
                                                                           new QLabel("Стоп"),
                                                                           QIcon(":/Images/stopButton.png"),
                                                                           iconSize);

    ActionStyledWidget *m_deblocKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                             m_deblocKsuAction,
                                                                             new QLabel("Деблок."),
                                                                             QIcon(":/Images/deblocButton.png"),
                                                                             iconSize);

    ActionStyledWidget *m_startKSUDirectLogicStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                                       m_startKSUDirectLogicAction,
                                                                                       new QLabel("Пуск"),
                                                                                       QIcon(":/Images/startButton.png"),
                                                                                       iconSize);

    ActionStyledWidget *m_stopKSUDirectLogicStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                                      m_stopKSUDirectLogicAction,
                                                                                      new QLabel("Стоп"),
                                                                                      QIcon(":/Images/stopButton.png"),
                                                                                      iconSize);

    ActionStyledWidget *m_ksuSupplyStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                             m_ksuSupply,
                                                                             new QLabel("Питание \r\n контроллера"),
                                                                             QIcon(":/Images/powerSupply.png"),
                                                                             iconSize);

    SwitchWidget *workMode = createWorkModeRadioButton();
    SwitchWidget *rs232tmsMode = create232tmsRadioButton();
    SwitchWidget *rs485tmsMode = create485tmsRadioButton();
    SwitchWidget *phasesMode = createPhasesModeRadioButton();

    AFCWidget *afc = new AFCWidget();
    afc->setViewForManualTestingWidget();

    ActionStyledFlowLayout *serviceMenuLayout = new ActionStyledFlowLayout;
    serviceMenuLayout->addWidget(m_abortStylesheetWidget);
    serviceMenuLayout->addWidget(m_ksuSupplyStylesheetWidget);

    ActionStyledFlowLayout *supplyMenuLayout = new ActionStyledFlowLayout;
    supplyMenuLayout->addWidget(m_phaseAStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_phaseBStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_phaseCStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_onPhasesABCStylesheetWidget);
    supplyMenuLayout->addWidget(m_turbineRotationStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(phasesMode);

    ActionStyledFlowLayout *ctrl485Layout = new ActionStyledFlowLayout;
    ctrl485Layout->addWidget(m_startKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_stopKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_deblocKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_timeKsuStylesheetWidget);

    ctrl485Layout->addWidget(m_doorKsuStylesheetBooleanWidget);
    ctrl485Layout->addWidget(m_externalDisableStylesheetBooleanWidget);
    ctrl485Layout->addWidget(m_manometrStylesheetBooleanWidget);
    ctrl485Layout->addWidget(m_0TMPNStylesheetBooleanWidget);
    ctrl485Layout->addWidget(m_0TMPNPlateStylesheetBooleanWidget);


    ActionStyledFlowLayout *suModeLayout = new ActionStyledFlowLayout;
    suModeLayout->addWidget(workMode);
    suModeLayout->addWidget(m_startKSUDirectLogicStylesheetWidget);
    suModeLayout->addWidget(m_stopKSUDirectLogicStylesheetWidget);

    ActionStyledFlowLayout *ctrlTMSLayout = new ActionStyledFlowLayout;
    ctrlTMSLayout->addWidget(m_tmsPowerStylesheetBooleanWidget);
    ctrlTMSLayout->addWidget(rs232tmsMode);
    ctrlTMSLayout->addWidget(rs485tmsMode);

    ActionStyledFlowLayout *afcLayout = new ActionStyledFlowLayout;
    afcLayout->addWidget(afc);

    // QDockWidget //

    // Сервисное меню
    QDockWidget *serviceMenuDockWidget = new QDockWidget(tr("Сервисное меню"), this);
    serviceMenuDockWidget->setObjectName("serviceMenuDockWidget");
    serviceMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    QWidget *serviceMenuWidget = new QWidget();
    serviceMenuWidget->setLayout(serviceMenuLayout);
    serviceMenuDockWidget->setWidget(serviceMenuWidget);

    // Измерительные цепи питания
    QDockWidget *supplyMenuDockWidget = new QDockWidget(tr("Измерительные цепи питания"), this);
    supplyMenuDockWidget->setObjectName("supplyMenuDockWidget");
    supplyMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    QWidget *supplyMenuWidget = new QWidget();
    supplyMenuWidget->setLayout(supplyMenuLayout);
    supplyMenuDockWidget->setWidget(supplyMenuWidget);

    // Управление КСУ по интерфейсу RS485 ctrl485Layout
    QDockWidget *ctrl485DockWidget = new QDockWidget(tr("Управление КСУ по интерфейсу RS485"), this);
    ctrl485DockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    ctrl485DockWidget->setObjectName("ctrl485DockWidget");
    QWidget *ctrl485MenuWidget = new QWidget();
    ctrl485MenuWidget->setLayout(ctrl485Layout);
    ctrl485DockWidget->setWidget(ctrl485MenuWidget);

    // Режим управления СУ
    QDockWidget *suModeMenuDockWidget = new QDockWidget(tr("Режим управления СУ"), this);
    suModeMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    suModeMenuDockWidget->setObjectName("suModeMenuDockWidget");
    QWidget *suModeMenuWidget = new QWidget();
    suModeMenuWidget->setLayout(suModeLayout);
    suModeMenuDockWidget->setWidget(suModeMenuWidget);

    // Управление ТМС
    QDockWidget *tmsMenuDockWidget = new QDockWidget(tr("Управление ТМС"), this);
    tmsMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    tmsMenuDockWidget->setObjectName("tmsMenuDockWidget");
    QWidget *tmsMenuWidget = new QWidget();
    tmsMenuWidget->setLayout(ctrlTMSLayout);
    tmsMenuDockWidget->setWidget(tmsMenuWidget);

    // Управление Турбинным вращением
    QDockWidget *turbineMenuDockWidget = new QDockWidget(tr("Упр. турбинное вращение"), this);
    turbineMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    turbineMenuDockWidget->setObjectName("turbineMenuDockWidget");
    QWidget *turbineMenuWidget = new QWidget();
    turbineMenuWidget->setLayout(afcLayout);
    turbineMenuDockWidget->setWidget(turbineMenuWidget);

    addDockWidget(Qt::RightDockWidgetArea, serviceMenuDockWidget);

    QMainWindow::tabifyDockWidget(serviceMenuDockWidget, supplyMenuDockWidget);
    QMainWindow::tabifyDockWidget(serviceMenuDockWidget, ctrl485DockWidget);
    QMainWindow::tabifyDockWidget(serviceMenuDockWidget, suModeMenuDockWidget);
    QMainWindow::tabifyDockWidget(serviceMenuDockWidget, tmsMenuDockWidget);
    QMainWindow::tabifyDockWidget(serviceMenuDockWidget, turbineMenuDockWidget);

#endif
#ifdef SAP_SU
    QSize iconSize = QSize(40, 40);

    ActionStyledBooleanWidget *powerSupply = new ActionStyledBooleanWidget(QString("$SAP_POWER_SUPPLY"),
                                                                           ACCESSLEVEL_USER,
                                                                           new QLabel("Подача питания на СУ"),
                                                                           QIcon(":/Images/tmsSupply.png"),
                                                                           iconSize);

    ActionStyledBooleanWidget *m_phaseAStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_A_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза А"),
                                                                                               QIcon(":/Images/phase_a.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_phaseBStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_B_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза B"),
                                                                                               QIcon(":/Images/phase_b.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_phaseCStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_C_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза С"),
                                                                                               QIcon(":/Images/phase_c.png"),
                                                                                               iconSize);

    ActionStyledWidget *m_onPhasesABCStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_onPhasesABCAction,
                                                                               new QLabel("Задействовать фазы \r\n питания"),
                                                                               QIcon(":/Images/on_phases.png"),
                                                                               iconSize);

    ActionStyledWidget *m_startKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                            m_startKsuAction,
                                                                            new QLabel("Пуск"),
                                                                            QIcon(":/Images/startButton.png"),
                                                                            iconSize);

    ActionStyledWidget *m_stopKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                           m_stopKsuAction,
                                                                           new QLabel("Стоп"),
                                                                           QIcon(":/Images/stopButton.png"),
                                                                           iconSize);

    ActionStyledWidget *m_deblocKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                             m_deblocKsuAction,
                                                                             new QLabel("Деблок."),
                                                                             QIcon(":/Images/deblocButton.png"),
                                                                             iconSize);

    ActionStyledWidget *m_timeKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                           m_timeKsuAction,
                                                                           new QLabel("Время КСУ"),
                                                                           QIcon(":/Images/timeButton.png"),
                                                                           iconSize);

    ActionStyledWidget *m_1stepLoadWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                   m_1stepLoad,
                                                                   new QLabel("1-я супень:\r\nI = 22A; cosf = 0,13"),
                                                                   QIcon(":/Images/timeButton.png"),
                                                                   iconSize);

    ActionStyledWidget *m_2stepLoadWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                   m_2stepLoad,
                                                                   new QLabel("2-я супень:\r\nI = 32A; cosf = 0,78"),
                                                                   QIcon(":/Images/timeButton.png"),
                                                                   iconSize);

    ActionStyledWidget *m_3stepLoadWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                   m_3stepLoad,
                                                                   new QLabel("3-я супень:\r\nI = 54A; cosf = 0,92"),
                                                                   QIcon(":/Images/timeButton.png"),
                                                                   iconSize);

    ActionStyledWidget *m_stepOffWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                 m_stepOff,
                                                                 new QLabel("Отключить нагрузки"),
                                                                 QIcon(":/Images/stopButton.png"),
                                                                 iconSize);

    ActionStyledWidget *m_supplySUdirectStartWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                             m_supplySUdirectStart,
                                                                             new QLabel("Напрямую\r\nот сети"),
                                                                             QIcon(":/Images/timeButton.png"),
                                                                             iconSize);

    ActionStyledWidget *m_supplySUfreqConverterWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_supplySUfreqConverter,
                                                                               new QLabel("Регулирование\r\nнапряжения питания"),
                                                                               QIcon(":/Images/timeButton.png"),
                                                                               iconSize);

    ActionStyledWidget *m_switchRs485Widget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_supplySUfreqConverter,
                                                                               new QLabel("Изменить RS485\r\A<->B"),
                                                                               QIcon(":/Images/switch_phases485.png"),
                                                                               iconSize);

    ActionStyledFlowLayout *supplyMenuLayout = new ActionStyledFlowLayout;
    supplyMenuLayout->addWidget(m_phaseAStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_phaseBStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_phaseCStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_onPhasesABCStylesheetWidget);
    supplyMenuLayout->addWidget(powerSupply);

    ActionStyledFlowLayout *ctrl485Layout = new ActionStyledFlowLayout;
    ctrl485Layout->addWidget(m_startKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_stopKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_deblocKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_timeKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_switchRs485Widget);

    ActionStyledFlowLayout *stepsLoadLayout = new ActionStyledFlowLayout;
    stepsLoadLayout->addWidget(m_1stepLoadWidget);
    stepsLoadLayout->addWidget(m_2stepLoadWidget);
    stepsLoadLayout->addWidget(m_3stepLoadWidget);
    stepsLoadLayout->addWidget(m_stepOffWidget);

    ActionStyledFlowLayout *supplySULayout = new ActionStyledFlowLayout;
    supplySULayout->addWidget(m_supplySUdirectStartWidget);
    supplySULayout->addWidget(m_supplySUfreqConverterWidget);
    supplySULayout->addWidget(m_irzFCWidget);

    // QDockWidget //

    // Измерительные цепи питания
    QDockWidget *supplyMenuDockWidget = new QDockWidget(tr("Измерительные цепи питания"), this);
    supplyMenuDockWidget->setObjectName("supplyMenuDockWidget");
    supplyMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    QWidget *supplyMenuWidget = new QWidget();
    supplyMenuWidget->setLayout(supplyMenuLayout);
    supplyMenuDockWidget->setWidget(supplyMenuWidget);

    // Управление КСУ по интерфейсу RS485 ctrl485Layout
    QDockWidget *ctrl485DockWidget = new QDockWidget(tr("Управление КСУ по интерфейсу RS485"), this);
    ctrl485DockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    ctrl485DockWidget->setObjectName("ctrl485DockWidget");
    QWidget *ctrl485MenuWidget = new QWidget();
    ctrl485MenuWidget->setLayout(ctrl485Layout);
    ctrl485DockWidget->setWidget(ctrl485MenuWidget);

    // Управление ступенчатой нагрузкой
    m_ctrlStepLoadDockWidget = new QDockWidget(tr("Управление нагрузкой: нагрузки отключены"), this);
    m_ctrlStepLoadDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_ctrlStepLoadDockWidget->setObjectName("ctrlStepLoadDockWidget");
    QWidget *ctrlStepLoadMenuWidget = new QWidget();
    ctrlStepLoadMenuWidget->setLayout(stepsLoadLayout);
    m_ctrlStepLoadDockWidget->setWidget(ctrlStepLoadMenuWidget);

    // Питание СУ
    m_supplySUDockWidget = new QDockWidget(tr("Питание СУ: режим прямого пуска"), this);
    m_supplySUDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_supplySUDockWidget->setObjectName("supplySUDockWidget");
    QWidget *supplySUDockMenuWidget = new QWidget();
    supplySUDockMenuWidget->setLayout(supplySULayout);
    m_supplySUDockWidget->setWidget(supplySUDockMenuWidget);

    addDockWidget(Qt::RightDockWidgetArea, supplyMenuDockWidget);
    QMainWindow::tabifyDockWidget(supplyMenuDockWidget, ctrl485DockWidget);
    QMainWindow::tabifyDockWidget(ctrl485DockWidget, m_ctrlStepLoadDockWidget);
    QMainWindow::tabifyDockWidget(m_ctrlStepLoadDockWidget, m_supplySUDockWidget);

#endif
#ifdef MECHOS
    QSize iconSize = QSize(40, 40);

    ActionStyledBooleanWidget *powerSupply = new ActionStyledBooleanWidget(QString("$SAP_POWER_SUPPLY"),
                                                                           ACCESSLEVEL_USER,
                                                                           new QLabel("Подача питания на СУ"),
                                                                           QIcon(":/Images/tmsSupply.png"),
                                                                           iconSize);

    ActionStyledBooleanWidget *m_phaseAStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_A_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза А"),
                                                                                               QIcon(":/Images/phase_a.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_phaseBStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_B_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза B"),
                                                                                               QIcon(":/Images/phase_b.png"),
                                                                                               iconSize);

    ActionStyledBooleanWidget *m_phaseCStylesheetBooleanWidget = new ActionStyledBooleanWidget(QString("$SAP_PHASE_C_ENABLE"),
                                                                                               ACCESSLEVEL_USER,
                                                                                               new QLabel("Фаза С"),
                                                                                               QIcon(":/Images/phase_c.png"),
                                                                                               iconSize);

    ActionStyledWidget *m_onPhasesABCStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_onPhasesABCAction,
                                                                               new QLabel("Задействовать фазы \r\n питания"),
                                                                               QIcon(":/Images/on_phases.png"),
                                                                               iconSize);

    ActionStyledWidget *m_startKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                            m_startKsuAction,
                                                                            new QLabel("Пуск"),
                                                                            QIcon(":/Images/startButton.png"),
                                                                            iconSize);

    ActionStyledWidget *m_stopKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                           m_stopKsuAction,
                                                                           new QLabel("Стоп"),
                                                                           QIcon(":/Images/stopButton.png"),
                                                                           iconSize);

    ActionStyledWidget *m_deblocKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                             m_deblocKsuAction,
                                                                             new QLabel("Деблок."),
                                                                             QIcon(":/Images/deblocButton.png"),
                                                                             iconSize);

    ActionStyledWidget *m_timeKsuStylesheetWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                           m_timeKsuAction,
                                                                           new QLabel("Время КСУ"),
                                                                           QIcon(":/Images/timeButton.png"),
                                                                           iconSize);

    ActionStyledWidget *m_1stepLoadWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                   m_1stepLoad,
                                                                   new QLabel("1-я супень:\r\nI = 22A; cosf = 0,13"),
                                                                   QIcon(":/Images/timeButton.png"),
                                                                   iconSize);

    ActionStyledWidget *m_2stepLoadWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                   m_2stepLoad,
                                                                   new QLabel("2-я супень:\r\nI = 32A; cosf = 0,78"),
                                                                   QIcon(":/Images/timeButton.png"),
                                                                   iconSize);

    ActionStyledWidget *m_3stepLoadWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                   m_3stepLoad,
                                                                   new QLabel("3-я супень:\r\nI = 54A; cosf = 0,92"),
                                                                   QIcon(":/Images/timeButton.png"),
                                                                   iconSize);

    ActionStyledWidget *m_stepOffWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                 m_stepOff,
                                                                 new QLabel("Отключить нагрузки"),
                                                                 QIcon(":/Images/stopButton.png"),
                                                                 iconSize);

    ActionStyledWidget *m_supplySUdirectStartWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                             m_supplySUdirectStart,
                                                                             new QLabel("Напрямую\r\nот сети"),
                                                                             QIcon(":/Images/timeButton.png"),
                                                                             iconSize);

    ActionStyledWidget *m_supplySUfreqConverterWidget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_supplySUfreqConverter,
                                                                               new QLabel("Регулирование\r\nнапряжения питания"),
                                                                               QIcon(":/Images/timeButton.png"),
                                                                               iconSize);

    ActionStyledWidget *m_switchRs485Widget = new ActionStyledWidget(ACCESSLEVEL_USER,
                                                                               m_supplySUfreqConverter,
                                                                               new QLabel("Изменить RS485\r\A<->B"),
                                                                               QIcon(":/Images/switch_phases485.png"),
                                                                               iconSize);

    ActionStyledFlowLayout *supplyMenuLayout = new ActionStyledFlowLayout;
    supplyMenuLayout->addWidget(m_phaseAStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_phaseBStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_phaseCStylesheetBooleanWidget);
    supplyMenuLayout->addWidget(m_onPhasesABCStylesheetWidget);
    supplyMenuLayout->addWidget(powerSupply);

    ActionStyledFlowLayout *ctrl485Layout = new ActionStyledFlowLayout;
    ctrl485Layout->addWidget(m_startKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_stopKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_deblocKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_timeKsuStylesheetWidget);
    ctrl485Layout->addWidget(m_switchRs485Widget);

    ActionStyledFlowLayout *stepsLoadLayout = new ActionStyledFlowLayout;
    stepsLoadLayout->addWidget(m_1stepLoadWidget);
    stepsLoadLayout->addWidget(m_2stepLoadWidget);
    stepsLoadLayout->addWidget(m_3stepLoadWidget);
    stepsLoadLayout->addWidget(m_stepOffWidget);

    ActionStyledFlowLayout *supplySULayout = new ActionStyledFlowLayout;
    supplySULayout->addWidget(m_supplySUdirectStartWidget);
    supplySULayout->addWidget(m_supplySUfreqConverterWidget);
    supplySULayout->addWidget(m_irzFCWidget);

    // QDockWidget //

    // Измерительные цепи питания
    QDockWidget *supplyMenuDockWidget = new QDockWidget(tr("Измерительные цепи питания"), this);
    supplyMenuDockWidget->setObjectName("supplyMenuDockWidget");
    supplyMenuDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    QWidget *supplyMenuWidget = new QWidget();
    supplyMenuWidget->setLayout(supplyMenuLayout);
    supplyMenuDockWidget->setWidget(supplyMenuWidget);

    // Управление КСУ по интерфейсу RS485 ctrl485Layout
    QDockWidget *ctrl485DockWidget = new QDockWidget(tr("Управление КСУ по интерфейсу RS485"), this);
    ctrl485DockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    ctrl485DockWidget->setObjectName("ctrl485DockWidget");
    QWidget *ctrl485MenuWidget = new QWidget();
    ctrl485MenuWidget->setLayout(ctrl485Layout);
    ctrl485DockWidget->setWidget(ctrl485MenuWidget);

    // Управление ступенчатой нагрузкой
    m_ctrlStepLoadDockWidget = new QDockWidget(tr("Управление нагрузкой: нагрузки отключены"), this);
    m_ctrlStepLoadDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_ctrlStepLoadDockWidget->setObjectName("ctrlStepLoadDockWidget");
    QWidget *ctrlStepLoadMenuWidget = new QWidget();
    ctrlStepLoadMenuWidget->setLayout(stepsLoadLayout);
    m_ctrlStepLoadDockWidget->setWidget(ctrlStepLoadMenuWidget);

    // Питание СУ
    m_supplySUDockWidget = new QDockWidget(tr("Питание СУ: режим прямого пуска"), this);
    m_supplySUDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_supplySUDockWidget->setObjectName("supplySUDockWidget");
    QWidget *supplySUDockMenuWidget = new QWidget();
    supplySUDockMenuWidget->setLayout(supplySULayout);
    m_supplySUDockWidget->setWidget(supplySUDockMenuWidget);

    addDockWidget(Qt::RightDockWidgetArea, supplyMenuDockWidget);
    QMainWindow::tabifyDockWidget(supplyMenuDockWidget, ctrl485DockWidget);
    QMainWindow::tabifyDockWidget(ctrl485DockWidget, m_ctrlStepLoadDockWidget);
    QMainWindow::tabifyDockWidget(m_ctrlStepLoadDockWidget, m_supplySUDockWidget);

#endif

    setCentralWidget(m_tabWidget);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &KsuManualTestingWidget::tabWidgetCurrentIndexChanged);

    m_logWidget = new KsuLogWidget();
    m_logDock = new QDockWidget(tr("Лог обмена данными с КСУ"));
    m_logDock->setObjectName(QStringLiteral("ksuManualLogDockWidget"));
    m_logDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_logDock->setWidget(m_logWidget);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
}

KsuManualTestingWidget::~KsuManualTestingWidget()
{

}

void KsuManualTestingWidget::stopAllTests()
{
    if (m_ksu735Manual0x10TestingWidget) {
        m_ksu735Manual0x10TestingWidget->stopTesting();
    }
    if (m_ksu735ManualFullTestingWidget) {
        m_ksu735ManualFullTestingWidget->stopTesting();
    }

    if (m_modbusFullTestingWidget) {
        m_modbusFullTestingWidget->stopTesting();
    }
    if (m_modbusManualSelectiveTestingWidget) {
        m_modbusManualSelectiveTestingWidget->stopTesting();
    }

    if (m_modbusManualCurrentParametrsArchiveWidget) {
        m_modbusManualCurrentParametrsArchiveWidget->stopTesting();
    }
    if (m_modbusManualChangeParametrsArchiveWidget) {
        m_modbusManualChangeParametrsArchiveWidget->stopTesting();
    }
    if (m_modbusManualEventsArchiveWidget) {
        m_modbusManualEventsArchiveWidget->stopTesting();
    }
    if (m_modbusManualLaunchParametrsArchiveWidget) {
        m_modbusManualLaunchParametrsArchiveWidget->stopTesting();
    }
}

void KsuManualTestingWidget::abortKsu()
{
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для аварийного отключения питания нажмите \"Ок\"")))
        return;
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_abortAction->text()));
    IOServer::instance()->abortButtonOnPCPressed();
}

//TODO: переделать на получение значения от синхронной записи
void KsuManualTestingWidget::startKsu()
{
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения пуска нажмите \"Ок\"")))
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_start"));
    if (!variable)
        return;
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_startKsuAction->text()));
    variable->setValue(true);
}

void KsuManualTestingWidget::stopKsu()
{
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения останова нажмите \"Ок\"")))
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_stop"));
    if (!variable)
        return;
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_stopKsuAction->text()));
    variable->setValue(true);
}

void KsuManualTestingWidget::deblocKsu()
{
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения деблокировки КСУ нажмите \"Ок\"")))
        return;

    IVariable *variable = IOServer::instance()->variableByAlias(QStringLiteral("KSU_deblock"));
    if (!variable)
        return;
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_deblocKsuAction->text()));
    variable->setValue(true);
}

void KsuManualTestingWidget::ksuTime()
{
    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения времени КСУ с временем ПК нажмите \"Ок\"")))
        return;

    if (testMode == TestMode::Ksu) {
        Ksu735Node *ksuNode = qobject_cast<Ksu735Node *>(ioServer->ksuNode());
        if (!ksuNode)
            return;

        IVariable *yearMonthVariable = ioServer->variableByAlias(QStringLiteral("KSU735_date_YM"));
        IVariable *dayHourVariable = ioServer->variableByAlias(QStringLiteral("KSU735_date_DH"));
        IVariable *minSecVariable = ioServer->variableByAlias(QStringLiteral("KSU735_date_MS"));

        if (!yearMonthVariable || !dayHourVariable || !minSecVariable)
            return;

        QDateTime dateTime = QDateTime::currentDateTime();
        int yearMonth   = (dateTime.date().year() - 2000) * 100 + dateTime.date().month();
        int dayHour     = dateTime.date().day() * 100 + dateTime.time().hour();
        int minSec      = dateTime.time().minute() * 100 + dateTime.time().second();

        QList<QPair<IVariable *, QVariant>> writedValuesData;
        writedValuesData.append(QPair<IVariable *, QVariant>(yearMonthVariable, yearMonth));
        writedValuesData.append(QPair<IVariable *, QVariant>(dayHourVariable, dayHour));
        writedValuesData.append(QPair<IVariable *, QVariant>(minSecVariable, minSec));

        ksuNode->writeVariables(writedValuesData, Ksu735Node::OperationTimingType::Sync);
    } else if (testMode == TestMode::Modbus) {
        IVariable *dateTimeVariable = ioServer->variableByAlias(QStringLiteral("KSUModbus_currentDateTime"));
        if (!dateTimeVariable)
            return;
        OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_timeKsuAction->text()));
        dateTimeVariable->setValue(QDateTime::currentDateTime());
    }
}

void KsuManualTestingWidget::setOffModeKsu()
{
    if (!IOServer::instance())
        return;

    IVariable *var3 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_OFF"));

    IVariable *var1 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_MANUAL"));
    if (!var1)
        return;

    IVariable *var2 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_AUTO"));
    if (!var2)
        return;

    var1->setValue(false);
    var2->setValue(false);

    if (var3)
        var3->setValue(true);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_offModeAction->text()));
}

void KsuManualTestingWidget::setManualModeKsu()
{
    if (!IOServer::instance())
        return;

    IVariable *var1 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_MANUAL"));
    if (!var1)
        return;

    IVariable *var2 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_AUTO"));
    if (!var2)
        return;

    // И только в такой последовательности, чтобы не схлопатать режим "Отключено"
    var1->setValue(true);
    var2->setValue(false);

    IVariable *var3 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_OFF"));
    if (var3)
        var3->setValue(false);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_manualModeAction->text()));
}

void KsuManualTestingWidget::setAutoModeKsu()
{
    if (!IOServer::instance())
        return;

    IVariable *var2 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_AUTO"));
    if (!var2)
        return;

    IVariable *var1 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_MANUAL"));
    if (!var1)
        return;

    // И только в такой последовательности, чтобы не схлопатать режим "Отключено"
    var2->setValue(true);
    var1->setValue(false);

    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_autoModeAction->text()));

    IVariable *var3 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_WORKMODE_OFF"));
    if (!var3)
        return;

    var3->setValue(false);
}

void KsuManualTestingWidget::switch485AB()
{
#ifdef SAP_KSU
#endif
#ifdef SAP_SU

    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Изменить A и B для интерфейса RS485?")))
        return;

    IVariable *switcherRS485 = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_KSULINK_SWITCH485"));
    if (!switcherRS485)
        return;

    switcherRS485->setValue(!switcherRS485->readValue().toBool());

#endif
#ifdef MECHOS

#endif
}

//void KsuManualTestingWidget::switchPhases()
//{
//    QMessageBox msgBox("Application Name", "Message", QMessageBox::Warning,
//                       QMessageBox::Yes | QMessageBox::Default,
//                       QMessageBox::No | QMessageBox::Escape,
//                       QMessageBox::NoButton);

//    msgBox.setWindowTitle("Инверсия фаз питания");
//    msgBox.setText("ВНИМАНИЕ!");
//    msgBox.setInformativeText(QString("Перед тем, как выполнится переключение фаз питания отключите в программе EnForm генерацию сигнала."
//                                      "Убедитесь, что генерация сигнала с прибора ЭНЕРГОФОРМА 3.1 отключена"
//                                      "В противном случае это может привести к выходу прибора из строя"
//                                      "Генерация отключена, можно продолжить?"));

//    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
//    if (layout)
//        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

//    msgBox.setMinimumWidth(400);
//    msgBox.setDefaultButton(QMessageBox::No);

//    switch (msgBox.exec()) {
//        case QMessageBox::No: {
//            return;
//            break;
//        }

//        case QMessageBox::Yes: {

//            if (!IOServer::instance())
//                return;

//            IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
//            IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
//            IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));
//            IVariable *switchPhase = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_SWITCH_PHASES"));

//            if (!phaseA || !phaseB || !phaseC || !switchPhase)
//                return;

//            phaseA->setValue(false);
//            phaseB->setValue(false);
//            phaseC->setValue(false);

//            switchPhase->setValue(!switchPhase->readValue().toBool());
//            break;
//        }
//    }
//}

//void KsuManualTestingWidget::switchTurbineRotation()
//{
//    IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));

//    if (!turbineRotation)
//        return;

//    // Отключение сигнала турбинного вращения
//    if (turbineRotation->readValue().toBool()) {

//        turbineRotation->setValue(false);

//        if (turbineRotation->readValue().toBool()) {
//            QMessageBox::warning(this, "Ошибка", "Не удалось отключить сигнал турбинного вращения");
//        } else {
//            QMessageBox::warning(this, "Отключено", "Сигнал турбинного вращения отключен");
//        }

//        return;
//    }

//    // Включение сигнала турбинного вращения

//    QMessageBox msgBox("Application Name", "Message", QMessageBox::Warning,
//                       QMessageBox::Yes | QMessageBox::Default,
//                       QMessageBox::No | QMessageBox::Escape,
//                       QMessageBox::NoButton);

//    msgBox.setWindowTitle("Изменение состояния сигнала турбинного вращения");
//    msgBox.setText("ВНИМАНИЕ!");
//    msgBox.setInformativeText(QString("Перед тем, как включить сигнал турбинного вращения отключите в программе EnForm генерацию сигнала."
//                                      "Убедитесь, что генерация сигнала с прибора ЭНЕРГОФОРМА 3.1 отключена \n"
//                                      "В противном случае это может привести к выходу прибора из строя"
//                                      "Генерация отключена, можно продолжить?"));

//    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
//    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
//    if (layout)
//        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

//    msgBox.setMinimumWidth(400);
//    msgBox.setDefaultButton(QMessageBox::No);

//    switch (msgBox.exec()) {
//        case QMessageBox::No: {
//            return;
//            break;
//        }

//        case QMessageBox::Yes: {

//            if (!IOServer::instance())
//                return;

//            IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
//            IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
//            IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));

//            if (!phaseA || !phaseB || !phaseC)
//                return;

//            phaseA->setValue(false);
//            phaseB->setValue(false);
//            phaseC->setValue(false);

//            turbineRotation->setValue(true);

//            if (turbineRotation->readValue().toBool()) {
//                QMessageBox::warning(this, "Вклчючено", "Сигнал турбинного вращения включен");
//            } else {
//                QMessageBox::warning(this, "Ошибка", "Не удалось включить сигнал турбинного вращения");
//            }

//            break;
//        }
//    }

//}

void KsuManualTestingWidget::onPhasesABC()
{
#ifdef SAP_KSU

    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Вкл/Откл фазы питания \"Ок\"")))
        return;

    IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
    IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
    IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));
    IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));


    if (!phaseA || !phaseB || !phaseC || !turbineRotation)
        return;

    turbineRotation->setValue(false);


    if (!turbineRotation->readValue().toBool()) {

        if(phaseA->readValue().toBool() && phaseB->readValue().toBool() && phaseC->readValue().toBool()) {
            phaseA->setValue(false);
            phaseB->setValue(false);
            phaseC->setValue(false);

            return;
        }

        phaseA->setValue(true);
        phaseB->setValue(true);
        phaseC->setValue(true);

    }
#endif
#ifdef SAP_SU
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Вкл/Откл фазы питания \"Ок\"")))
        return;

    IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
    IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
    IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));

    if (!phaseA || !phaseB || !phaseC)
        return;

    if (phaseA->readValue().toBool() && phaseB->readValue().toBool() && phaseC->readValue().toBool()) {
        phaseA->setValue(false);
        phaseB->setValue(false);
        phaseC->setValue(false);

        return;
    } else {

        phaseA->setValue(true);
        phaseB->setValue(true);
        phaseC->setValue(true);

    }

#endif
#ifdef MECHOS
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Вкл/Откл фазы питания \"Ок\"")))
        return;

    IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
    IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
    IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));

    if (!phaseA || !phaseB || !phaseC)
        return;

    if (phaseA->readValue().toBool() && phaseB->readValue().toBool() && phaseC->readValue().toBool()) {
        phaseA->setValue(false);
        phaseB->setValue(false);
        phaseC->setValue(false);

        return;
    } else {

        phaseA->setValue(true);
        phaseB->setValue(true);
        phaseC->setValue(true);

    }

#endif

    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_onPhasesABCAction->text()));
}

void KsuManualTestingWidget::rs232TmsEmulator()
{
    if (!IOServer::instance())
        return;

    IVariable *rs232Emu = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_EMULATOR_OFF_232"));
    if (!rs232Emu)
        return;

    IVariable *rs232Real = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_REAL_ON_232"));
    if (!rs232Real)
        return;

    rs232Emu->setValue(false);
    rs232Real->setValue(false);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_rs232TmsEmulator->text()));
}

void KsuManualTestingWidget::rs232TmsReal()
{
    if (!IOServer::instance())
        return;

    IVariable *rs232Emu = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_EMULATOR_OFF_232"));
    if (!rs232Emu)
        return;

    IVariable *rs232Real = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_REAL_ON_232"));
    if (!rs232Real)
        return;

    rs232Emu->setValue(true);
    rs232Real->setValue(true);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_rs232TmsReal->text()));
}

void KsuManualTestingWidget::rs232TmsOff()
{
    if (!IOServer::instance())
        return;

    IVariable *rs232Emu = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_EMULATOR_OFF_232"));
    if (!rs232Emu)
        return;

    IVariable *rs232Real = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_REAL_ON_232"));
    if (!rs232Real)
        return;

    rs232Emu->setValue(true);
    rs232Real->setValue(false);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\" ТМС RS232").arg(m_rs232TmsOff->text()));
}

void KsuManualTestingWidget::rs485TmsEmulator()
{
    if (!IOServer::instance())
        return;

    IVariable *rs485Emu = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_EMULATOR_OFF_485"));
    if (!rs485Emu)
        return;

    IVariable *rs485Real = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_REAL_ON_485"));
    if (!rs485Real)
        return;

    rs485Emu->setValue(false);
    rs485Real->setValue(false);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_rs485TmsEmulator->text()));
}

void KsuManualTestingWidget::rs485TmsReal()
{
    if (!IOServer::instance())
        return;

    IVariable *rs485Emu = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_EMULATOR_OFF_485"));
    if (!rs485Emu)
        return;

    IVariable *rs485Real = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_REAL_ON_485"));
    if (!rs485Real)
        return;

    rs485Emu->setValue(true);
    rs485Real->setValue(true);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_rs485TmsReal->text()));
}

void KsuManualTestingWidget::rs485TmsOff()
{
    if (!IOServer::instance())
        return;

    IVariable *rs485Emu = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_EMULATOR_OFF_485"));
    if (!rs485Emu)
        return;

    IVariable *rs485Real = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TMS_REAL_ON_485"));
    if (!rs485Real)
        return;

    rs485Emu->setValue(true);
    rs485Real->setValue(false);
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\" ТМС RS485").arg(m_rs485TmsOff->text()));
}

void KsuManualTestingWidget::startKSUDirectLogic()
{
    if (!IOServer::instance())
        return;

    IVariable *ksuStartButtonFromDirectLogic = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_START_SU"));
    if (!ksuStartButtonFromDirectLogic)
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения пуска нажмите \"Ок\"")))
        return;

    ksuStartButtonFromDirectLogic->setValue(true);
    QTimer::singleShot(500, [ksuStartButtonFromDirectLogic]{ksuStartButtonFromDirectLogic->setValue(false);});
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_startKSUDirectLogicAction->text()));
}

void KsuManualTestingWidget::stopKSUDirectLogic()
{
    if (!IOServer::instance())
        return;

    IVariable *ksuStopButtonFromDirectLogic = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_STOP_SU"));
    if (!ksuStopButtonFromDirectLogic)
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Для подтверждения останова нажмите \"Ок\"")))
        return;

    ksuStopButtonFromDirectLogic->setValue(true);
    QTimer::singleShot(500, [ksuStopButtonFromDirectLogic]{ksuStopButtonFromDirectLogic->setValue(false);});
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_stopKSUDirectLogicAction->text()));

}

void KsuManualTestingWidget::phaseAbc()
{
    //    QMessageBox msgBox("Application Name", "Message", QMessageBox::Warning,
    //                       QMessageBox::Yes | QMessageBox::Default,
    //                       QMessageBox::No | QMessageBox::Escape,
    //                       QMessageBox::NoButton);

    //    msgBox.setWindowTitle("Инверсия фаз питания");
    //    msgBox.setText("ВНИМАНИЕ!");
    //    msgBox.setInformativeText(QString("Перед тем, как выполнится переключение фаз питания отключите в программе EnForm генерацию сигнала."
    //                                      "Убедитесь, что генерация сигнала с прибора ЭНЕРГОФОРМА 3.1 отключена"
    //                                      "В противном случае это может привести к выходу прибора из строя"
    //                                      "Генерация отключена, можно продолжить?"));

    //    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    //    if (layout)
    //        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    //    msgBox.setMinimumWidth(400);
    //    msgBox.setDefaultButton(QMessageBox::No);

    //    switch (msgBox.exec()) {
    //        case QMessageBox::No: {
    //            return;
    //            break;
    //        }

    //        case QMessageBox::Yes: {

    if (!IOServer::instance())
        return;

    IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
    IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
    IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));
    IVariable *switchPhase = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_SWITCH_PHASES"));
    IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));

    if (!phaseA || !phaseB || !phaseC || !switchPhase || !turbineRotation)
        return;

    // Запоминаем состояние фаз питания
    bool phaseA_oldValue = phaseA->readValue().toBool();
    bool phaseB_oldValue = phaseB->readValue().toBool();
    bool phaseC_oldValue = phaseC->readValue().toBool();

    if (phaseA_oldValue || phaseB_oldValue || phaseC_oldValue)
        turbineRotation->setValue(false);

    // Отключаем фазы
    phaseA->setValue(false);
    phaseB->setValue(false);
    phaseC->setValue(false);

    //Отключаем переключатель фаз питания
    switchPhase->setValue(false);

    QTimer::singleShot(500, [phaseA, phaseA_oldValue]{phaseA->setValue(phaseA_oldValue);});
    QTimer::singleShot(500, [phaseB, phaseB_oldValue]{phaseB->setValue(phaseB_oldValue);});
    QTimer::singleShot(500, [phaseC, phaseC_oldValue]{phaseC->setValue(phaseC_oldValue);});
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_phaseAbcAction->text()));
}


void KsuManualTestingWidget::phaseCba()
{
    //    QMessageBox msgBox("Application Name", "Message", QMessageBox::Warning,
    //                       QMessageBox::Yes | QMessageBox::Default,
    //                       QMessageBox::No | QMessageBox::Escape,
    //                       QMessageBox::NoButton);

    //    msgBox.setWindowTitle("Инверсия фаз питания");
    //    msgBox.setText("ВНИМАНИЕ!");
    //    msgBox.setInformativeText(QString("Перед тем, как выполнится переключение фаз питания отключите в программе EnForm генерацию сигнала."
    //                                      "Убедитесь, что генерация сигнала с прибора ЭНЕРГОФОРМА 3.1 отключена"
    //                                      "В противном случае это может привести к выходу прибора из строя"
    //                                      "Генерация отключена, можно продолжить?"));

    //    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    //    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    //    if (layout)
    //        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    //    msgBox.setMinimumWidth(400);
    //    msgBox.setDefaultButton(QMessageBox::No);

    //    switch (msgBox.exec()) {
    //        case QMessageBox::No: {
    //            return;
    //            break;
    //        }

    //        case QMessageBox::Yes: {

    if (!IOServer::instance())
        return;

    IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
    IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
    IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));
    IVariable *switchPhase = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_SWITCH_PHASES"));
    IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));

    if (!phaseA || !phaseB || !phaseC || !switchPhase || !turbineRotation)
        return;

    // Запоминаем состояние фаз питания
    bool phaseA_oldValue = phaseA->readValue().toBool();
    bool phaseB_oldValue = phaseB->readValue().toBool();
    bool phaseC_oldValue = phaseC->readValue().toBool();

    if (phaseA_oldValue || phaseB_oldValue || phaseC_oldValue)
        turbineRotation->setValue(false);

    // Отключаем фазы
    phaseA->setValue(false);
    phaseB->setValue(false);
    phaseC->setValue(false);

    //Включаем переключатель фаз питания
    switchPhase->setValue(true);

    QTimer::singleShot(500, [phaseA, phaseA_oldValue]{phaseA->setValue(phaseA_oldValue);});
    QTimer::singleShot(500, [phaseB, phaseB_oldValue]{phaseB->setValue(phaseB_oldValue);});
    QTimer::singleShot(500, [phaseC, phaseC_oldValue]{phaseC->setValue(phaseC_oldValue);});
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_phaseCbaAction->text()));
}


void KsuManualTestingWidget::phaseOff()
{
    QMessageBox msgBox("Application Name", "Message", QMessageBox::Warning,
                       QMessageBox::Yes | QMessageBox::Default,
                       QMessageBox::No | QMessageBox::Escape,
                       QMessageBox::NoButton);

    msgBox.setWindowTitle("Инверсия фаз питания");
    msgBox.setText("ВНИМАНИЕ!");
    msgBox.setInformativeText(QString("Перед тем, как выполнится переключение фаз питания отключите в программе EnForm генерацию сигнала."
                                      "Убедитесь, что генерация сигнала с прибора ЭНЕРГОФОРМА 3.1 отключена"
                                      "В противном случае это может привести к выходу прибора из строя"
                                      "Генерация отключена, можно продолжить?"));

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(400);
    msgBox.setDefaultButton(QMessageBox::No);

    switch (msgBox.exec()) {
    case QMessageBox::No: {
        return;
        break;
    }

    case QMessageBox::Yes: {

        if (!IOServer::instance())
            return;

        IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
        IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
        IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));
        IVariable *switchPhase = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_SWITCH_PHASES"));

        if (!phaseA || !phaseB || !phaseC || !switchPhase)
            return;

        phaseA->setValue(false);
        phaseB->setValue(false);
        phaseC->setValue(false);

        switchPhase->setValue(false);

        break;
    }
    }
    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\" (фаза)").arg(m_phaseOffAction->text()));
}

//void KsuManualTestingWidget::setPhaseA()
//{
//    if (!IOServer::instance())
//        return;

//    IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
//    if (!phaseA)
//        return;

//    if (!showConfirmChangeValueMessage(QStringLiteral("Для изменения состояния фазы А нажмите \"Ок\"")))
//        return;

//    phaseA->setValue(!phaseA->readValue.toBool());
//}

//void KsuManualTestingWidget::setPhaseB()
//{
//    if (!IOServer::instance())
//        return;

//    IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
//    if (!phaseB)
//        return;

//    if (!showConfirmChangeValueMessage(QStringLiteral("Для изменения состояния фазы B нажмите \"Ок\"")))
//        return;

//    phaseB->setValue(!phaseB->readValue.toBool());

//}

//void KsuManualTestingWidget::setPhaseC()
//{
//    if (!IOServer::instance())
//        return;

//    IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));
//    if (!phaseC)
//        return;

//    if (!showConfirmChangeValueMessage(QStringLiteral("Для изменения состояния фазы C нажмите \"Ок\"")))
//        return;

//    phaseC->setValue(!phaseC->readValue.toBool());

//}

void KsuManualTestingWidget::ksuSupply()
{
    if (!IOServer::instance())
        return;

    if (!showConfirmChangeValueMessage(QStringLiteral("Вкл/Откл питание КСУ \"Ок\"?")))
        return;

#ifdef SAP_KSU
    IVariable *uz1 = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_DRIVE"));
    if (!uz1)
        return;

    uz1->setValue(!uz1->readValue().toBool());

    IVariable *uz2 = IOServer::instance()->variableByAlias(QStringLiteral("$UZ2_CTRL_DRIVE"));
    if (!uz2)
        return;

    uz2->setValue(!uz2->readValue().toBool());
#endif
#ifdef SAP_SU
    IVariable *powerSupply = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_SUPPLY"));
    if (!powerSupply)
        return;

    powerSupply->setValue(!powerSupply->readValue().toBool());

#endif
#ifdef MECHOS
    IVariable *powerSupply = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_SUPPLY"));
    if (!powerSupply)
        return;

    powerSupply->setValue(!powerSupply->readValue().toBool());
#endif

    OperatorClickReport::instance()->write(QString("Ручное тестирование: нажата кнопка \"%1\"").arg(m_ksuSupply->text()));

}

void KsuManualTestingWidget::manometr()
{

}

void KsuManualTestingWidget::externalDisable()
{

}

void KsuManualTestingWidget::stepOffSUdirectStart()
{
    m_ctrlStepLoadDockWidget->setWindowTitle("Управление нагрузкой: нагрузки отключены");


    if (!IOServer::instance())
        return;

    IVariable *variable;

    // Отключаем тепловую пушку
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем нагрузку первой ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем нагрузку второй ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем электродвигатель
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
    if (!variable)
        return;

    variable->setValue(false);

    LoggerTaskManager2::getInstance()->log(QString("Отключение всех ступеней нагрузки (прямого пуска)"), LoggerTaskManager2::Info);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Отключение всех ступеней нагрузки (прямого пуска)"),
                                    QString());

}

void KsuManualTestingWidget::step1Load()
{
    if (!IOServer::instance())
        return;

    m_ctrlStepLoadDockWidget->setWindowTitle("Управление нагрузкой: включена 1-ая ступень");

    IVariable *variable;

    // Отключаем тепловую пушку
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем нагрузку первой ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем нагрузку второй ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

    if (!variable)
        return;

    variable->setValue(false);

    // Включаем электродвигатель
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
    if (!variable)
        return;

    variable->setValue(true);

    LoggerTaskManager2::getInstance()->log(QString("Включена нагрузка 1-я супень: I = 22A; cosf = 0,13"), LoggerTaskManager2::Info);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Включена нагрузка 1-я супень: I = 22A; cosf = 0,13"),
                                    QString());
}

void KsuManualTestingWidget::step2Load()
{
    if (!IOServer::instance())
        return;

    m_ctrlStepLoadDockWidget->setWindowTitle("Управление нагрузкой: включена 2-ая ступень");

    IVariable *variable;

    // Отключаем нагрузку второй ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

    if (!variable)
        return;

    variable->setValue(false);

    // Включаем тепловую пушку
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable)
        return;

    variable->setValue(true);

    // Включаем нагрузку первой ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

    if (!variable)
        return;

    variable->setValue(true);

    // Включаем электродвигатель
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
    if (!variable)
        return;

    variable->setValue(true);

    LoggerTaskManager2::getInstance()->log(QString("Включена нагрузка 2-я супень: I = 32A; cosf = 0,78"), LoggerTaskManager2::Info);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Включена нагрузка 2-я супень: I = 32A; cosf = 0,78"),
                                    QString());
}

void KsuManualTestingWidget::step3Load()
{
    if (!IOServer::instance())
        return;

    m_ctrlStepLoadDockWidget->setWindowTitle("Управление нагрузкой: включена 3-я ступень");

    IVariable *variable;

    // Включаем тепловую пушку
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN_ENABLE"));

    if (!variable)
        return;

    variable->setValue(true);

    // Включаем нагрузку первой ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN1_ON"));

    if (!variable)
        return;

    variable->setValue(true);

    // Включаем нагрузку второй ступени
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_HEATER_FAN2_ON"));

    if (!variable)
        return;

    variable->setValue(true);

    // Включаем электродвигатель
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_ELECTRIC_MOTOR_ENABLE"));
    if (!variable)
        return;

    variable->setValue(true);

    LoggerTaskManager2::getInstance()->log(QString("Включена нагрузка 3-я супень: I = 54A; cosf = 0,92"), LoggerTaskManager2::Info);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Включена нагрузка 3-я супень: I = 54A; cosf = 0,92"),
                                    QString());
}

void KsuManualTestingWidget::supplySUdirectStart()
{
    m_irzFCWidget->setDisabled(true);
    m_ctrlStepLoadDockWidget->setEnabled(true);

    m_supplySUDockWidget->setWindowTitle("Питание СУ: режим прямого пуска");

    if (!IOServer::instance())
        return;

    IVariable *variable;

    // Останавливаем частотник
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$UZ1_CTRL_DRIVE"));
    if (!variable)
        return;

    variable->setValue(0);

    // Снимаем сигнал разрешения работы UZ1
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_PCH_ENABLE"));
    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем питание СУ от ПЧ (регулирование напряжения)
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_PCH"));
    if (!variable)
        return;

    variable->setValue(false);

    // Включаем питание СУ от сети (прямой пуск)

    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_SUPPLY"));
    if (!variable)
        return;

    variable->setValue(true);

    ParamsReport::instance()->write(ParamsReport::ParamsReportFunction::Write,
                                    QByteArray(),
                                    QByteArray(),
                                    QString(),
                                    QString("Отключение нагрузки (ПЧ)"),
                                    QString());

}

void KsuManualTestingWidget::supplySUfreqConverter()
{
    m_ctrlStepLoadDockWidget->setDisabled(true);
    m_irzFCWidget->setEnabled(true);

    m_supplySUDockWidget->setWindowTitle("Питание СУ: режим регулировки напряжения");

    if (!IOServer::instance())
        return;

    IVariable *variable;

    // Отключаем питание СУ от сети (прямой пуск)

    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_SUPPLY"));
    if (!variable)
        return;

    variable->setValue(false);

    // Отключаем все нагрузки прямого пуска;
    stepOffSUdirectStart();

    // Включаем сигнал разрешения работы UZ1
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_PCH_ENABLE"));
    if (!variable)
        return;

    variable->setValue(true);

    // Включаем питание СУ от ПЧ (регулирование напряжения)
    variable = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_POWER_PCH"));
    if (!variable)
        return;

    variable->setValue(true);
}

void KsuManualTestingWidget::tabWidgetCurrentIndexChanged(int index)
{
    Q_UNUSED(index);
    stopAllTests();
}

bool KsuManualTestingWidget::showConfirmChangeValueMessage(const QString &text) const
{
    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QStringLiteral("Подтверждение"));
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    return msgBox.exec() == QMessageBox::Ok;
}
