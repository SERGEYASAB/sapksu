#include "SceneControlPanel.h"
#include "Common/PushButtonPlus.h"
#include "Scene/SceneWidget.h"
#include "Scene/EditableScene.h"
#include "Scene/EditableSceneWidget.h"
#include "ReportManager/OperatorClickReport.h"
#include "ReportManager/FaultReport.h"
#include "StartWindow/ActionControlWidgetsShow.h"
#include "ErrorClassifier/ErrorClassifier.h"

SceneControlPanel::SceneControlPanel(SceneControlPanel::RunMode runMode, QWidget *parent)
    : QWidget(parent),
      m_runMode(runMode)
{
    ///Создание каталога
    QDir dir;
    dir.setPath(viewsFolderPath());
    if(!dir.exists()) {
        dir.setPath(QCoreApplication::applicationDirPath() + QDir::separator());
        dir.mkdir("Views");
    }

    ///Создание элементов
    m_viewListWidget = new QListWidget(this);
    refreshFileList();

    QSize iconSize = QSize(32, 32);

    m_createViewButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), "Создать");
    m_createViewButton->setIconSize(iconSize);
    m_createViewButton->setSpaceOfIconAndText(15);

    m_removeViewButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), "Удалить");
    m_removeViewButton->setIconSize(iconSize);
    m_removeViewButton->setSpaceOfIconAndText(15);

    m_editViewButton = new PushButtonPlus(QIcon(":/Images/preferences-system.png"), "Изменить");
    m_editViewButton->setIconSize(iconSize);
    m_editViewButton->setSpaceOfIconAndText(15);

    m_runViewButton = new PushButtonPlus(QIcon(":/Images/arrow-run.png"), "Выполнить");
    m_runViewButton->setIconSize(iconSize);
    m_runViewButton->setSpaceOfIconAndText(15);

    m_closeButton = new PushButtonPlus(QIcon(":/Images/application-exit.png"), "Закрыть");
    m_closeButton->setIconSize(iconSize);
    m_closeButton->setSpaceOfIconAndText(15);

    connect(m_createViewButton, &PushButtonPlus::clicked, this, &SceneControlPanel::createView);
    connect(m_removeViewButton, &PushButtonPlus::clicked, this, &SceneControlPanel::removeView);
    connect(m_editViewButton, &PushButtonPlus::clicked, this, &SceneControlPanel::editView);
    connect(m_runViewButton, &PushButtonPlus::clicked, this, &SceneControlPanel::runView);
    connect(m_closeButton, &QPushButton::clicked, this, &SceneControlPanel::close);

    m_buttonsLayout = new QVBoxLayout;
    m_buttonsLayout->addStrut(150);

    if (m_runMode == SceneControlPanel::RunMode::Editing) {
        m_buttonsLayout->addWidget(m_createViewButton);
        m_buttonsLayout->addWidget(m_removeViewButton);
        m_buttonsLayout->addWidget(m_editViewButton);
    }

    if (m_runMode == SceneControlPanel::RunMode::Execution) {
        m_buttonsLayout->addWidget(m_runViewButton);
    }

    m_buttonsLayout->addStretch();
    m_buttonsLayout->addWidget(m_closeButton);

    m_mainLayout = new QHBoxLayout;
    m_mainLayout->addWidget(m_viewListWidget);
    m_mainLayout->addLayout(m_buttonsLayout);

#ifdef QT_DEBUG
    QGroupBox *debugGroupBox = new QGroupBox("QT_DEBUG");

    QVBoxLayout *debugVBoxLayout = new QVBoxLayout;
    debugVBoxLayout->addStrut(150);
    debugVBoxLayout->addWidget(m_createViewButton);
    debugVBoxLayout->addWidget(m_removeViewButton);
    debugVBoxLayout->addWidget(m_editViewButton);
    debugVBoxLayout->addWidget(m_runViewButton);
    debugVBoxLayout->addStretch();
    debugVBoxLayout->addWidget(m_closeButton);

    debugGroupBox->setLayout(debugVBoxLayout);
    m_mainLayout->addWidget(debugGroupBox);

#endif

    setLayout(m_mainLayout);
}

QString SceneControlPanel::viewsFolderPath()
{
    return QCoreApplication::applicationDirPath() + QDir::separator() + "Views";
}

void SceneControlPanel::createView()
{
    setWindowState(Qt::WindowMinimized);

    EditableSceneWidget *editableScene = new EditableSceneWidget();
    editableScene->setAttribute(Qt::WA_DeleteOnClose, true);
    editableScene->setWindowModality(Qt::ApplicationModal);
    editableScene->setWindowTitle("Редактор графических панелей");
    editableScene->setWindowIcon(QIcon(":/Images/graphicPanelEditor.png"));
    editableScene->setWindowState(Qt::WindowMaximized);
    editableScene->show();
    editableScene->activateWindow();

    connect(editableScene, &EditableScene::destroyed, this, &SceneControlPanel::refreshFileList);
    connect(editableScene, &EditableSceneWidget::destroyed, this, &SceneControlPanel::showNormal);
}

void SceneControlPanel::runView()
{
    if (!m_viewListWidget->currentIndex().isValid())
        return;

    /*
    SceneWidget *scene = new SceneWidget(viewsFolderPath() + QDir::separator() + m_viewListWidget->currentItem()->text());
    scene->setAttribute(Qt::WA_DeleteOnClose, true);
    scene->show();
    */

    close();

    SceneWidget *scene = new SceneWidget(viewsFolderPath() + QDir::separator() + m_viewListWidget->currentItem()->text());
    scene->setAttribute(Qt::WA_DeleteOnClose, true);
    scene->setAttribute(Qt::WA_DeleteOnClose);
    scene->setWindowModality(Qt::WindowModal);
    scene->setWindowTitle("Графическое представление");
    scene->setWindowIcon(QIcon(":/Images/graphicPanelEditor.png"));
    scene->move(QApplication::desktop()->screenGeometry().center() - scene->rect().center());
    scene->showNormal();

    connect(this, &SceneControlPanel::destroyed, scene, &SceneWidget::showNormal);
}

void SceneControlPanel::refreshFileList()
{
    m_viewPaths.clear();
    m_viewListWidget->clear();
    for (auto fileName : static_cast<QDir>(viewsFolderPath() +
                                           QDir::separator()).entryList(QDir::Files)) {
        QFile testFile(viewsFolderPath() + QDir::separator() + fileName);

        if (!testFile.open(QIODevice::ReadOnly)) {
            qCritical() << tr("Не удается прочитать файл с содержанием представления");
            testFile.close();
            continue;
        }

        QString errorMsg;
        int errorLine = 0, errorColumn = 0;
        QDomDocument xmlTestFile;

        if (!xmlTestFile.setContent(&testFile, &errorMsg, &errorLine, &errorColumn)) {
            qCritical() << tr("Содержимое файла представления повреждено") << fileName << errorMsg << errorLine << errorColumn;
            testFile.close();
            continue;
        }

        m_viewPaths.append(testFile.fileName());
        m_viewListWidget->addItem(fileName);
        testFile.close();
    }
    m_viewListWidget->setCurrentRow(0);
}

void SceneControlPanel::closeEvent(QCloseEvent *event)
{
    if (m_runMode == Execution)
        OperatorClickReport::instance()->write(QStringLiteral("Окно графических панелей: закрытие окна"));
    else
        OperatorClickReport::instance()->write(QStringLiteral("Окно редактора графических панелей: закрытие окна"));
}

void SceneControlPanel::removeView()
{
    if (!m_viewListWidget->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Подтверждение удаления"));
    msgBox.setText(tr("Вы действительно хотите удалить представление ") + "\"" + m_viewListWidget->currentItem()->text() + "\"");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);
    msgBox.exec();

    if (msgBox.clickedButton() == noButton)
        return;

    QFile file(viewsFolderPath() + QDir::separator() + m_viewListWidget->currentItem()->text());
    if(!file.remove()) {
        ErrorClassifier::instance()->showError(37);
    } else {
        delete m_viewListWidget->takeItem(m_viewListWidget->currentRow());
    }
}

void SceneControlPanel::editView()
{
    if (!m_viewListWidget->currentIndex().isValid())
        return;

    setWindowState(Qt::WindowMinimized);

    EditableSceneWidget *editableScene = new EditableSceneWidget(viewsFolderPath() +
                                                                 QDir::separator() + m_viewListWidget->currentItem()->text());
    editableScene->setAttribute(Qt::WA_DeleteOnClose, true);
    editableScene->setWindowModality(Qt::ApplicationModal);
    editableScene->setWindowTitle("Редактор графических панелей");
    editableScene->setWindowIcon(QIcon(":/Images/graphicPanelEditor.png"));
    editableScene->setWindowState(Qt::WindowMaximized);
    editableScene->show();
    editableScene->activateWindow();

    connect(editableScene, &EditableScene::destroyed, this, &SceneControlPanel::refreshFileList);
    connect(editableScene, &EditableSceneWidget::destroyed, this, &SceneControlPanel::showNormal);
}
