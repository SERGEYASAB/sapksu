#ifndef STARTWINDOWTESTING_H
#define STARTWINDOWTESTING_H

#include "Common/PrefixHeader.h"

class ActionStyledWidget;
class DevicesTabWidget;
class KsuManualTestingWidget;

class StartWindowTesting : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindowTesting(QWidget *parent = nullptr);
    ~StartWindowTesting();

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void devicesTabWidget(int index);
    void energoForm();
    void manualTestMode();
    void stepTestMode();
    void autoTestMode();
    void graphicsPanel();
    void updateksu();
    void reportConfigure();
    void testCreatorControl();
    void testKitControl();
    void referenceSettingsKSU();

private:
    DevicesTabWidget *m_devicesTabWidget = nullptr;
    KsuManualTestingWidget *m_ksuManualTesting = nullptr;

    QAction *m_directlogicAction = nullptr;
    QAction *m_voltmetrAction = nullptr;
    QAction *m_frecurencyConvertorAction = nullptr;
    QAction *m_nlRizAction = nullptr;
    QAction *m_mktnAction = nullptr;
    QAction *m_energoFormAction = nullptr;
    QAction *m_upsAction = nullptr;
    QAction *m_manualTestModeAction = nullptr;
    QAction *m_stepTestModeAction = nullptr;
    QAction *m_autoTestModeAction = nullptr;
    QAction *m_graphicPanelAction = nullptr;
    QAction *m_updateKsuAction = nullptr;
    QAction *m_reportConfigureAction = nullptr;
    QAction *m_testEditorAction = nullptr;
    QAction *m_testKitControlAction = nullptr;
    QAction *m_referenceSettingsKSUAction = nullptr;

    ActionStyledWidget *m_directLogicWidget = nullptr;
    ActionStyledWidget *m_voltmetrWidget = nullptr;
    ActionStyledWidget *m_frecurencyConvertorWidget = nullptr;
    ActionStyledWidget *m_nlRizActionWidget = nullptr;
    ActionStyledWidget *m_mktnWidget = nullptr;
    ActionStyledWidget *m_energoFormWidget = nullptr;
    ActionStyledWidget *m_upsWidget = nullptr;
    ActionStyledWidget *m_manualTestModeWidget = nullptr;
    ActionStyledWidget *m_stepTestModeWidget = nullptr;
    ActionStyledWidget *m_autoTestModeWidget = nullptr;
    ActionStyledWidget *m_graphicPanelWidget = nullptr;
    ActionStyledWidget *m_updateKsuWidget = nullptr;
    ActionStyledWidget *m_reportConfigureWidget = nullptr;
    ActionStyledWidget *m_testCreatorControlWidget = nullptr;
    ActionStyledWidget *m_testKitControlWidget = nullptr;
    ActionStyledWidget *m_referenceSettingsKSUWidget = nullptr;
};
#endif // STARTWINDOWTESTING_H
