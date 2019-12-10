#include "ParameterAliasWidget.h"
#include "ParameterAliasSelectWidget.h"

ParameterAliasWidget::ParameterAliasWidget(QWidget *parent) : QWidget(parent)
{
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_aliasLineEdit = new QLineEdit(this);
    m_aliasLineEdit->setReadOnly(true);
    m_aliasLineEdit->setFocus();
    m_aliasLineEdit->installEventFilter(this);
    m_mainLayout->addWidget(m_aliasLineEdit);

    m_aliasSelectToolButton = new QToolButton(this);
    m_aliasSelectToolButton->setText("...");
    connect(m_aliasSelectToolButton, &QToolButton::clicked, this, &ParameterAliasWidget::selectAlias);
    m_mainLayout->addWidget(m_aliasSelectToolButton);

    setLayout(m_mainLayout);
    //Необходимо для корректной работы делегата в таблице
    setFocusProxy(m_aliasLineEdit);
}

QString ParameterAliasWidget::selectedAlias()
{
    return m_aliasLineEdit->text();
}

bool ParameterAliasWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_aliasLineEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Backspace || keyEvent->matches(QKeySequence::Delete)) {
            clearAlias();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ParameterAliasWidget::selectAlias()
{
    ParameterAliasSelectWidget *parametrAliasWidget = new ParameterAliasSelectWidget(m_aliasLineEdit->text());
    parametrAliasWidget->setAttribute(Qt::WA_DeleteOnClose);
    parametrAliasWidget->setWindowModality(Qt::ApplicationModal);
    parametrAliasWidget->setWindowTitle("Список псевдонимов переменных");
    parametrAliasWidget->setWindowIcon(QIcon(":/Images/aliaces.png"));
    parametrAliasWidget->resize(QSize(800, 600));
    parametrAliasWidget->move(QApplication::desktop()->screenGeometry().center() - parametrAliasWidget->rect().center());
    parametrAliasWidget->show();
    parametrAliasWidget->activateWindow();

    connect(parametrAliasWidget, &ParameterAliasSelectWidget::itemSelected, this, &ParameterAliasWidget::setupAlias);
}

void ParameterAliasWidget::setupAlias(QString alias)
{
    m_aliasLineEdit->setText(alias);
    emit aliasSelected(alias);
}

void ParameterAliasWidget::clearAlias()
{
    m_aliasLineEdit->clear();
}
