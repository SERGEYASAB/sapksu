#include "EditorWindow.h"
#include "AppSettings/AppSettings.h"
#include "IOServer/IOServer.h"
#include "DBConfig/DBConfig.h"
#include "IOServer/nsVariable/IVariable.h"
#include "Common/QPlainTextEdit/CodeEditorPlainTextEdit/CodeEditorPlainTextEdit.h"
#include "Common/ParametrAliases/ParameterAliasSelectWidget.h"
#include "Common/QSyntaxHighlighter/JSSyntaxHighlighter/JSSyntaxHighlighter.h"
#include "TestManager/JSTest.h"
#include "TestManager/ExecutorWindow/TestCreator.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

EditorWindow::EditorWindow(const QString &path, QWidget *parent) : QMainWindow(parent)
{
    //Настройка окна
    QSqlQuery query(DBConfig::instance()->configDatabase());
    query.exec(QString("SELECT name FROM `Tests` "
                       "WHERE path = '%1';").arg(path));

    setWindowTitle(query.first() ? query.record().value("name").toString() :
                                   tr("Неизвестный тест"));

    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    //    m_statusBar->showMessage(path);

    //Стек редакторов кода шагов
    m_stepWindows = new QStackedWidget();
    setCentralWidget(m_stepWindows);

    //Верхняя панель
    m_mainToolBar = new QToolBar(tr("Верхнее меню"));
    m_mainToolBar->setObjectName("m_mainToolBar");
    m_mainToolBar->setMovable(false);

    m_saveXmlAction = new QAction(QIcon(":/Images/document-save.png"), tr("&Сохранить изменения"), this);
    m_saveXmlAction->setStatusTip(tr("Сохранить изменения в текущем файле"));
    m_saveXmlAction->setShortcut(QKeySequence::Save);

    m_saveAsAction = new QAction(QIcon(":/Images/save_as_debugmode.png"), tr("&Выберите дирректорию проекта, для сохранения скрипта"), this);

    connect(m_saveXmlAction, &QAction::triggered, this, &EditorWindow::save);
    connect(m_saveAsAction, &QAction::triggered, this, &EditorWindow::saveAs);

    m_mainToolBar->addAction(m_saveXmlAction);
    m_mainToolBar->addAction(m_saveAsAction);

    addToolBar(Qt::TopToolBarArea, m_mainToolBar);

    m_undoAction = new QAction(QIcon(":/Images/edit-undo.png"), tr("&Отмена"), this);
    m_undoAction->setStatusTip(tr("Отменить последнюю операцию редактирования"));
    m_undoAction->setShortcuts(QKeySequence::Undo);
    m_undoAction->setDisabled(true);
    connect(m_undoAction, &QAction::triggered, [this]()
    {
        static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->undo();
    });

    m_redoAction = new QAction(QIcon(":/Images/edit-redo.png"), tr("&Повтор"), this);
    m_redoAction->setStatusTip(tr("Повторить последнюю операцию редактирования"));
    m_redoAction->setShortcuts(QKeySequence::Redo);
    m_redoAction->setDisabled(true);
    connect(m_redoAction, &QAction::triggered, [this]()
    {
        static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->redo();
    });

    m_cutAction = new QAction(QIcon(":/Images/edit-cut.png"), tr("&Вырезать"), this);
    m_cutAction->setStatusTip(tr("Вырезать выделенный текст"));
    m_cutAction->setShortcuts(QKeySequence::Cut);
    m_cutAction->setDisabled(true);
    connect(m_cutAction, &QAction::triggered, [this]()
    {
        static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->cut();
    });

    m_copyAction = new QAction(QIcon(":/Images/edit-copy.png"), tr("&Копировать"), this);
    m_copyAction->setStatusTip(tr("Копировать выделенный текст в буфер обмена"));
    m_copyAction->setShortcuts(QKeySequence::Copy);
    m_copyAction->setDisabled(true);
    connect(m_copyAction, &QAction::triggered, [this]()
    {
        static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->copy();
    });

    m_pasteAction = new QAction(QIcon(":/Images/edit-paste.png"), tr("&Вставить"), this);
    m_pasteAction->setStatusTip(tr("Вставить текст из буфера обмена"));
    m_pasteAction->setShortcuts(QKeySequence::Paste);
    connect(m_pasteAction, &QAction::triggered, [this]()
    {
        static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->paste();
    });

    m_selectAllAction = new QAction(QIcon(":/Images/edit-all.png"), tr("Выделить все"), this);
    m_selectAllAction->setStatusTip(tr("Выделить весь текст"));
    m_selectAllAction->setShortcuts(QKeySequence::SelectAll);
    connect(m_selectAllAction, &QAction::triggered, [this]()
    {
        static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->selectAll();
    });

    m_mainToolBar->addAction(m_undoAction);
    m_mainToolBar->addAction(m_redoAction);
    m_mainToolBar->addAction(m_cutAction);
    m_mainToolBar->addAction(m_copyAction);
    m_mainToolBar->addAction(m_pasteAction);
    m_mainToolBar->addAction(m_selectAllAction);

    // CRITICAL: Вылетает слишком часто
    //    connect(QApplication::clipboard(), &QClipboard::dataChanged, [this]()
    //    {
    //        if (m_pasteAction != nullptr)
    //            m_pasteAction->setEnabled(!QApplication::clipboard()->text().isEmpty());
    //    });

    //Док шагов
    m_stepAddAction = new QAction(QIcon(":/Images/list-add.png"), tr("&Добавить"), this);
    m_stepAddAction->setStatusTip(tr("Добавить шаг"));
    connect(m_stepAddAction, &QAction::triggered, this, &EditorWindow::stepAdd);

    m_stepRemoveAction = new QAction(QIcon(":/Images/list-remove.png"), tr("&Удалить"), this);
    m_stepRemoveAction->setStatusTip(tr("Удалить шаг"));
    connect(m_stepRemoveAction, &QAction::triggered, this, &EditorWindow::stepRemove);

    m_stepEditAction = new QAction(QIcon(":/Images/eraser.png"), tr("&Редактировать"), this);
    m_stepEditAction->setStatusTip(tr("Редактировать шаг"));
    connect(m_stepEditAction, &QAction::triggered, this, &EditorWindow::stepEdit);

    m_stepUpAction = new QAction(QIcon(":/Images/go-up.png"), tr("&Переместить вверх"), this);
    m_stepUpAction->setStatusTip(tr("Переместить шаг вверх"));
    connect(m_stepUpAction, &QAction::triggered, this, &EditorWindow::stepUp);

    m_stepDownAction = new QAction(QIcon(":/Images/go-down.png"), tr("&Переместить вниз"), this);
    m_stepDownAction->setStatusTip(tr("Переместить шаг вних"));
    connect(m_stepDownAction, &QAction::triggered, this, &EditorWindow::stepDown);

    m_stepCopyAction = new QAction(QIcon(":/Images/edit-copy.png"), tr("&Копировать шаг"), this);
    m_stepCopyAction->setStatusTip(tr("Копировать шаг"));
    connect(m_stepCopyAction, &QAction::triggered, this, &EditorWindow::stepCopy);

    m_stepToolBar = new QToolBar(tr("Управление"));
    m_stepToolBar->setObjectName("m_stepToolBar");
    m_stepToolBar->setMovable(false);
    m_stepToolBar->addAction(m_stepAddAction);
    m_stepToolBar->addAction(m_stepRemoveAction);
    m_stepToolBar->addAction(m_stepEditAction);
    m_stepToolBar->addAction(m_stepUpAction);
    m_stepToolBar->addAction(m_stepDownAction);
    m_stepToolBar->addAction(m_stepCopyAction);

    m_stepWindow = new QMainWindow(this);
    m_stepWindow->addToolBar(Qt::TopToolBarArea, m_stepToolBar);

    m_stepDockWidget = new QDockWidget(tr("Шаги скрипта"));
    m_stepDockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    m_stepDockWidget->setWidget(m_stepWindow);
    m_stepDockWidget->setObjectName("stepDockWidget");

    m_stepListWidget = new QListWidget();
    m_stepListWidget->setAlternatingRowColors(true);
    connect(m_stepListWidget, &QListWidget::currentRowChanged, this, &EditorWindow::setupStepProperties);
    m_stepWindow->setCentralWidget(m_stepListWidget);
    addDockWidget(Qt::LeftDockWidgetArea, m_stepDockWidget);

    //Список функций
    m_funcWindow = new QMainWindow(this);
    m_funcDockWidget = new QDockWidget(tr("Функции"));
    m_funcDockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    m_funcDockWidget->setObjectName("funcDockWidget");

    m_funcListWidget = new QListWidget();
    m_funcListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    for (auto i : TestCreator::instance()->funcList())
        m_funcListWidget->addItem(i.name + i.signature);

    connect(m_funcListWidget, &QListWidget::doubleClicked, [this]()
    {
        if (!m_stepListWidget->selectedItems().isEmpty())
            static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->insertPlainText(m_funcListWidget->currentItem()->text());
    });

    m_funcWindow->setCentralWidget(m_funcListWidget);
    m_funcDockWidget->setWidget(m_funcWindow);

    addDockWidget(Qt::RightDockWidgetArea, m_funcDockWidget);

    //Док внешних переменных
    m_extVarWindow = new QMainWindow(this);

    m_addExtVarAction = new QAction(QIcon(":/Images/list-add.png"), tr("&Добавить"), this);
    m_addExtVarAction->setStatusTip(tr("Добавить глобальные переменные в тест"));
    connect(m_addExtVarAction, &QAction::triggered, this, &EditorWindow::selectExternalVariables);

    m_removeExtVarAction = new QAction(QIcon(":/Images/list-remove.png"), tr("&Удалить"), this);
    m_removeExtVarAction->setStatusTip(tr("Удалить глобальную переменную из теста"));
    m_removeExtVarAction->setDisabled(true);
    connect(m_removeExtVarAction, &QAction::triggered, this, &EditorWindow::removeExternalVariable);

    m_highlightExtVarAction = new QAction(QIcon(":/Images/color-scheme.png"), tr("&Подсветка"), this);
    m_highlightExtVarAction->setStatusTip(tr("Подсвечсивать выделенную глобальную переменную в тексте"));
    m_highlightExtVarAction->setCheckable(true);
    connect(m_highlightExtVarAction, &QAction::toggled, this, &EditorWindow::rehighlight);

    m_extVarToolBar = new QToolBar(tr("Управление"));
    m_extVarToolBar->setObjectName("m_extVarToolBar");
    m_extVarToolBar->setMovable(false);
    m_extVarToolBar->addAction(m_addExtVarAction);
    m_extVarToolBar->addAction(m_removeExtVarAction);
    m_extVarToolBar->addAction(m_highlightExtVarAction);
    m_extVarWindow->addToolBar(Qt::TopToolBarArea, m_extVarToolBar);

    m_extVarDockWidget = new QDockWidget(tr("Глобальные переменные"));
    m_extVarDockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    m_extVarDockWidget->setObjectName("extVarDockWidget");

    m_extVarListWidget = new QListWidget();
    connect(m_extVarListWidget, &QListWidget::currentRowChanged, this, &EditorWindow::setupExternalVariableProperties);
    m_extVarListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    connect(m_extVarListWidget, &QListWidget::doubleClicked, [this]()
    {
        if (!m_stepListWidget->selectedItems().isEmpty())
            static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->insertPlainText(m_extVarListWidget->currentItem()->text());
    });

    m_extVarWindow->setCentralWidget(m_extVarListWidget);
    m_extVarDockWidget->setWidget(m_extVarWindow);

    addDockWidget(Qt::RightDockWidgetArea, m_extVarDockWidget);

    //Док локальных переменных
    m_intVarWindow = new QMainWindow(this);

    m_addIntVarAction = new QAction(QIcon(":/Images/list-add.png"), tr("&Добавить"), this);
    m_addIntVarAction->setStatusTip(tr("Добавить локальную переменную"));
    connect(m_addIntVarAction, &QAction::triggered, this, &EditorWindow::addInternalVariable);

    m_removeIntVarAction = new QAction(QIcon(":/Images/list-remove.png"), tr("&Удалить"), this);
    m_removeIntVarAction->setStatusTip(tr("Удалить локальную переменную"));
    m_removeIntVarAction->setDisabled(true);
    connect(m_removeIntVarAction, &QAction::triggered, this, &EditorWindow::removeInternalVariable);

    m_editIntVarAction = new QAction(QIcon(":/Images/eraser.png"), tr("&Редактировать"), this);
    m_editIntVarAction->setStatusTip(tr("Редактировать локальную переменную"));
    m_editIntVarAction->setDisabled(true);
    connect(m_editIntVarAction, &QAction::triggered, this, &EditorWindow::editInternalVariable);

    m_highlightIntVarAction = new QAction(QIcon(":/Images/color-scheme.png"), tr("&Подсветка"), this);
    m_highlightIntVarAction->setStatusTip(tr("Подсвечивать выделенную локальную переменную в тексте"));
    m_highlightIntVarAction->setCheckable(true);
    connect(m_highlightIntVarAction, &QAction::toggled, this, &EditorWindow::rehighlight);

    m_intVarToolBar = new QToolBar(tr("Управление"));
    m_intVarToolBar->setObjectName("m_intVarToolBar");
    m_intVarToolBar->setMovable(false);
    m_intVarToolBar->addAction(m_addIntVarAction);
    m_intVarToolBar->addAction(m_removeIntVarAction);
    m_intVarToolBar->addAction(m_editIntVarAction);
    m_intVarToolBar->addAction(m_highlightIntVarAction);
    m_intVarWindow->addToolBar(Qt::TopToolBarArea, m_intVarToolBar);

    m_intVarDockWidget = new QDockWidget(tr("Локальные переменные"));
    m_intVarDockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    m_intVarDockWidget->setObjectName("intVarDockWidget");

    m_intVarListWidget = new QListWidget();
    m_intVarListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    connect(m_intVarListWidget, &QListWidget::currentRowChanged, this, &EditorWindow::setupInternalVariableProperties);
    connect(m_intVarListWidget, &QListWidget::doubleClicked, [this]()
    {
        if (!m_stepListWidget->selectedItems().isEmpty())
            static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->insertPlainText(m_intVarListWidget->currentItem()->text() + QString(".value"));
    });

    m_intVarWindow->setCentralWidget(m_intVarListWidget);
    m_intVarDockWidget->setWidget(m_intVarWindow);

    addDockWidget(Qt::RightDockWidgetArea, m_intVarDockWidget);

    m_filePath = QCoreApplication::applicationDirPath() + QDir::separator() +
            "DBConfig" + QDir::separator() +
            "Tests" + QDir::separator() +
            path;
    loadConfiguration(m_filePath);
    traverseXmlNodes(m_xml);

    //Если файл пустой, создаем шаги "Инициализация" и "Деинициализация"
    if (m_steps.isEmpty()) {
        CodeEditorPlainTextEdit *beginEditor = new CodeEditorPlainTextEdit();
        connect(beginEditor, &CodeEditorPlainTextEdit::undoAvailable, m_undoAction, &QAction::setEnabled);
        connect(beginEditor, &CodeEditorPlainTextEdit::redoAvailable, m_redoAction, &QAction::setEnabled);
        connect(beginEditor, &CodeEditorPlainTextEdit::selectionChanged, [this, beginEditor]()
        {
            m_copyAction->setEnabled(!beginEditor->textCursor().selectedText().isEmpty());
            m_cutAction->setEnabled(!beginEditor->textCursor().selectedText().isEmpty());
        });

        beginEditor->setPlainText(tr("//Шаг инициализации"));
        m_steps.append({ tr("Инициализация"), beginEditor, new JSSyntaxHighlighter(beginEditor->document()) });
        m_stepWindows->addWidget(beginEditor);
        m_stepListWidget->insertItem(0, m_steps.first().name);

        CodeEditorPlainTextEdit *endEditor = new CodeEditorPlainTextEdit();
        connect(endEditor, &CodeEditorPlainTextEdit::undoAvailable, m_undoAction, &QAction::setEnabled);
        connect(endEditor, &CodeEditorPlainTextEdit::redoAvailable, m_redoAction, &QAction::setEnabled);
        connect(endEditor, &CodeEditorPlainTextEdit::selectionChanged, [this, endEditor]()
        {
            m_copyAction->setEnabled(!endEditor->textCursor().selectedText().isEmpty());
            m_cutAction->setEnabled(!endEditor->textCursor().selectedText().isEmpty());
        });
        endEditor->setPlainText(tr("//Шаг деинициализации"));
        m_steps.append({ tr("Деинициализация"), endEditor, new JSSyntaxHighlighter(endEditor->document())});
        m_stepWindows->addWidget(endEditor);
        m_stepListWidget->insertItem(m_steps.count() - 1, m_steps.last().name);
    }

    m_stepListWidget->setCurrentRow(0);
    //Подстветка первого и последнего шагов
    m_stepListWidget->item(0)->setBackgroundColor(Qt::lightGray);
    m_stepListWidget->item(m_steps.count() - 1)->setBackgroundColor(Qt::lightGray);
}

void EditorWindow::readSettings()
{
    auto settings = AppSettings::settings();
    restoreGeometry(settings->value(QString(metaObject()->className()) + "/geometry").toByteArray());
    restoreState(settings->value(QString(metaObject()->className()) + "/windowState").toByteArray());
}

bool EditorWindow::loadConfiguration(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << tr("Не удается открыть файл теста")<<file.fileName();
        return false;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!m_xml.setContent(file.readAll(), &errorMsg, &errorLine, &errorColumn)) {
        qCritical() << tr("Конфигурационный файл поврежден") << errorMsg << errorLine << errorColumn;
        file.close();
        return false;
    }

    file.close();
    return true;
}

void EditorWindow::traverseXmlNodes(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull()) {
        if (domNode.isElement()) {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()) {

                if (domElement.tagName() == "Test") {
                    //Нет пока атрибутов
                }
                else if (domElement.tagName() == "ExternalVariables") {
                    traverseExternalVariables(domNode);
                }
                else if (domElement.tagName() == "InternalVariables") {
                    travenreInternalVariables(domNode);
                }
                else if (domElement.tagName() == "Steps") {
                    traverseSteps(domNode);
                }
            }
        }
        traverseXmlNodes(domNode);
        domNode = domNode.nextSibling();
    }
}

void EditorWindow::traverseSteps(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();
    while (!domNode.isNull()) {
        if (domNode.isElement()) {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()) {
                if (domElement.tagName() == "Step" ||
                        domElement.tagName() == "Begin" ||
                        domElement.tagName() == "End") {
                    CodeEditorPlainTextEdit *editor = new CodeEditorPlainTextEdit();
                    connect(editor, &CodeEditorPlainTextEdit::undoAvailable, m_undoAction, &QAction::setEnabled);
                    connect(editor, &CodeEditorPlainTextEdit::redoAvailable, m_redoAction, &QAction::setEnabled);
                    connect(editor, &CodeEditorPlainTextEdit::selectionChanged, [this, editor]()
                    {
                        m_copyAction->setEnabled(!editor->textCursor().selectedText().isEmpty());
                        m_cutAction->setEnabled(!editor->textCursor().selectedText().isEmpty());
                    });
                    editor->setPlainText(domElement.text());
                    m_stepWindows->addWidget(editor);
                    m_steps.append({domElement.attribute("Name", tr("Неизвестный шаг")), editor, new JSSyntaxHighlighter(editor->document())});
                    m_stepListWidget->addItem(m_steps.last().name);
                }
            }
        }
        traverseSteps(domNode);
        domNode = domNode.nextSibling();
    }
}

void EditorWindow::traverseExternalVariables(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();
    QSqlQuery query(DBConfig::instance()->configDatabase());

    while (!domNode.isNull()) {
        if (domNode.isElement()) {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()) {
                if (domElement.tagName() == "ExternalVariable") {
                    //Проверка на наличие переменной в БД
                    if (query.exec(QString("SELECT * FROM `ParametrAliases` WHERE alias = '%1';")
                                   .arg(domElement.attribute("Alias")))) {
                        if(query.first()) {
                            QListWidgetItem *item = new QListWidgetItem(query.record().value("alias").toString());
                            item->setToolTip(query.record().value("description").toString());
                            m_extVarListWidget->addItem(item);
                        }
                        else
                            qDebug()<<"Переменная" + domElement.attribute("Alias") + "не найдена в БД";
                    }
                    else
                        qDebug()<<"Не удалось связаться с БД";
                }
            }
        }
        traverseSteps(domNode);
        domNode = domNode.nextSibling();
    }
}

void EditorWindow::travenreInternalVariables(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();

    while (!domNode.isNull()) {
        if (domNode.isElement()) {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()) {
                if (domElement.tagName() == "InternalVariable") {

                    QListWidgetItem *item = new QListWidgetItem(domElement.attribute("Name", tr("Неизвестная переменная")));
                    item->setToolTip(domElement.attribute("Description", ""));
                    item->setData(Qt::UserRole, domElement.attribute("Writable", "1"));
                    m_intVarListWidget->addItem(item);
                }
            }
        }
        traverseSteps(domNode);
        domNode = domNode.nextSibling();
    }
}

QDomDocument EditorWindow::makeXml()
{
    QDomDocument xml;
    QDomElement insertingNode;

    QDomElement testNode = xml.createElement("Test");
    xml.appendChild(testNode);
    QDomElement variablesNode = xml.createElement("Variables");
    testNode.appendChild(variablesNode);
    QDomElement internalVariablesNode = xml.createElement("InternalVariables");
    variablesNode.appendChild(internalVariablesNode);
    QDomElement externalVariablesNode = xml.createElement("ExternalVariables");
    variablesNode.appendChild(externalVariablesNode);
    QDomElement stepsNode = xml.createElement("Steps");
    testNode.appendChild(stepsNode);

    for (int i = 0; i < m_intVarListWidget->count(); i++) {
        insertingNode = xml.createElement("InternalVariable");
        insertingNode.setAttribute("Name", m_intVarListWidget->item(i)->text());
        insertingNode.setAttribute("Description", m_intVarListWidget->item(i)->toolTip());
        insertingNode.setAttribute("Writable", m_intVarListWidget->item(i)->data(Qt::UserRole).toBool());
        internalVariablesNode.appendChild(insertingNode);
    }

    for (int i = 0; i < m_extVarListWidget->count(); i++) {
        insertingNode = xml.createElement("ExternalVariable");
        insertingNode.setAttribute("Alias", m_extVarListWidget->item(i)->text());
        externalVariablesNode.appendChild(insertingNode);
    }

    for (int i = 0; i < m_steps.count(); i++) {
        if (i == 0)
            insertingNode = xml.createElement("Begin");
        else if (i == m_steps.count() - 1)
            insertingNode = xml.createElement("End");
        else
            insertingNode = xml.createElement("Step");

        insertingNode.setAttribute("Name", m_stepListWidget->item(i)->text());

        QDomText code = xml.createTextNode(m_steps.at(i).editor->toPlainText());
        insertingNode.appendChild(code);

        stepsNode.appendChild(insertingNode);
    }
    return xml;
}

bool EditorWindow::saveConfiguration(const QDomDocument &xml, const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        ErrorClassifier::instance()->showError(38);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(38));
        return false;
    }

    QTextStream stream(&file);
    xml.save(stream, 4);
    file.close();
    m_xml = xml;
    m_statusBar->showMessage(tr("Сохранено"));
    return true;
}

void EditorWindow::save()
{
    saveConfiguration(makeXml(), m_filePath);
}

void EditorWindow::saveAs()
{
    QString extension = ".script";
    QString caption = tr("Сохранить скрипт");
    QString directory = QApplication::applicationDirPath();
    QString filter = QString("Файл скрипта (*%1)").arg(extension);

    QString fileName = QFileDialog::getSaveFileName(this, caption, directory, filter);

    if (fileName.isEmpty())
        return;

    if (!fileName.contains(extension))
        fileName += extension;

    saveConfiguration(makeXml(), fileName);
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
    QDomDocument currentConfig = makeXml();
    if (currentConfig.toString(4) != m_xml.toString(4)) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Сохранение изменений"));
        msgBox.setText(tr("Сохранить изменения?"));
        msgBox.setIcon(QMessageBox::Question);

        QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(tr("&Нет"), QMessageBox::NoRole);
        QPushButton *cancelButton = msgBox.addButton(tr("&Отмена"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(cancelButton);

        msgBox.exec();
        if (msgBox.clickedButton() == yesButton) {
            if (saveConfiguration(currentConfig, m_filePath))
                event->accept();
            else {
                m_statusBar->showMessage(tr("Файл не сохранен!"));
                event->ignore();
            }
        }
        else if (msgBox.clickedButton() == noButton)
            event->accept();
        else if (msgBox.clickedButton() == cancelButton) {
            event->ignore();
            return;
        }
    }

    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    settings->endGroup();
    settings->sync();

    QMainWindow::closeEvent(event);
}

void EditorWindow::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();

    switch (event->key())
    {
    case Qt::Key_Up: {
        for (auto i : m_steps) {
            if (modifiers & Qt::ControlModifier) {
                QFont font = i.editor->font();
                font.setPointSize(font.pointSize() + 1);
                i.editor->setFont(font);
            }
        }
    }
        break;

    case Qt::Key_Down: {
        for (auto i : m_steps) {
            if (modifiers & Qt::ControlModifier) {
                QFont font = i.editor->font();
                font.setPointSize(font.pointSize() - 1);
                i.editor->setFont(font);
            }
        }
    }
    }
}

void EditorWindow::stepAdd()
{
    bool ok;

    QString text = QInputDialog::getText(this,
                                         tr("Добавить шаг"),
                                         tr("Название нового шага:"), QLineEdit::Normal,
                                         QString(tr("Новый шаг")), &ok,
                                         Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    if (ok && !text.isEmpty())
    {
        CodeEditorPlainTextEdit *editor = new CodeEditorPlainTextEdit();
        editor->setPlainText(tr("//Редактирование алгоритмов"));
        int targetPos = m_steps.count() - 1;
        m_steps.insert(targetPos, { text, editor, new JSSyntaxHighlighter(editor->document())});
        m_stepListWidget->insertItem(targetPos, m_steps.at(targetPos).name);
        m_stepWindows->addWidget(editor);
        m_stepListWidget->setCurrentRow(targetPos);
    }
}

void EditorWindow::stepRemove()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Удаление шага"));
    msgBox.setText(tr("Вы действительно хотите удалить шаг \"%1\"?").arg(m_stepListWidget->currentItem()->text()));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();
    if (msgBox.clickedButton() == yesButton)
    {
        delete m_stepWindows->currentWidget();
        delete m_stepListWidget->takeItem(m_stepListWidget->currentRow());
        m_steps.removeAt(m_stepListWidget->currentRow());
    }
}

void EditorWindow::stepEdit()
{
    bool ok;

    QString text = QInputDialog::getText(this,
                                         tr("Редактирование шага"),
                                         tr("Введите новое название:"), QLineEdit::Normal,
                                         m_stepListWidget->currentItem()->text(), &ok,
                                         Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);

    if (ok && !text.isEmpty()) {
        m_stepListWidget->currentItem()->setText(text);
    }
}

void EditorWindow::stepUp()
{
    int currentRow = m_stepListWidget->currentRow();

    if (currentRow > 0) {
        m_steps.swap(currentRow, currentRow - 1);
        QListWidgetItem *item = m_stepListWidget->takeItem(currentRow);
        m_stepListWidget->insertItem(currentRow - 1, item);
        m_stepListWidget->setCurrentItem(item);
    }
}

void EditorWindow::stepDown()
{
    int currentRow = m_stepListWidget->currentRow();

    if (currentRow < m_steps.count() - 1) {
        m_steps.swap(currentRow, currentRow + 1);
        QListWidgetItem *item = m_stepListWidget->takeItem(currentRow);
        m_stepListWidget->insertItem(currentRow + 1, item);
        m_stepListWidget->setCurrentItem(item);
    }
}

void EditorWindow::stepCopy()
{

    QString textStep = static_cast<CodeEditorPlainTextEdit*>(m_stepWindows->currentWidget())->toPlainText();
    QString textNameStep = m_stepListWidget->currentItem()->text();

    bool ok;

    QString text = QInputDialog::getText(this,
                                         tr("Копировать шаг"),
                                         tr("Название нового шага:"), QLineEdit::Normal,
                                         textNameStep, &ok,
                                         Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    if (ok && !text.isEmpty())
    {
        CodeEditorPlainTextEdit *editor = new CodeEditorPlainTextEdit();
        editor->setPlainText(textStep);
        int targetPos = m_steps.count() - 1;
        m_steps.insert(targetPos, { text, editor, new JSSyntaxHighlighter(editor->document())});
        m_stepListWidget->insertItem(targetPos, m_steps.at(targetPos).name);
        m_stepWindows->addWidget(editor);
        m_stepListWidget->setCurrentRow(targetPos);
    }

}

void EditorWindow::setupStepProperties(int step)
{
    if (m_stepListWidget->selectedItems().isEmpty()) {
        m_stepUpAction->setEnabled(false);
        m_stepDownAction->setEnabled(false);
        m_stepRemoveAction->setEnabled(false);
        m_stepEditAction->setEnabled(false);
        return;
    }
    m_undoAction->setEnabled(m_steps.at(step).editor->isUndoAvailable());
    m_redoAction->setEnabled(m_steps.at(step).editor->isRedoAvailable());
    m_copyAction->setEnabled(!m_steps.at(step).editor->textCursor().selectedText().isEmpty());
    m_cutAction->setEnabled(!m_steps.at(step).editor->textCursor().selectedText().isEmpty());
    m_stepWindows->setCurrentWidget(m_steps.at(step).editor);

    if(step == 0 || step == m_steps.count() - 1) {
        m_stepUpAction->setEnabled(false);
        m_stepDownAction->setEnabled(false);
        m_stepRemoveAction->setEnabled(false);
        m_stepEditAction->setEnabled(false);
    }
    else if(step == 1) {
        m_stepUpAction->setEnabled(false);
        m_stepDownAction->setEnabled(m_steps.count() > 3);
        m_stepRemoveAction->setEnabled(true);
        m_stepEditAction->setEnabled(true);
    }
    else if(step == m_steps.count() - 2) {
        m_stepUpAction->setEnabled(step > 1);
        m_stepDownAction->setEnabled(false);
        m_stepRemoveAction->setEnabled(true);
        m_stepEditAction->setEnabled(true);
    }
    else if (step == -1) {
        m_stepUpAction->setEnabled(false);
        m_stepDownAction->setEnabled(false);
        m_stepRemoveAction->setEnabled(false);
        m_stepEditAction->setEnabled(false);
    }
    else {
        m_stepUpAction->setEnabled(true);
        m_stepDownAction->setEnabled(true);
        m_stepRemoveAction->setEnabled(true);
        m_stepEditAction->setEnabled(true);
    }

    m_statusBar->showMessage(QString("Номер шага: %1").arg(step));

}

void EditorWindow::selectExternalVariables()
{
    ParameterAliasSelectWidget *aliasSelectionWidget = new ParameterAliasSelectWidget();
    aliasSelectionWidget->setWindowModality(Qt::ApplicationModal);
    aliasSelectionWidget->setupButtons(ParameterAliasSelectWidget::OkButton | ParameterAliasSelectWidget::CancelButton);
    aliasSelectionWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(aliasSelectionWidget, &ParameterAliasSelectWidget::itemSelected, [this, aliasSelectionWidget]()
    {
        for (auto i : aliasSelectionWidget->getView()->selectionModel()->selectedIndexes()) {
            if (m_extVarListWidget->findItems(i.data().toString(), Qt::MatchExactly).isEmpty() && !i.column()) {
                QListWidgetItem *item = new QListWidgetItem(i.data().toString());
                item->setToolTip(i.data(Qt::ToolTipRole).toString());
                m_extVarListWidget->addItem(item);
            }
        }
    });

    aliasSelectionWidget->show();
}

void EditorWindow::removeExternalVariable()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Удаление переменной"));
    msgBox.setText(tr("Вы действительно хотите удалить глобальную переменную из теста?\"%1\"?").arg(m_extVarListWidget->currentItem()->text()));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();
    if (msgBox.clickedButton() == yesButton)
        delete m_extVarListWidget->takeItem(m_extVarListWidget->currentRow());
}

void EditorWindow::setupExternalVariableProperties(int index)
{
    m_removeExtVarAction->setEnabled(index > -1);
    rehighlight();
}

void EditorWindow::addInternalVariable()
{
    QDialog dialog(this, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowCloseButtonHint);
    dialog.setWindowTitle(tr("Локальная переменная"));
    QFormLayout form(&dialog);

    QLineEdit *nameEdit = new QLineEdit(tr("Новая переменная"), &dialog);
    nameEdit->selectAll();
    form.addRow(tr("Название"), nameEdit);

    QLineEdit *descriptionEdit = new QLineEdit(&dialog);
    form.addRow(tr("Описание"), descriptionEdit);

    QComboBox *rightsCombo = new QComboBox(&dialog);
    rightsCombo->addItems(QStringList()<<tr("Только чтение")<<tr("Чтение и запись"));
    form.addRow(tr("Доступ"), rightsCombo);

    QDialogButtonBox buttonBox(&dialog);
    QPushButton *okButton = new QPushButton(tr("Ок"));
    connect(okButton, &QPushButton::clicked, [this, &dialog, nameEdit]()
    {
        if(nameEdit->text().isEmpty()) {
            ErrorClassifier::instance()->showError(39);
            return;
        }
        else if (!m_intVarListWidget->findItems(nameEdit->text(), Qt::MatchExactly).isEmpty()) {
            ErrorClassifier::instance()->showError(40);
            return;
        }
        dialog.accept();
    });
    buttonBox.addButton(okButton, QDialogButtonBox::QDialogButtonBox::AcceptRole);

    QPushButton *cancelButton = new QPushButton(tr("Отмена"));
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    buttonBox.addButton(cancelButton, QDialogButtonBox::RejectRole);

    form.addRow(&buttonBox);

    if (dialog.exec() == QDialog::Accepted) {
        QListWidgetItem *item = new QListWidgetItem(nameEdit->text());
        item->setToolTip(descriptionEdit->text());
        m_intVarListWidget->insertItem(m_intVarListWidget->selectedItems().isEmpty() ? m_intVarListWidget->count()
                                                                                     : m_intVarListWidget->currentRow(), item);
        item->setData(Qt::UserRole, QVariant(rightsCombo->currentIndex()));
        m_intVarListWidget->setCurrentItem(item);
    }
}

void EditorWindow::removeInternalVariable()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Удаление переменной"));
    msgBox.setText(tr("Вы действительно хотите удалить локальную переменную из теста?\"%1\"?").arg(m_intVarListWidget->currentItem()->text()));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();
    if (msgBox.clickedButton() == yesButton)
        delete m_intVarListWidget->takeItem(m_intVarListWidget->currentRow());
}

void EditorWindow::editInternalVariable()
{
    QDialog dialog(this);
    dialog.setWindowTitle(tr("Локальная переменная"));
    QFormLayout form(&dialog);

    QLineEdit *nameEdit = new QLineEdit(m_intVarListWidget->currentItem()->text(), &dialog);
    form.addRow(tr("Название"), nameEdit);
    nameEdit->selectAll();

    QLineEdit *descriptionEdit = new QLineEdit(m_intVarListWidget->currentItem()->toolTip(), &dialog);
    form.addRow(tr("Описание"), descriptionEdit);

    QComboBox *rightsCombo = new QComboBox(&dialog);
    rightsCombo->addItems(QStringList()<<tr("Только чтение")<<tr("Чтение и запись"));
    rightsCombo->setCurrentIndex(m_intVarListWidget->currentItem()->data(Qt::UserRole).toInt());
    form.addRow(tr("Доступ"), rightsCombo);

    QDialogButtonBox buttonBox(&dialog);
    QPushButton *okButton = new QPushButton(tr("Ок"));
    connect(okButton, &QPushButton::clicked, [this, &dialog, nameEdit]()
    {
        if(nameEdit->text().isEmpty()) {
            ErrorClassifier::instance()->showError(41);
            return;
        }
        dialog.accept();
    });
    buttonBox.addButton(okButton, QDialogButtonBox::QDialogButtonBox::AcceptRole);

    QPushButton *cancelButton = new QPushButton(tr("Отмена"));
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    buttonBox.addButton(cancelButton, QDialogButtonBox::RejectRole);

    form.addRow(&buttonBox);

    if(dialog.exec() == QDialog::Accepted) {
        m_intVarListWidget->currentItem()->setText(nameEdit->text());
        m_intVarListWidget->currentItem()->setToolTip(descriptionEdit->text());
        m_intVarListWidget->currentItem()->setData(Qt::UserRole, QVariant(rightsCombo->currentIndex()));
    }
}

void EditorWindow::setupInternalVariableProperties(int index)
{
    bool state = index > -1;
    m_removeIntVarAction->setEnabled(state);
    m_editIntVarAction->setEnabled(state);

    rehighlight();
}

void EditorWindow::rehighlight()
{
    for (auto i : m_steps) {
        QTextCharFormat format;
        i.highlighter->clearHighlight();
        if (m_extVarListWidget->currentItem() && m_highlightExtVarAction->isChecked()) {
            format.setBackground(QColor(210, 190, 210));
            format.setFontWeight(QFont::Bold);
            i.highlighter->highlight(QStringList()<<m_extVarListWidget->currentItem()->text(), format);
        }

        if (m_intVarListWidget->currentItem() && m_highlightIntVarAction->isChecked()) {
            format.setBackground(QColor(150, 210, 130));
            format.setFontWeight(QFont::Bold);
            i.highlighter->highlight(QStringList()<<m_intVarListWidget->currentItem()->text(), format);
        }
    }
}
