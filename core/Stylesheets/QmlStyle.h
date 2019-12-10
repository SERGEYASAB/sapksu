#ifndef QMLSTYLE_H
#define QMLSTYLE_H

#include <QObject>
#include <QString>

class QmlStyle: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString style READ style WRITE setstyle NOTIFY styleChanged)
    Q_PROPERTY(QString theme READ theme WRITE settheme NOTIFY themeChanged)

public:
    QmlStyle();

    QString style();
    void setstyle(const QString &style);

    QString theme();
    void settheme(const QString &theme);

signals:
    void styleChanged();
    void themeChanged();

private:
    QString m_style;
    QString m_theme;
};

#endif // QMLSTYLE_H
