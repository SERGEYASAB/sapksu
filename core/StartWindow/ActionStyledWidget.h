#ifndef ACTIONSTYLEDWIDGET_H
#define ACTIONSTYLEDWIDGET_H

#include "Common/PrefixHeader.h"

class ActionStyledWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ActionStyledWidget(int accessLevel = 0,
                                QAction *action = nullptr,
                                QLabel *descriptionLabel = nullptr,
                                const QIcon &pictureIcon = QIcon(":/Images/question_icon.png"),
                                const QSize &iconSize = QSize(100, 100),
                                QWidget *parent = nullptr);

    virtual ~ActionStyledWidget();

private:
    int m_accessLevel;
    QAction *m_action = nullptr;
    QLabel *m_descriptionLabel = nullptr;
    QIcon m_pictureIcon;
    QSize m_iconSize;
    QPushButton *m_actionButton;

    QVBoxLayout *m_vlayoutWidget;

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

};

#endif // ACTIONSTYLEDWIDGET_H
