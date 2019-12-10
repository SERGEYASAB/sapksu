#ifndef KSU735MANUAL0X10TESTINGWIDGET_H
#define KSU735MANUAL0X10TESTINGWIDGET_H

#include "Common/PrefixHeader.h"
#include "Ksu735Manual0x10TestingModel.h"

class Ksu735Node;
class Ksu735Variable;
class Ksu735Manual0x10TestingModel;

namespace Ui {
    class Ksu735Manual0x10TestingWidget;
}

class Ksu735Manual0x10TestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Ksu735Manual0x10TestingWidget(Ksu735Node *node, QWidget *parent = nullptr);
    ~Ksu735Manual0x10TestingWidget();

public slots:
    void stopTesting();

protected slots:
    void test();
    void request();
    void clear();

private:
    typedef Ksu735Manual0x10TestingModel::VariableValue VariableValue;
    typedef Ksu735Manual0x10TestingModel::VariableValueList VariableValueList;

    void testKsuConnection();

private:
    Ui::Ksu735Manual0x10TestingWidget *ui;

    Ksu735Node *m_node = nullptr;
    Ksu735Manual0x10TestingModel *m_model = nullptr;
    VariableValueList m_variableValues;

    volatile bool m_testing = false;

    QDateTime m_testBeginDt;
    int m_txCount, m_rxCount, m_errorCount;
};

#endif // KSU735MANUAL0X10TESTINGWIDGET_H
