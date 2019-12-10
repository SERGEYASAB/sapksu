#ifndef SYSTEMUSER_H
#define SYSTEMUSER_H

#include "Common/PrefixHeader.h"

class SystemUser
{
public:
    static SystemUser *instance();

    int access() const;
    void setAccess(int access);

    QString user() const;
    void setUser(const QString &user);

    QString description() const;
    void setDescription(const QString &description);

private:

    int m_access;
    QString m_user;
    QString m_description;

    SystemUser(SystemUser const&) = delete;             // Copy construct
    SystemUser(SystemUser&&) = delete;                  // Move construct
    SystemUser& operator=(SystemUser const&) = delete;  // Copy assign
    SystemUser& operator=(SystemUser &&) = delete;      // Move assign

protected:
    explicit SystemUser();
    virtual ~SystemUser();
};

#endif // SYSTEMUSER_H
