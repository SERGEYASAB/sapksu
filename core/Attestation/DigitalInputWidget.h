#ifndef DIGITALINPUTWIDGET_H
#define DIGITALINPUTWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;

class DigitalInputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DigitalInputWidget(const QString &inputName, const QString &inputAlias, QWidget *parent = 0);

signals:

public slots:

private:
    void testInputValue();

private:
    QGroupBox *m_groupBox = nullptr;

    QLabel *m_inputStateIconLabel = nullptr;
    QLabel *m_inputStateTextLabel = nullptr;

    QPixmap m_disabledPixmap;
    QPixmap m_enabledPixmap;

    IVariable *m_inputVariable = nullptr;
};

#endif // DIGITALINPUTWIDGET_H
