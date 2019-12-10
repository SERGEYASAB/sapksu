#ifndef IOWIDGET_H
#define IOWIDGET_H

#include <QMainWindow>

class QTreeView;
class QTableView;
class DomModel;
class VariablesModel;

class IOWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit IOWidget(QWidget *parent = 0);
    ~IOWidget();

public slots:
    void showNode(const QModelIndex &index);
    void showVariable(const QModelIndex &index);

private:
    DomModel *m_domModel;
    VariablesModel *m_variablesModel;
    QTreeView *m_treeView;
    QTabWidget *m_tabWidget;
    QTableView *m_variablesView;
    QWidget *m_settingsView;
    QWidget *m_variableSettingsWindow;
};

#endif // IOWIDGET_H
