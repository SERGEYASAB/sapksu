#include "PushButtonPlus.h"
#include <QStylePainter>
#include <QStyleOptionButton>
#include <QRect>

PushButtonPlus::PushButtonPlus(QWidget *parent) : QPushButton(parent)
{

}

PushButtonPlus::PushButtonPlus(const QString &text, QWidget *parent) :
    QPushButton(text, parent)
{

}

PushButtonPlus::PushButtonPlus(const QIcon &icon, const QString &text, QWidget *parent)
    : QPushButton(icon, text, parent),
    spaceOfIconAndText(8)
{

}

void PushButtonPlus::paintEvent(QPaintEvent *event)
{
    QStylePainter stylePainter(this);
    QStyleOptionButton styleOption;
    initStyleOption(&styleOption);
    styleOption.text.clear();
    styleOption.icon = QIcon();

    stylePainter.drawControl(QStyle::CE_PushButton, styleOption);

    QRect rect = event->rect();
    rect.moveTo(icon().pixmap(iconSize()).width() + spaceOfIconAndText, 0);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    stylePainter.drawText(rect, text(), textOption);

    QPixmap Pixmap = icon().pixmap(iconSize());
    int y = qAbs(rect.height() - Pixmap.height()) / 2;
    const int x = 6;
    stylePainter.drawPixmap(x, y, Pixmap);

}

int PushButtonPlus::getSpaceOfIconAndText() const
{
    return spaceOfIconAndText;
}

void PushButtonPlus::setSpaceOfIconAndText(int value)
{
    spaceOfIconAndText = value;
}
