#ifndef KSU735MANUAL0X20TESTINGWIDGET_H
#define KSU735MANUAL0X20TESTINGWIDGET_H

#include "Common/PrefixHeader.h"
#include "Ksu735Manual0x20TestingParametr.h"
#include "Ksu735Manual0x20TestingModel.h"

class Ksu735Node;

namespace Ui {
    class Ksu735Manual0x20TestingWidget;
}

class Ksu735Manual0x20TestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Ksu735Manual0x20TestingWidget(Ksu735Node *node, QWidget *parent = nullptr);
    ~Ksu735Manual0x20TestingWidget();

private slots:
    void request0x20();
    void request0x40();
    void requestClear();

    void loadSettings();
    void saveSettings();
    void clearSettings();

private:
//    void showMessage(QMessageBox::Icon icon, const QString &title, const QString &)

private:
    Ui::Ksu735Manual0x20TestingWidget *ui;

    Ksu735Node *m_node = nullptr;

    Ksu735Manual0x20TestingModel *m_model;
    Ksu735Manual0x20TestingParametrsList m_parametrsList;
};

#endif // KSU735MANUAL0X20TESTINGWIDGET_H
