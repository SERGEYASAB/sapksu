
#include "SystemUser_copy.h"

SystemUser_copy *SystemUser_copy::instance()
{
    static SystemUser_copy singletone;
    return &singletone;
}

QString SystemUser_copy::description() const
{
    return m_description;
}

void SystemUser_copy::setDescription(const QString &description)
{
    m_description = description;
}

bool SystemUser_copy::passwordCorrect(){
    return m_passwordCorrect;
}

void SystemUser_copy::setPasswordCorrect(bool passwordcorrect){
    if(m_passwordCorrect == passwordcorrect){
        return;
    }
    m_passwordCorrect=passwordcorrect;
}

int SystemUser_copy::theme(){
    return m_theme;
}

void  SystemUser_copy::settheme(const int theme){

    if(theme==m_theme){
        return;
    }
    m_theme = theme;
}


QString SystemUser_copy::openedWindow(){
    return m_openedWindow;
}
void SystemUser_copy::setOpenedWindow(QString openwindow){
    m_openedWindow = openwindow;
}

SystemUser_copy::SystemUser_copy()
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
    m_theme = 1;
    setDescription("Гость");
    setDescription("Неавторизованный пользователь \"Гость\"");

}

SystemUser_copy::~SystemUser_copy()
{

}
