#include "SceneTextWidget.h"

SceneTextWidget::SceneTextWidget(const QString &text, bool resizeable, QWidget *parent) : ISceneItem(resizeable, parent)
{
    setToolTip(tr("Текст"));

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_label = new QLabel(text);
    m_label->setAlignment(Qt::AlignCenter);

    QFont defaultFont;
    defaultFont.setFamily("Times New Roman");
    defaultFont.setPointSize(14);
    setFont(defaultFont);

    m_mainLayout->addWidget(m_label);
    setLayout(m_mainLayout);
}

QFont SceneTextWidget::getFont()
{
    return m_label->font();
}

void SceneTextWidget::setFont(const QFont &font)
{
    m_label->setFont(font);
}

QColor SceneTextWidget::getFontColor()
{
    return m_label->palette().foreground().color();
}

void SceneTextWidget::setFontColor(const QColor &color)
{
    QPalette pal = m_label->palette();
    pal.setColor(m_label->foregroundRole(), color);
    m_label->setPalette(pal);
}

QString SceneTextWidget::getText()
{
    return m_label->text();
}

void SceneTextWidget::setText(const QString &text)
{
    m_label->setText(text);
}
