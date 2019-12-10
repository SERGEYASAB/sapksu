#ifndef MODBUSKSUMANUALSELECTIVETESTINGWIDGET_H
#define MODBUSKSUMANUALSELECTIVETESTINGWIDGET_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualSelectiveTestingParametr.h"
#include "ModbusKsuManualSelectiveTestingModel.h"

class ModbusKsuNode;

namespace Ui {
class ModbusKsuManualSelectiveTestingWidget;
}

class ModbusKsuManualSelectiveTestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusKsuManualSelectiveTestingWidget(ModbusKsuNode *node, QWidget *parent = 0);
    ~ModbusKsuManualSelectiveTestingWidget();

public slots:
    void stopTesting();

private slots:
    void requestReadVariables();
    void requestWriteVariables();
    void requestClear();

    void loadSettings();
    void saveSettings();
    void clearSettings();

private:
    void startStopTest();

private:
    Ui::ModbusKsuManualSelectiveTestingWidget *ui;

    ModbusKsuNode *m_node = nullptr;

    ModbusKsuManualSelectiveTestingModel *m_model = nullptr;
    ModbusKsuManualSelectiveTestingParametrsList m_parametrsList;

    volatile bool m_testing = false;
};

#endif // MODBUSKSUMANUALSELECTIVETESTINGWIDGET_H
