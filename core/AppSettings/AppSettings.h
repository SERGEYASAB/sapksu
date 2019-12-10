#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include "Common/PrefixHeader.h"

class AppSettings : public QObject
{
    Q_OBJECT

public:
    static AppSettings *instance();
    static SUPtr<QSettings> settings();

private:
    explicit AppSettings(QObject *parent = 0);
    ~AppSettings();

    Q_DISABLE_COPY(AppSettings)
};

#endif // APPSETTINGS_H
