#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include "Common/PrefixHeader.h"

class SwitchWidget : public QGroupBox
{
    Q_OBJECT
public:
    enum SwitchReason {
        Unknown = 0,
        ProgramSwitch = 1,
        UserSwitch = 2
    };

    SwitchWidget(const QString &header, QList<QAction *> positions, bool readOnly = false, QWidget *parent = Q_NULLPTR);
    void check(QAction *action);
    void resetCheck();

    bool readOnly();
    void setReadOnly(bool state);

signals:
    void switched(SwitchReason reason);

private:
    QHash<QRadioButton *, QAction *> m_positions;
    bool m_readOnly;
};

#endif // SWITCHWIDGET_H
