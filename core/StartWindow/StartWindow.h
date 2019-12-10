#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include "Common/PrefixHeader.h"

class ActionStyledWidget;
class ActionStyledGroupMenuWidget;

class StartWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = Q_NULLPTR);

private:

    QAction *m_currentUserAction = nullptr;
    QAction *m_configUsersAction = nullptr;
    QAction *m_referenceSettingsKSUAction = nullptr;
    QAction *m_processSqliteAction = nullptr;
    QAction *m_testEditorAction = nullptr;
    QAction *m_testKitControlAction = nullptr;
    QAction *m_selectKsuAction = nullptr;
    QAction *m_graphicPanelAction = nullptr;
    QAction *m_devicesAction = nullptr;
    QAction *m_setupStylesheetAction = nullptr;
    QAction *m_attestationAction = nullptr;
    QAction *m_reportConfigureAction = nullptr;
    QAction *m_uploadConfigAction = nullptr;

    ActionStyledWidget *m_currentUserWidget = nullptr;
    ActionStyledWidget *m_configUsersWidget = nullptr;
    ActionStyledWidget *m_referenceSettingsKSUWidget = nullptr;
    ActionStyledWidget *m_processSqliteWidget = nullptr;
    ActionStyledWidget *m_testCreatorControlWidget = nullptr;
    ActionStyledWidget *m_testKitControlWidget = nullptr;
    ActionStyledWidget *m_selectKsuWidget = nullptr;
    ActionStyledWidget *m_graphicPanelWidget = nullptr;
    ActionStyledWidget *m_devicesWidget = nullptr;
    ActionStyledWidget *m_setupStylesheetWidget = nullptr;
    ActionStyledWidget *m_attestationWidget = nullptr;
    ActionStyledWidget *m_reportConfigureWidget = nullptr;
    ActionStyledWidget *m_uploadConfiWidget = nullptr;


private slots:
    void currentUser();
    void configUsers();
    void referenceSettingsKSU();
    void processSqlite();
    void testCreatorControl();
    void testKitControl();
    void selectKsu();
    void graphicsPanel();
    void devices();
    void setupStylesheet();
    void openAttestationWidget();
    void reportConfigure();
    void uploadConfig();
};

#endif // STARTWINDOW_H
