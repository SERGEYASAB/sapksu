#include "AppSettings.h"

AppSettings *AppSettings::instance()
{
    static AppSettings singletone;
    return &singletone;
}

SUPtr<QSettings> AppSettings::settings()
{
    return SUPtr<QSettings>(new QSettings(tr("settings.conf"), QSettings::IniFormat));
}

AppSettings::AppSettings(QObject *parent) : QObject(parent)
{

}

AppSettings::~AppSettings()
{
}
