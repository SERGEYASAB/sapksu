#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include "Common/PrefixHeader.h"

class IVariable;
class EngineLocalVariable;
class CodeEditorPlainTextEdit;
class JSSyntaxHighlighter;

class EditorWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit EditorWindow(const QString &path, QWidget *parent = nullptr);

public slots:
    void readSettings();

protected:
    bool loadConfiguration(const QString &path);
    void traverseXmlNodes(const QDomNode &node);
    void traverseSteps(const QDomNode &node);
    void traverseExternalVariables(const QDomNode &node);
    void travenreInternalVariables(const QDomNode &node);
    //Формитует xml из того, что есть в редакторе
    QDomDocument makeXml();
    //Сохраняет xml в файл
    bool saveConfiguration(const QDomDocument &xml, const QString &path);
    //Формирует и сохраняет
    void save();
    void saveAs();
    void closeEvent(QCloseEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

private:
    //Шаги
    struct Step {
        QString name;
        CodeEditorPlainTextEdit *editor;
        JSSyntaxHighlighter *highlighter;
    };
    QList<Step> m_steps;

    QMainWindow *m_stepWindow;
    QStackedWidget *m_stepWindows;
    QListWidget *m_stepListWidget;
    QDockWidget *m_stepDockWidget;
    QToolBar *m_stepToolBar;
    QAction *m_stepAddAction;
    QAction *m_stepRemoveAction;
    QAction *m_stepEditAction;
    QAction *m_stepUpAction;
    QAction *m_stepDownAction;
    QAction *m_stepCopyAction;

    void stepAdd();
    void stepRemove();
    void stepEdit();
    void stepUp();
    void stepDown();
    void stepCopy();
    void setupStepProperties(int step);

    QMainWindow *m_funcWindow;
    QListWidget *m_funcListWidget;
    QDockWidget *m_funcDockWidget;

    //Внешние переменные
    QMainWindow *m_extVarWindow;
    QListWidget *m_extVarListWidget;
    QDockWidget *m_extVarDockWidget;
    QToolBar *m_extVarToolBar;
    QAction *m_addExtVarAction;
    QAction *m_removeExtVarAction;
    QAction *m_highlightExtVarAction;
    void selectExternalVariables();
    void removeExternalVariable();
    void setupExternalVariableProperties(int index);

    //Локальные переменные
    QMainWindow *m_intVarWindow;
    QListWidget *m_intVarListWidget;
    QDockWidget *m_intVarDockWidget;
    QToolBar *m_intVarToolBar;
    QAction *m_addIntVarAction;
    QAction *m_removeIntVarAction;
    QAction *m_editIntVarAction;
    QAction *m_highlightIntVarAction;
    void addInternalVariable();
    void removeInternalVariable();
    void editInternalVariable();
    void setupInternalVariableProperties(int index);

    //Общие
    QToolBar *m_mainToolBar;
    QAction *m_saveXmlAction;
    QAction *m_saveAsAction = nullptr;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;
    QStatusBar *m_statusBar;

    QDomDocument m_xml;
    QString m_filePath;
    void rehighlight();

};

#endif // EDITORWINDOW_H
