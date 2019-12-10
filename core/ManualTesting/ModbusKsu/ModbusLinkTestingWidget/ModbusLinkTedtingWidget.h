#ifndef MODBUSLINKTEDTINGWIDGET_H
#define MODBUSLINKTEDTINGWIDGET_H


#include "Common/PrefixHeader.h"

class ModbusKsuNode;
class ModbusVariable;

namespace Ui {
    class ModbusLinkTedtingWidget;
}

class ModbusLinkTedtingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusLinkTedtingWidget(ModbusKsuNode *node, QWidget *parent = nullptr);
    ~ModbusLinkTedtingWidget();

    struct VariableValue {
        ModbusVariable *variable;
        QVariant value;
    };

public slots:
    void stopTesting();

protected slots:
    void test();
    void request();
    void clear();

private:
    void testKsuConnection();

private:
    Ui::ModbusLinkTedtingWidget *ui;

    ModbusKsuNode *m_node = nullptr;

    QList<VariableValue *> m_variableValues;

    volatile bool m_testing = false;

    QDateTime m_testBeginDt;
    int m_txCount, m_rxCount, m_errorCount;
};

#endif // MODBUSLINKTEDTINGWIDGET_H
