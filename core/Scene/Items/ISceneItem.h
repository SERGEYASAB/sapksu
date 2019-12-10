#ifndef ISCENEITEM_H
#define ISCENEITEM_H

#include "Common/PrefixHeader.h"

class ISceneItem : public QFrame
{
    Q_OBJECT
public:
    ISceneItem(bool resizeable, QWidget *parent);

    virtual QFont getFont();
    virtual void setFont(const QFont &font);
    virtual QColor getFontColor();
    virtual void setFontColor(const QColor &color);
    virtual QColor getBackgroundColor();
    virtual void setBackgroundColor(QColor color);

    void setFrame(bool state);
    bool getFrame();
    void setFocused(bool state);
    bool getFocused();

signals:
    void resized(QSize size);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    bool frame;
    bool focused;
    QSizeGrip *m_sizeGrip;
};

#endif // ISCENEITEM_H
