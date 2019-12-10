#ifndef PUSHBUTTONPLUS_H
#define PUSHBUTTONPLUS_H

#include <QObject>
#include <QtWidgets>

class PushButtonPlus : public QPushButton
{
    Q_OBJECT

public:
    explicit PushButtonPlus(QWidget* parent = Q_NULLPTR);
    explicit PushButtonPlus(const QString &text, QWidget *parent = Q_NULLPTR);
    explicit PushButtonPlus(const QIcon &icon, const QString &text, QWidget *parent = Q_NULLPTR);

    int getSpaceOfIconAndText() const;
    void setSpaceOfIconAndText(int value);

protected:
    void paintEvent(QPaintEvent *event) override;

    int spaceOfIconAndText;


};

#endif // PUSHBUTTONPLUS_H
