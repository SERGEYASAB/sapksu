#include "ActionStyledGroupMenuWidget.h"

ActionStyledGroupMenuWidget::ActionStyledGroupMenuWidget(const QString &name, QWidget *parent)
    : QWidget(parent)
{
    QGraphicsDropShadowEffect *shadowBlurEffect = new QGraphicsDropShadowEffect(this);
    shadowBlurEffect->setBlurRadius(20);
    shadowBlurEffect->setOffset(3);

    QLabel *label = new QLabel(name);
    label->setStyleSheet("background-color: grey; font: bold italic; font-size: 18px");
    label->setGraphicsEffect(shadowBlurEffect);

    QWidget *lineWidget = new QWidget();
    lineWidget->setFixedHeight(2);
    lineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineWidget->setStyleSheet(QString("background-color: grey"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label, 0 , Qt::AlignLeft);
    layout->addWidget(lineWidget);

    setLayout(layout);
}

ActionStyledGroupMenuWidget::ActionStyledGroupMenuWidget(const QString &name, int fontSize, QWidget *parent)
    : QWidget(parent)
{
    QGraphicsDropShadowEffect *shadowBlurEffect = new QGraphicsDropShadowEffect(this);
    shadowBlurEffect->setBlurRadius(20);
    shadowBlurEffect->setOffset(3);

    QLabel *label = new QLabel(name);
    label->setStyleSheet(QString("background-color: grey; font: bold italic; font-size: %1px").arg(fontSize));
    label->setGraphicsEffect(shadowBlurEffect);

    QWidget *lineWidget = new QWidget();
    lineWidget->setFixedHeight(2);
    lineWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineWidget->setStyleSheet(QString("background-color: white"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label, 0 , Qt::AlignLeft);
    layout->addWidget(lineWidget);

    setLayout(layout);
}

