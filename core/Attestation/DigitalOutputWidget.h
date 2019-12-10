#ifndef DIGITALOUTPUTWIDGET_H
#define DIGITALOUTPUTWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;

class DigitalOutputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DigitalOutputWidget(const QString &outputName, const QString &outputAlias, QWidget *parent = 0);

signals:

public slots:
    void disableOutput(bool checked);
    void enableOutput(bool checked);

private:
    QGroupBox *m_groupBox = nullptr;

    QRadioButton *m_disableRadioButton = nullptr;
    QRadioButton *m_enableRadioButton = nullptr;

    IVariable *m_outputVariable = nullptr;
    bool init = true;
};

#endif // DIGITALOUTPUTWIDGET_H
