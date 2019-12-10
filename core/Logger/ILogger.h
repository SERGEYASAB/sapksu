#ifndef ILOGGER_H
#define ILOGGER_H

#include <QObject>
#include <QTime>
#include <QSettings>
#include "LoggerTaskManager.h"

Q_DECLARE_METATYPE(QTextCharFormat)

// TODO: сделать загрузку и сохранение свойств, если потребуется

class ILogger : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LoggerTaskManager::LogLevels filter READ filter WRITE setFilter)

public:
    explicit ILogger(const QString &configFileName = QString(), const QString &configGroupName = QString());
    ~ILogger();

public slots:
    //Сюда передается уже отформатированная строка
    //Проверка фильтров производится в интерфейсе.
    //Здесь описываем лишь непосредственно вывод и сопуствующие ему условия (размер буфера и т.д.)
    //Можно использовать для вывода лога только в этот экземпляр.
    //Тогда передаваемая строка никак не форматируется.
    virtual void log(LoggerTaskManager::LogLevel level, const QString &text);

    LoggerTaskManager::LogLevels filter();
    virtual void setFilter(LoggerTaskManager::LogLevels filter);

private:
    QString m_configFileName;
    QString m_configGroupName;

    LoggerTaskManager::LogLevels m_filter;

protected slots:
    //Всегда вызывается (в интерфейсе) перед log(..)
    //Добавляет штамп времени\префиксы\постфиксы\раскраску и т.д.
    //Если не переопределен, не добавляет ничего
    //Вынесено в отдельный слот т.к. этот метод необходимо вызывать еще и
    //при фильтрации (раз уж решили делать общий буфер)
    virtual QString modify(LoggerTaskManager::LogLine &line);

    //Эти 2 не доделал. Пока решили не сохранять и не считывать конфиги.
    void initProperties();
    void saveProperties();
};

#endif // ILOGGER_H
