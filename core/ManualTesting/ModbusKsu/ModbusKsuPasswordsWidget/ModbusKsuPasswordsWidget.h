#ifndef MODBUSKSUPASSWORDSWIDGET_H
#define MODBUSKSUPASSWORDSWIDGET_H

#include "Common/PrefixHeader.h"

class ModbusKsuNode;

namespace Ui {
    class ModbusKsuPasswordsWidget;
}

class ModbusKsuPasswordsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusKsuPasswordsWidget(ModbusKsuNode *node, QWidget *parent = 0);
    ~ModbusKsuPasswordsWidget();

private slots:
    void readFirstPassword();
    void writeFirstPassword();

    void readSecondPassword();
    void writeSecondPassword();

private:
    ModbusKsuNode *m_node = nullptr;

    Ui::ModbusKsuPasswordsWidget *ui;
};

#endif // MODBUSKSUPASSWORDSWIDGET_H
