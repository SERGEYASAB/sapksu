#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "ILogger.h"
#include <QDebug>

#define LOGGER_FILE_NAME QApplication::applicationDirPath() + "/Logs/" + QDate::currentDate().toString("dd.MM.yyyy") + " " + QTime::currentTime().toString("hh.mm.ss.zzz") + ".txt"

class FileLogger : public ILogger
{
    Q_OBJECT
public:
    explicit FileLogger(const QString &m_configFileName = QString(), const QString &configGroupName = QString());
    ~FileLogger();

    quint64 getMaxBufferSize();
    void setMaxBufferSize(quint64 size);

    qint64 getMaxFileSize() const;
    void setMaxFileSize(const qint64 &value);

public slots:
    void log(LoggerTaskManager::LogLevel level, const QString &text);

protected slots:
    void write();

private:
    QFile file;
    qint64 recentBufferLine;
    QString recentFileName;
    qint64 maxFileSize;
    quint64 maxBufferSize;
    quint64 currentBufferSize;

    //Все таки нужен буфер.
    //Ибо при фильтрации необходимо запоминать текущее состояние
    QStringList buffer;
};

#endif // FILELOGGER_H
