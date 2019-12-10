#ifndef SCENETEXTWIDGET_H
#define SCENETEXTWIDGET_H

#include "Common/PrefixHeader.h"
#include "ISceneItem.h"

class SceneTextWidget : public ISceneItem
{
public:
    SceneTextWidget(const QString &text, bool resizeable = false, QWidget *parent = 0);
    QFont getFont() override;
    void setFont(const QFont &font) override;
    QColor getFontColor() override;
    void setFontColor(const QColor &color) override;
    QString getText();
    void setText(const QString &text);

public slots:

private:
    QVBoxLayout *m_mainLayout;
    QLabel *m_label;
};

#endif // SCENETEXTWIDGET_H
