#include "WindowsEventLogger.h"

WindowsEventLogger::WindowsEventLogger(const QString &configFileName, const QString &configGroupName)
    : ILogger(configFileName, configGroupName)
{
}

void WindowsEventLogger::log(LoggerTaskManager::LogLevel level, const QString &text)
{

    // CRITICAL: вылетает если раскоментить в режиме отладки

//#ifdef Q_OS_WIN
//    LPWSTR w_text[1] = { (LPWSTR)text.toStdWString().c_str() };
//    LPWSTR info = (LPWSTR)L"SAP KSU Event log";
//    WORD type;
//    switch(level)
//    {
//        case LoggerTaskManager::Info:
//        {
//            type = EVENTLOG_INFORMATION_TYPE;
//            break;
//        }

//        case LoggerTaskManager::Warning:
//        {
//            type = EVENTLOG_WARNING_TYPE;
//            break;
//        }

//        case LoggerTaskManager::Critical:
//        {
//            type = EVENTLOG_ERROR_TYPE;
//            break;
//        }

//        case LoggerTaskManager::Debug:
//        default:
//        {
//            qDebug()<<"Не пиши отладочную инфу в журнал событий!";
//            return;
//        }
//    }
//    ReportEvent(RegisterEventSource(NULL, L"LOGGGG"), type, UI_CATEGORY, MSG_INVALID_COMMAND, NULL, 1, ((DWORD)wcslen(info) + 1) * sizeof(WCHAR), (LPCWSTR*)w_text, info);
//#else
    ILogger::log(level, text);
//#endif
}

