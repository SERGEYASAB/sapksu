#ifndef STATICCONSTVALUES_H
#define STATICCONSTVALUES_H

#include <QString>

static const int ACCESSLEVEL_ADMIN      = 900;
static const int ACCESSLEVEL_ENGINEER   = 700;
static const int ACCESSLEVEL_OPERATOR   = 300;
static const int ACCESSLEVEL_USER       = 100;
static const int ACCESSLEVEL_ZERO       = 0;

// SAP_KSU
static const int VERSION_NUMBER_KSU_1   = 1;
static const int VERSION_NUMBER_KSU_2   = 0;
static const int VERSION_NUMBER_KSU_3   = 8;

// SAP_SU
static const int VERSION_NUMBER_SU_1   = 1;
static const int VERSION_NUMBER_SU_2   = 1;
static const int VERSION_NUMBER_SU_3   = 19;

// MECHOS
static const int VERSION_NUMBER_MECHOS_1   = 1;
static const int VERSION_NUMBER_MECHOS_2   = 1;
static const int VERSION_NUMBER_MECHOS_3   = 18;

#ifdef SAP_KSU
static const QString titleString = QString("САП КСУ - Стенд автоматизированной проверки контроллеров станции управления. Версия v%1.%2.%3")
                                   .arg(VERSION_NUMBER_KSU_1)
                                   .arg(VERSION_NUMBER_KSU_2)
                                   .arg(VERSION_NUMBER_KSU_3);
#endif
#ifdef SAP_SU
static const QString titleString = QString("САП СУ - Стенд автоматизированной проверки станций управления. Версия v%1.%2.%3")
                                   .arg(VERSION_NUMBER_SU_1)
                                   .arg(VERSION_NUMBER_SU_2)
                                   .arg(VERSION_NUMBER_SU_3);
#endif
#ifdef MECHOS
static const QString titleString = QString("СТСУ Механик - Стенд автоматизированной проверки станций управления. Версия v%1.%2.%3")
                                   .arg(VERSION_NUMBER_MECHOS_1)
                                   .arg(VERSION_NUMBER_MECHOS_2)
                                   .arg(VERSION_NUMBER_MECHOS_3);
#endif

#endif // STATICCONSTVALUES_H
