#include "ILogger.h"

ILogger::ILogger(const QString &configFileName, const QString &configGroupName)
{
    m_configFileName = configFileName;
    m_configGroupName = configGroupName;

    connect(LoggerTaskManager::getInstance(), &LoggerTaskManager::timeToLog, [this](LoggerTaskManager::LogLine &line)
    {
        if(m_filter & line.level)
            emit log(line.level, modify(line));
    });

    //Создаем папку если таковой нет
    QDir logDir = QApplication::applicationDirPath() + "/Logs";
    if(!logDir.exists())
    {
        logDir.cdUp();
        logDir.mkdir("Logs");
    }
}

ILogger::~ILogger()
{
}

QString ILogger::modify(LoggerTaskManager::LogLine &line)
{
    QString str = line.timeStamp.toString("hh:mm:ss.zzz ");

    switch(line.level)
    {
        case LoggerTaskManager::Info:
        {
            str.append("ИНФОРМАЦИЯ: ");
            break;
        }

        case LoggerTaskManager::Warning:
        {
            str.append("ПРЕДУПРЕЖДЕНИЕ: ");
            break;
        }

        case LoggerTaskManager::Critical:
        {
            str.append("ОШИБКА: ");
            break;
        }

        case LoggerTaskManager::Debug:
        default:
        {
            str.append("ОТЛАДКА: ");
            break;
        }
    }
    str += line.text;
    return str;
}

void ILogger::log(LoggerTaskManager::LogLevel level, const QString &text)
{
    qDebug()<<this->metaObject()->className()<<level<<" - Не переопределена функция вывода! "<<text;
}

LoggerTaskManager::LogLevels ILogger::filter()
{
    return m_filter;
}

void ILogger::setFilter(LoggerTaskManager::LogLevels filter)
{
    m_filter = filter;
}

void ILogger::initProperties()
{
    if(!m_configFileName.isEmpty() && !m_configGroupName.isEmpty())
    {
        QSettings config(QApplication::applicationDirPath() + "/" + m_configFileName, QSettings::IniFormat);
        config.beginGroup(m_configGroupName);
        for(auto name : config.allKeys())
        {
            QVariant value = config.value(name);
            if(value.canConvert(QVariant::Int))
            {
                setProperty(name.toLatin1(), value.toInt());
            }
            else if(value.canConvert(QVariant::TextFormat))
                setProperty(name.toLatin1(), qvariant_cast<QTextCharFormat>(value));
        }
        config.endGroup();
    }
    else
        setFilter(LoggerTaskManager::Empty);
}

void ILogger::saveProperties()
{
    if(!m_configFileName.isEmpty() && !m_configGroupName.isEmpty())
    {
        QSettings config(QApplication::applicationDirPath() + "/" + m_configFileName, QSettings::IniFormat);
        config.beginGroup(m_configGroupName);
        for(int i = 1; i < metaObject()->propertyCount(); i++)
        {
            QVariant value = property(metaObject()->property(i).name());
            if(value.canConvert(QVariant::Int))
                config.setValue(metaObject()->property(i).name(), value.toInt());
            else if(value.canConvert(QVariant::TextFormat))
                config.setValue(metaObject()->property(i).name(), qvariant_cast<QTextCharFormat>(value));
        }
        config.endGroup();
    }
}

