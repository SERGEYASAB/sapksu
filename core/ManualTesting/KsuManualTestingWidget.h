#ifndef KSUMANUALTESTINGWIDGET_H
#define KSUMANUALTESTINGWIDGET_H

#include "Common/PrefixHeader.h"

class INode;

class ManualTestingConnectionParametrsWidget;

class Ksu735Manual0x10TestingWidget;
class Ksu735Manual0x20TestingWidget;
class Ksu735ManualFullTestingWidget;
class Ksu735Manual0x30TestingWidget;
class ModbusUtility;
class ModbusKsuManualFullTestingWidget;
class ModbusKsuManualSelectiveTestingWidget;
class ModbusKsuPasswordsWidget;

class ModbusLinkTedtingWidget;
class ModbusKsuManualTestingCurrentParametrsArchiveWidget;
class ModbusKsuManualTestingChangeParametrsArchiveWidget;
class ModbusKsuManualTestingEventsArchiveWidget;
class ModbusKsuManualTestingLaunchParametrsArchiveWidget;

class SwitchWidget;
class KsuLogWidget;
class IRZFCWidget;

class KsuManualTestingWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit KsuManualTestingWidget(INode *node, QWidget *parent = 0);
    ~KsuManualTestingWidget();

public slots:
    void stopAllTests();
    void readSettings();

private slots:
    void abortKsu();
    void startKsu();
    void stopKsu();
    void deblocKsu();
    void ksuTime();
    void setOffModeKsu();
    void setManualModeKsu();
    void setAutoModeKsu();
    //    void switchPhases();
    //    void switchTurbineRotation();
    void switch485AB();
    void onPhasesABC();
    void rs232TmsEmulator();
    void rs232TmsReal();
    void rs232TmsOff();
    void rs485TmsEmulator();
    void rs485TmsReal();
    void rs485TmsOff();
    void startKSUDirectLogic();
    void stopKSUDirectLogic();
    void phaseAbc();
    void phaseCba();
    void phaseOff();
    //    void setPhaseA();
    //    void setPhaseB();
    //    void setPhaseC();
    void ksuSupply();
    void manometr();
    void externalDisable();
    void stepOffSUdirectStart();
    void step1Load();
    void step2Load();
    void step3Load();

    void supplySUdirectStart();
    void supplySUfreqConverter();

    void tabWidgetCurrentIndexChanged(int index);

private:
    bool showConfirmChangeValueMessage(const QString &text) const;
    SwitchWidget *createWorkModeRadioButton();
    SwitchWidget *create232tmsRadioButton();
    SwitchWidget *create485tmsRadioButton();
    SwitchWidget *createPhasesModeRadioButton();

private:
    enum class TestMode {
        Ksu,
        Modbus,
        Unknown
    };
    TestMode testMode = TestMode::Unknown;

    QAction *m_abortAction = nullptr;
    QAction *m_startKsuAction = nullptr;
    QAction *m_stopKsuAction = nullptr;
    QAction *m_deblocKsuAction = nullptr;
    QAction *m_timeKsuAction = nullptr;
    QAction *m_offModeAction = nullptr;
    QAction *m_manualModeAction = nullptr;
    QAction *m_autoModeAction = nullptr;
    QAction *switchRs485Action = nullptr;
    //    QAction *m_switchPhasesAction = nullptr;
    //    QAction *m_switchTurbineRotationAction = nullptr;
    QAction *m_onPhasesABCAction = nullptr;
    QAction *m_rs232TmsEmulator = nullptr;
    QAction *m_rs232TmsReal = nullptr;
    QAction *m_rs232TmsOff = nullptr;
    QAction *m_rs485TmsEmulator = nullptr;
    QAction *m_rs485TmsReal = nullptr;
    QAction *m_rs485TmsOff = nullptr;
    QAction *m_startKSUDirectLogicAction = nullptr;
    QAction *m_stopKSUDirectLogicAction = nullptr;
    QAction *m_phaseAbcAction = nullptr;
    QAction *m_phaseCbaAction = nullptr;
    QAction *m_phaseOffAction = nullptr;
    //    QAction *m_phaseAAction = nullptr;
    //    QAction *m_phaseBAction = nullptr;
    //    QAction *m_phaseCAction = nullptr;
    QAction *m_ksuSupply = nullptr;
    QAction *m_manometrAction = nullptr;
    QAction *m_externalDisabel = nullptr;
    QAction *m_1stepLoad = nullptr;
    QAction *m_2stepLoad = nullptr;
    QAction *m_3stepLoad = nullptr;
    QAction *m_stepOff = nullptr;

    QAction *m_supplySUdirectStart = nullptr;
    QAction *m_supplySUfreqConverter = nullptr;

    QTabWidget *m_tabWidget = nullptr;
    QToolBar *m_controlToolBar = nullptr;
    IRZFCWidget *m_irzFCWidget = nullptr;
    QDockWidget *m_ctrlStepLoadDockWidget = nullptr;
    QDockWidget *m_supplySUDockWidget = nullptr;

    ManualTestingConnectionParametrsWidget *m_connectionParametrsWidget = nullptr;
    Ksu735Manual0x10TestingWidget *m_ksu735Manual0x10TestingWidget = nullptr;
    Ksu735Manual0x20TestingWidget *m_ksu735Manual0x20TestingWidget = nullptr;
    Ksu735ManualFullTestingWidget *m_ksu735ManualFullTestingWidget = nullptr;
    Ksu735Manual0x30TestingWidget *m_ksu735Manual0x30TestingWidget = nullptr;

    ModbusUtility *m_modbusUtility = nullptr;
//    ModbusLinkTedtingWidget *m_modbusLinkTedtingWidget = nullptr;
    ModbusKsuManualFullTestingWidget *m_modbusFullTestingWidget = nullptr;
    ModbusKsuManualSelectiveTestingWidget *m_modbusManualSelectiveTestingWidget = nullptr;
    ModbusKsuManualTestingCurrentParametrsArchiveWidget *m_modbusManualCurrentParametrsArchiveWidget = nullptr;
    ModbusKsuManualTestingChangeParametrsArchiveWidget *m_modbusManualChangeParametrsArchiveWidget = nullptr;
    ModbusKsuManualTestingEventsArchiveWidget *m_modbusManualEventsArchiveWidget = nullptr;
    ModbusKsuManualTestingLaunchParametrsArchiveWidget *m_modbusManualLaunchParametrsArchiveWidget = nullptr;
    ModbusKsuPasswordsWidget *m_modbusKsuPasswordsWidget = nullptr;

    QDockWidget *m_logDock= nullptr;
    KsuLogWidget *m_logWidget = nullptr;
    QPushButton *m_showDockButton = nullptr;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // KSUMANUALTESTINGWIDGET_H
