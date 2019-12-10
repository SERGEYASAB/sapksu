#ifndef IRZFCWIDGET_H
#define IRZFCWIDGET_H

#include "Common/PrefixHeader.h"
#include "Devices/Device.h"

class INode;

class IRZFCWidget : public QWidget
{
    Q_OBJECT
public:

    enum IRZFCWidgetMode {
        Full    = 0,
        Cut     = 1
    };
    Q_ENUM(IRZFCWidgetMode)

    explicit IRZFCWidget(const IRZFCWidgetMode &value = IRZFCWidgetMode::Full, QWidget *parent = 0);

    void setMode(const IRZFCWidgetMode &value);

signals:

public slots:

private slots:
    void configurationChanged();
    void writeVoltageInput();
    void start();
    void stop();
    bool showConfirmChangeValueMessage(const QString &text) const;

private:
    INode *findNodeByName(QList<INode *> &container, const QString &nodeName) const;

    Device *v1;
    QSpinBox *m_voltageInputLineEdit;
    QDoubleSpinBox  *m_k1;
    QDoubleSpinBox  *m_k2;
    QPushButton *m_voltageInputButton;
    QAction *m_startAction = nullptr;
    QAction *m_stopAction = nullptr;

    IRZFCWidgetMode m_mode = IRZFCWidgetMode::Full;
};

#endif // IRZFCWIDGET_H
