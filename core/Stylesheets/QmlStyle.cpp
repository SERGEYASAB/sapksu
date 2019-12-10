#include "QmlStyle.h"
#include <QQuickStyle>

QmlStyle::QmlStyle():QObject()
{
    m_style = QQuickStyle::name();

}

QString QmlStyle::style()
{
    if(m_style == nullptr){}
  return m_style;
}

void QmlStyle::setstyle(const QString &style)
{
    if(style==m_style){
       return;
    }
    m_style = style;
        emit styleChanged();
}

QString QmlStyle::theme()
{
    return m_theme;
}

void QmlStyle::settheme(const QString &theme)
{
    if(theme==m_theme){
       return;
    }
    m_theme = theme;
        emit themeChanged();
}
