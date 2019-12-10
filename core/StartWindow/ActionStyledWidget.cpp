#include "ActionStyledWidget.h"
#include "Authentication/SystemUser.h"

ActionStyledWidget::ActionStyledWidget(int accessLevel, QAction *action, QLabel *descriptionLabel, const QIcon &pictureIcon, const QSize &iconSize, QWidget *parent)
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

    m_action = action;

    m_actionButton = new QPushButton ;
    m_actionButton->setIcon(m_pictureIcon);
    m_actionButton->setStyleSheet(QString("border:2px solid grey; border-radius: %1px;background-color: transparent;").arg(m_iconSize.rwidth()/2));
    m_actionButton->setFixedSize(m_iconSize);
    m_actionButton->setIconSize(QSize(m_iconSize.rwidth() * 0.8, m_iconSize.rheight() * 0.8));

    if (m_action != nullptr)
        connect(m_actionButton, &QPushButton::clicked, m_action, &QAction::trigger);


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

ActionStyledWidget::~ActionStyledWidget()
{

}

void ActionStyledWidget::enterEvent(QEvent *event)
{
    if (SystemUser::instance()->access() >= m_accessLevel) {
        m_actionButton->setIconSize(m_iconSize);
        m_actionButton->setStyleSheet(QString("border:2px solid grey; border-radius: %1px;background-color: transparent;").arg(m_iconSize.rwidth()/2));
        m_descriptionLabel->setStyleSheet("color: darkblue");
    }
}

void ActionStyledWidget::leaveEvent(QEvent *event)
{
    if (SystemUser::instance()->access() >= m_accessLevel) {
        m_actionButton->setIconSize(QSize(m_iconSize.rwidth() * 0.8, m_iconSize.rheight() * 0.8));
        m_actionButton->setStyleSheet(QString("border:2px solid grey; border-radius: %1px;background-color: transparent;").arg(m_iconSize.rwidth()/2));
        m_descriptionLabel->setStyleSheet("color: black");
    }
}
