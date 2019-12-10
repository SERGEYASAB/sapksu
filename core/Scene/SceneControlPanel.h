#ifndef SCENECONTROLPANEL_H
#define SCENECONTROLPANEL_H

#include "Common/PrefixHeader.h"

class SceneControlPanel : public QWidget
{

public:
    enum RunMode { Execution = 0, Editing = 1 };
    explicit SceneControlPanel(SceneControlPanel::RunMode runMode = Execution, QWidget *parent = nullptr);
    static QString viewsFolderPath();

private slots:
    void createView();
    void removeView();
    void editView();
    void runView();
    void refreshFileList();

private:
    RunMode m_runMode = SceneControlPanel::RunMode::Execution;

    QHBoxLayout *m_mainLayout = nullptr;
    QVBoxLayout *m_buttonsLayout = nullptr;
    QHBoxLayout *m_viewListLayout = nullptr;

    QListWidget *m_viewListWidget = nullptr;
    PushButtonPlus *m_createViewButton = nullptr;
    PushButtonPlus *m_removeViewButton = nullptr;
    PushButtonPlus *m_editViewButton = nullptr;
    PushButtonPlus *m_runViewButton = nullptr;
    PushButtonPlus *m_closeButton = nullptr;

    QStringList m_viewPaths;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SCENECONTROLPANEL_H
