#include "SwitchWidget.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"

SwitchWidget::SwitchWidget(const QString &header, QList<QAction*> positions, bool readOnly, QWidget *parent) :
    QGroupBox (parent)
{
    setTitle(header);
    setStyleSheet("QGroupBox {"
                  "border: 2px solid gray;"
                  "border-radius: 5px;"
                  "margin-top: 1ex;"
                  "}"
                  "QGroupBox::title {"
                  "subcontrol-origin: margin;"
                  "subcontrol-position: top center;"
                  "padding-top: -5px;"
                  "padding-left: 0px;"
                  "padding-right: 0px;"
                  "padding-bottom: 0px; "
                  "}");
    QFont titleFont;
    titleFont.setBold(true);
    setFont(titleFont);

    QVBoxLayout *groupLay = new QVBoxLayout();

    for (auto action : positions) {
        QRadioButton *button = new QRadioButton(action->text(), this);
        button->setStyleSheet("QRadioButton::indicator::unchecked {"
                              "image: url(:/Images/radio-button-off-icon.png);"
                              "}"
                              "QRadioButton::indicator::checked {"
                              "image: url(:/Images/radio-button-on-icon.png);"
                              "}");
        button->setDisabled(readOnly);
        m_positions[button] = action;
        if (action) {
            groupLay->addWidget(button);
            connect(button, &QRadioButton::pressed, action, [this, action, button]()
            {
                if (!button->isChecked()) {
                    if (QMessageBox::information(this,
                                                 tr("Подтверждение"),
                                                 tr("Для присвоения параметру <%1> значения <%2> нажмите \"Ок\"")
                                                 .arg(title()).arg(button->text()),
                                                 QMessageBox::Ok | QMessageBox::Cancel,
                                                 QMessageBox::Ok) == QMessageBox::Ok) {
                        action->trigger();
                        button->setChecked(true);
                        emit switched(UserSwitch);
                    }
                }
            });
        }
        else
            qDebug() << "Не определен экшн для переключателя " << header;
    }
    setLayout(groupLay);
}

void SwitchWidget::check(QAction *action)
{
    if (action && m_positions.values().contains(action) && m_positions.key(action)) {
        action->trigger();
        m_positions.key(action)->setChecked(true);
        emit switched(ProgramSwitch);
    }
}

void SwitchWidget::resetCheck()
{
    for (int i = 0; i < m_positions.count(); i++) {
        m_positions.keys().at(i)->setAutoExclusive(false);
        m_positions.keys().at(i)->setChecked(false);
        m_positions.keys().at(i)->setAutoExclusive(true);
    }
    emit switched(ProgramSwitch);
}

bool SwitchWidget::readOnly()
{
    return m_readOnly;
}

void SwitchWidget::setReadOnly(bool state)
{
    if (m_readOnly != state) {
        for (int i = 0; i < m_positions.count(); i++)
            m_positions.keys().at(i)->setDisabled(state);
        m_readOnly = state;
    }
}
