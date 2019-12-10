#ifndef ACTIONSTYLEDBOOLEANWIDGET_H
#define ACTIONSTYLEDBOOLEANWIDGET_H

#include "Common/PrefixHeader.h"

class IVariable;
class ActionStyledBooleanWidget : public QWidget
{
public:

    explicit ActionStyledBooleanWidget(const QString &alias,
                                       int accessLevel = 0,
                                       QLabel *descriptionLabel = nullptr,
                                       const QIcon &pictureIcon = QIcon(":/Images/question_icon.png"),
                                       const QSize &iconSize = QSize(100, 100),
                                       QWidget *parent = nullptr);

    virtual ~ActionStyledBooleanWidget() {}
    bool subActions(IVariable *variable);

    inline bool isWritable() const { return m_writable; }
    inline void setWritable(bool writable) { m_writable = writable; }

    inline bool inverse() const { return m_inverse; }
    void setInverse(bool inverse);

private:
    IVariable *m_variable = nullptr;
    int m_accessLevel;
    QLabel *m_descriptionLabel = nullptr;
    QIcon m_pictureIcon;
    QSize m_iconSize;
    QPushButton *m_actionButton = nullptr;
    QVBoxLayout *m_vlayoutWidget = nullptr;
    QString m_borderColorString;

    bool m_writable = true;
    bool m_inverse = false;

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private slots:
    void variableChanged();
    void actionButtonClicked();
};

#endif // ACTIONSTYLEDBOOLEANWIDGET_H
