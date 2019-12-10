#ifndef ENERGOFORMDEVICEWIDGET_H
#define ENERGOFORMDEVICEWIDGET_H

#include "Common/PrefixHeader.h"

class EnergoFormNode;

namespace Ui {
    class EnergoFormDeviceWidget;
}

class EnergoFormDeviceWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EnergoFormDeviceWidget(EnergoFormNode *node, QWidget *parent = nullptr);
    ~EnergoFormDeviceWidget();

private slots:
    void startGeneration();
    void stopGeneration();

private:
    void connectUiSignals();
    void connectNodeSignals();

    void createLineEditValidators();
    void createUValidator();
    void createIValidator();
    void createAngleValidator();

private:
    Ui::EnergoFormDeviceWidget *ui;

    EnergoFormNode *m_node = nullptr;
};

#endif // ENERGOFORMDEVICEWIDGET_H
