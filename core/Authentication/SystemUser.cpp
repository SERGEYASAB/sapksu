#include "SystemUser.h"

SystemUser *SystemUser::instance()
{
    static SystemUser singletone;
    return &singletone;
}

int SystemUser::access() const
{
    return m_access;
}

void SystemUser::setAccess(int accessLevel)
{
    m_access = accessLevel;
}

QString SystemUser::user() const
{
    return m_user;
}

void SystemUser::setUser(const QString &userName)
{
    m_user = userName;
}

QString SystemUser::description() const
{
    return m_description;
}

void SystemUser::setDescription(const QString &description)
{
    m_description = description;
}

SystemUser::SystemUser()
{

    /* Не удалять
#ifdef QT_NO_DEBUG
    setUser("Гость");
    setAccess(ACCESSLEVEL_USER);
    setDescription("Неавторизованный пользователь \"Гость\"");
#endif

#ifdef QT_DEBUG
    setUser("Администратор");
    setAccess(ACCESSLEVEL_ADMIN);
    setDescription("Режим отладки - режим по умолчанию");
#endif
*/

    setUser("Гость");
    setAccess(ACCESSLEVEL_USER);
    setDescription("Неавторизованный пользователь \"Гость\"");

}

SystemUser::~SystemUser()
{

}
