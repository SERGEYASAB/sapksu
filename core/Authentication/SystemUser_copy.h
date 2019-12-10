#ifndef SystemUser_copy_H
#define SystemUser_copy_H

#include "Common/PrefixHeader.h"

class SystemUser_copy
{
public:
    static SystemUser_copy *instance();

    QString description() const;
    void setDescription(const QString &description);

    bool passwordCorrect();
    void setPasswordCorrect(bool passwordcorrect);

    int theme();
    void settheme(const int theme);

    QString openedWindow();
    void setOpenedWindow(QString openwindow);

private:

    QString m_description;
    bool m_passwordCorrect = false;
    int m_theme;
    QString m_openedWindow;

    SystemUser_copy(SystemUser_copy const&) = delete;             // Copy construct
    SystemUser_copy(SystemUser_copy&&) = delete;                  // Move construct
    SystemUser_copy& operator=(SystemUser_copy const&) = delete;  // Copy assign
    SystemUser_copy& operator=(SystemUser_copy &&) = delete;      // Move assign

protected:
    explicit SystemUser_copy();
    virtual ~SystemUser_copy();
};

#endif // SystemUser_copy_H
