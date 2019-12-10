#include "ActionStyledBooleanWidget.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "Authentication/SystemUser.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

void ActionStyledBooleanWidget::variableChanged()
{
    if (m_variable != nullptr) {
        QVariant value = m_variable->value();
        if (!value.isValid()) {
            m_borderColorString = "#A0A0A4";
        } else {
            if (!m_inverse) {
                if (value.toBool()) {
                    m_borderColorString = "#6AB700";
                } else {
                    m_borderColorString = "#EC4B22";
                }
            } else {
                if (!value.toBool()) {
                    m_borderColorString = "#6AB700";
                } else {
                    m_borderColorString = "#EC4B22";
                }
            }
        }
    } else {
        m_borderColorString = "#A0A0A4";
    }

    m_actionButton->setStyleSheet(QString("border:8px solid %2; border-radius: %1px; background-color: transparent;").arg(m_iconSize.rwidth()/2).arg(m_borderColorString));
}

ActionStyledBooleanWidget::ActionStyledBooleanWidget(const QString &alias, int accessLevel, QLabel *descriptionLabel, const QIcon &pictureIcon, const QSize &iconSize, QWidget *parent)
    : QWidget(parent),
      m_accessLevel(accessLevel),
      m_pictureIcon(pictureIcon)
{
    m_iconSize = iconSize;

    if (descriptionLabel == nullptr) {
        m_descriptionLabel = new QLabel(tr("undefined"));
    } else {
        m_descriptionLabel = descriptionLabel;
        m_descriptionLabel->setAlignment(Qt::AlignCenter);
    }

    m_actionButton = new QPushButton();
    m_actionButton->setIcon(m_pictureIcon);
    m_actionButton->setStyleSheet(QString("border:8px solid %2; border-radius: %1px; background-color: transparent;").arg(m_iconSize.rwidth()/2).arg(m_borderColorString));
    m_actionButton->setFixedSize(m_iconSize);
    m_actionButton->setIconSize(QSize(m_iconSize.rwidth() * 0.8, m_iconSize.rheight() * 0.8));

    connect(m_actionButton, &QPushButton::clicked, this, &ActionStyledBooleanWidget::actionButtonClicked);

    m_variable = IOServer::instance()->variableByAlias(alias);

    if (m_variable != nullptr) {
        connect(m_variable, &IVariable::valueChanged, this, &ActionStyledBooleanWidget::variableChanged);
        m_variable->readValue();
        variableChanged();
    }

    if (SystemUser::instance()->access() < m_accessLevel) {
        m_actionButton->setDisabled(true);
    }

    m_vlayoutWidget = new QVBoxLayout;
    m_vlayoutWidget->addWidget(m_actionButton, 0, Qt::AlignCenter);
    m_vlayoutWidget->addSpacing(0);
    m_vlayoutWidget->addWidget(m_descriptionLabel, 0, Qt::AlignCenter);
    m_vlayoutWidget->addStretch();

    setLayout(m_vlayoutWidget);
}

void ActionStyledBooleanWidget::actionButtonClicked()
{
    if (!m_writable)
        return;

    if (!m_variable)
        return;

    QMessageBox msgBox;

    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(QStringLiteral("Подтверждение"));
    msgBox.setText(QString("Вы действительно хотите изменить состояние выхода %1?").arg(m_variable->alias()));
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    if (msgBox.exec() == QMessageBox::Ok) {

        if (m_variable != nullptr) {
            if (subActions(m_variable)) {
                m_variable->writeValue(!m_variable->readValue().toBool());
            }
        }
    }
}

void ActionStyledBooleanWidget::enterEvent(QEvent *event)
{
    if (SystemUser::instance()->access() >= m_accessLevel && m_writable) {
        m_actionButton->setIconSize(m_iconSize);
        m_actionButton->setStyleSheet(QString("border:8px solid %2; border-radius: %1px; background-color: transparent;").arg(m_iconSize.rwidth()/2).arg(m_borderColorString));
        m_descriptionLabel->setStyleSheet("color: darkblue");
    }
    QWidget::enterEvent(event);
}

void ActionStyledBooleanWidget::leaveEvent(QEvent *event)
{
    if (SystemUser::instance()->access() >= m_accessLevel && m_writable) {
        m_actionButton->setIconSize(QSize(m_iconSize.rwidth() * 0.8, m_iconSize.rheight() * 0.8));
        m_actionButton->setStyleSheet(QString("border:8px solid %2; border-radius: %1px; background-color: transparent;").arg(m_iconSize.rwidth()/2).arg(m_borderColorString));
        m_descriptionLabel->setStyleSheet("color: black");
    }
    QWidget::leaveEvent(event);
}

// Костылевый метод, которые добавляет к определенным алиасам кнопок, дополнительные действия
bool ActionStyledBooleanWidget::subActions(IVariable *variable)
{
#ifdef SAP_KSU
    // При включении фазы питаний, необходимо отключать турбинку - ТАБУ
    if (variable->alias() == "$SAP_PHASE_A_ENABLE") {
        if (!variable->readValue().toBool()) {
            IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));
            if (!turbineRotation)
                return false;
            turbineRotation->setValue(false);
        }
    } else if (variable->alias() == "$SAP_PHASE_B_ENABLE") {
        if (!variable->readValue().toBool()) {
            IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));
            if (!turbineRotation)
                return false;
            turbineRotation->setValue(false);
        }
    } else if (variable->alias() == "$SAP_PHASE_C_ENABLE") {
        if (!variable->readValue().toBool()) {
            IVariable *turbineRotation = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_TURBINE_ROTATION"));
            if (!turbineRotation)
                return false;
            turbineRotation->setValue(false);
        }
    } else if (variable->alias() == "$SAP_TURBINE_ROTATION") {

        IVariable *phaseA = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_A_ENABLE"));
        IVariable *phaseB = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_B_ENABLE"));
        IVariable *phaseC = IOServer::instance()->variableByAlias(QStringLiteral("$SAP_PHASE_C_ENABLE"));

        if (phaseA->readValue().toBool() || phaseB->readValue().toBool() || phaseC->readValue().toBool()) {
            ErrorClassifier::instance()->showError(49);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(49));
            return false;
        }
    }
#endif
#ifdef SAP_SU
#endif
#ifdef MECHOS
#endif

    return true;
}

void ActionStyledBooleanWidget::setInverse(bool inverse)
{
    if (m_inverse != inverse) {
        m_inverse = inverse;
        variableChanged();
    }
}
