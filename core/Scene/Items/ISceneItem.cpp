#include "ISceneItem.h"

ISceneItem::ISceneItem(bool resizeable, QWidget *parent) : QFrame(parent)
{
    m_sizeGrip = resizeable ? new QSizeGrip(this) : nullptr;

    setBackgroundColor(Qt::white);
    frame = false;
    focused = false;
}

QFont ISceneItem::getFont()
{
    return QFrame::font();
}

void ISceneItem::setFont(const QFont &font)
{
    QFrame::setFont(font);
}

QColor ISceneItem::getFontColor()
{
    return QColor();
}

void ISceneItem::setFontColor(const QColor &color)
{
    Q_UNUSED(color)
}

QColor ISceneItem::getBackgroundColor()
{
    return palette().background().color();
}

void ISceneItem::setBackgroundColor(QColor color)
{
    QPalette pal = palette();
    pal.setBrush(QPalette::Background, color);
    setPalette(pal);
}

void ISceneItem::setFrame(bool state)
{
    setFrameShape(state ? QFrame::Box : QFrame::NoFrame);
    if(focused)
        setFocused(true);
    frame = state;
}

bool ISceneItem::getFrame()
{
    return frame;
}

void ISceneItem::setFocused(bool state)
{
    focused = state;
    if(focused)
    {
        setFrameShape(QFrame::Box);
        setLineWidth(3);
    }
    else
    {
        setFrame(frame);
        setLineWidth(1);
    }
}

bool ISceneItem::getFocused()
{
    return focused;
}

void ISceneItem::resizeEvent(QResizeEvent *event)
{
    if (event->size() == event->oldSize())
        return;

    QFrame::resizeEvent(event);
    emit resized(size());
}
