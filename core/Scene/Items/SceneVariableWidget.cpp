#include "SceneVariableWidget.h"

#include "IOServer/nsNode/INode.h"
#include "IOServer/nsVariable/IVariable.h"
#include "IOServer/IOServer.h"

SceneVariableWidget::SceneVariableWidget(const QString &alias, bool editable, bool resizeable, QWidget *parent) : ISceneItem(resizeable, parent)
{
    m_variable = nullptr;

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    setLayout(m_mainLayout);
    m_valueLabel = new QLabel(this);
    m_valueLabel->setAlignment(Qt::AlignCenter);

    QFont defaultFont;
    defaultFont.setFamily("Times New Roman");
    defaultFont.setPointSize(14);
    setFont(defaultFont);

    m_editable = editable;
    m_variable = nullptr;
    setAlias(alias);

    m_mainLayout->addWidget(m_valueLabel);
}

QString SceneVariableWidget::getAlias() const
{
    return m_alias;
}

void SceneVariableWidget::setAlias(const QString &alias)
{
    if (m_alias == alias)
        return;

    QString type;
    QString toolTip;

    m_alias = alias;

    toolTip = "Переменная. Псевдоним: " + getAlias();
    m_valueLabel->setText("0");


    if (!m_editable) {
        if (m_variable) {
            disconnect(m_variable, &IVariable::valueChanged, this, &SceneVariableWidget::refresh);
        }

        if (IOServer::instance()) {
            m_variable = IOServer::instance()->variableByAlias(alias);
        } else {
            m_variable = nullptr;
        }

        if (m_variable) {
            connect(m_variable, &IVariable::valueChanged, this, &SceneVariableWidget::refresh);
            QMetaEnum dataTypes = QMetaEnum::fromType<IVariable::DataType>();
            for (int i = 0; i < dataTypes.keyCount(); i++)
                if (m_variable->serverValueDataType() == IVariable::DataType(dataTypes.value(i)))
                    type = dataTypes.key(i);
        }

        if(!m_variable) {
            toolTip += " (НЕ НАЙДЕНА)";
            m_valueLabel->setText("?");
        }
        else {
            toolTip += ", Тип: " + (type.isEmpty() ? "Неизвестен" : type);
        }
    }
    setToolTip(toolTip);
}

IVariable *SceneVariableWidget::getVariable()
{
    return m_variable;
}

void SceneVariableWidget::setVariable(IVariable *var)
{
    if(m_variable == var)
        return;
    setAlias(var->alias());
}

void SceneVariableWidget::poll()
{
    qDebug()<<"Внеочередной запрос переменной " + getAlias();
}

QFont SceneVariableWidget::getFont()
{
    return m_valueLabel->font();
}

void SceneVariableWidget::setFont(const QFont &font)
{
    m_valueLabel->setFont(font);
}

QColor SceneVariableWidget::getFontColor()
{
    return m_valueLabel->palette().foreground().color();
}

void SceneVariableWidget::setFontColor(const QColor &color)
{
    QPalette pal = m_valueLabel->palette();
    pal.setColor(m_valueLabel->foregroundRole(), color);
    m_valueLabel->setPalette(pal);
}

void SceneVariableWidget::refresh()
{
    if (m_variable)
        m_valueLabel->setText(m_variable->value().toString());
}
