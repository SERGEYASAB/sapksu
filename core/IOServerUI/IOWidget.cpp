#include "IOWidget.h"
#include <QtWidgets>
#include "DomItem.h"
#include "DomModel.h"
#include "VariablesModel.h"
#include "IOServer/IOServer.h"
#include "IOServerUI/IOWidgetsFactory.h"

IOWidget::IOWidget(QWidget *parent) : QMainWindow(parent)
{

    m_variableSettingsWindow = nullptr;

    m_domModel = new DomModel(IOServer::instance()->rootItem());

    m_treeView = new QTreeView(this);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSortingEnabled(false);
    m_treeView->setHeaderHidden(true);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setModel(m_domModel);

    connect(m_treeView, &QTreeView::clicked, this, &IOWidget::showNode);

    QDockWidget *dockTree = new QDockWidget(tr("Подключения и узлы"));
    dockTree->setFeatures(QDockWidget::DockWidgetMovable);
    dockTree->setWidget(m_treeView);
    addDockWidget(Qt::LeftDockWidgetArea, dockTree);

    m_tabWidget = new QTabWidget;
    m_tabWidget->addTab(m_settingsView = new QWidget(), tr("Настройки узла"));
    m_tabWidget->addTab(m_variablesView = new QTableView(this), tr("Содержимое узла"));
    connect(m_variablesView, &QTableView::doubleClicked, this, &IOWidget::showVariable);

    setCentralWidget(m_tabWidget);

    QIcon windowIcon = QIcon(":/Images/connection-window.png");
    setWindowModality(Qt::ApplicationModal);
    setWindowIcon(windowIcon);
    setWindowTitle(tr("Конфигурация объектов"));
    resize(1280, 960);
}

IOWidget::~IOWidget()
{

}

void IOWidget::showNode(const QModelIndex &index)
{

    int currentIndex = m_tabWidget->currentIndex();

    DomItem *domItem = static_cast<DomItem *>(index.internalPointer());

    delete m_settingsView;

    m_settingsView = IOWidgetsFactory::modify(domItem->ptrNode());
    m_tabWidget->insertTab(0, m_settingsView, tr("Настройки узла"));

    m_variablesView->setModel(new VariablesModel(static_cast<DomItem *>(index.internalPointer())));

    m_tabWidget->setCurrentIndex(currentIndex);

}

void IOWidget::showVariable(const QModelIndex &index)
{
    if(m_variableSettingsWindow)
        delete m_variableSettingsWindow;

    m_variableSettingsWindow = IOWidgetsFactory::modify(static_cast<DomItem *>(index.internalPointer())->ptrNode());
    m_variableSettingsWindow->show();
}
