#ifndef DEVICESWINDOW_H
#define DEVICESWINDOW_H

#include "Common/PrefixHeader.h"

class ActionStyledWidget;

class DevicesTabWidget;
class DevicesWindow  : public QWidget
{
    Q_OBJECT

public:
    explicit DevicesWindow(QWidget *parent = nullptr);
    ~DevicesWindow();

private:
    QAction *m_controllerAction = nullptr;
    QAction *m_energoFormAction = nullptr;
    QAction *m_upsAction = nullptr;
    QAction *m_voltmetrAction = nullptr;
    QAction *m_frecurencyConvertorAction = nullptr;
    QAction *m_nlRizAction = nullptr;
    QAction *m_mktnAction = nullptr;
    QAction *m_mktn2Action = nullptr;

    ActionStyledWidget *m_controllerWidget = nullptr;
    ActionStyledWidget *m_energoFormWidget = nullptr;
    ActionStyledWidget *m_upsWidget = nullptr;
    ActionStyledWidget *m_voltmetrWidget = nullptr;
    ActionStyledWidget *m_frecurencyConvertorWidget = nullptr;
    ActionStyledWidget *m_nlRizActionWidget = nullptr;
    ActionStyledWidget *m_mktnWidget = nullptr;
    ActionStyledWidget *m_mktn2Widget = nullptr;

    DevicesTabWidget *m_devicesTabWidget = nullptr;

private slots:
    void devicesTabWidget(int index);
    void energoForm();

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
};

#endif // DEVICESWINDOW_H
