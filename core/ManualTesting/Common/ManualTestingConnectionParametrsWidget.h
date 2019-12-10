#ifndef MANUALTESTINGCONNECTIONPARAMETRSWIDGET_H
#define MANUALTESTINGCONNECTIONPARAMETRSWIDGET_H

#include "Common/PrefixHeader.h"

namespace Ui {
    class ManualTestingConnectionParametrsWidget;
}

class KSUPortConnectionWidget;
class InterferenceSender;
class Ksu735Node;

class ManualTestingConnectionParametrsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManualTestingConnectionParametrsWidget(QWidget *parent = 0);
    ~ManualTestingConnectionParametrsWidget();

private slots:
    void editSettingsClicked();
    void noInterferenceRadioButtonChecked();
    void randomInterferenceRadioButtonChecked();
    void fixedInterferenceRadioButtonChecked();
    void presetInterferenceRadioButtonChecked();
    void randomCountInterferenceSpinBoxChanged(int count);
    void fixedCountInterferenceSpinBoxChanged(int count);
    void prefixInterferenceLineEditTextChanged(const QString &text);
    void postfixInterferenceLineEditTextChanged(const QString &text);

private:
    Ui::ManualTestingConnectionParametrsWidget *ui;

    InterferenceSender *m_interferenceSender = nullptr;

    QPushButton *m_savePortSettingsButton;
    KSUPortConnectionWidget *m_portSettings = nullptr;

    QLabel *m_ksuAddressLabel = nullptr;
    QSpinBox *m_ksuAddressSpinBox = nullptr;

    QLabel *m_ksuTimeoutLabel = nullptr;
    QSpinBox *m_ksuTimeoutSpinBox = nullptr;
    QProcess *cmd = nullptr;
};

#endif // MANUALTESTINGCONNECTIONPARAMETRSWIDGET_H
