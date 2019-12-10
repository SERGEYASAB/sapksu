#ifndef KSU735MANUAL0X30TESTINGWIDGET_H
#define KSU735MANUAL0X30TESTINGWIDGET_H

#include "Common/PrefixHeader.h"
#include "Ksu735Manual0x30TestingArchiveRecordsModel.h"

class Ksu735Node;
class Ksu735Manual0x30TestingModel;

namespace Ui {
    class Ksu735Manual0x30TestingWidget;
}

class Ksu735Manual0x30TestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Ksu735Manual0x30TestingWidget(Ksu735Node *node, QWidget *parent = nullptr);
    ~Ksu735Manual0x30TestingWidget();

private slots:
    void request0x30();

private:
    Ui::Ksu735Manual0x30TestingWidget *ui;

    Ksu735Node *m_node = nullptr;
    Ksu735Manual0x30TestingModel *m_model = nullptr;

    Ksu735Manual0x30TestingArchiveRecordsModel m_recordsModel;
};

#endif // KSU735MANUAL0X30TESTINGWIDGET_H
