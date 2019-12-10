#ifndef LoggerWidget_H
#define LoggerWidget_H

#include <QTime>
#include <QApplication>
#include "ILogger.h"
#include <QtWidgets>

class LoggerWidget : public ILogger
{
    Q_OBJECT
    Q_PROPERTY(qint32 maxLineCount READ maxLineCount WRITE setMaxLineCount)

public:
    LoggerWidget(const QString &m_configFileName = QString(), const QString &configGroupName = QString());
    ~LoggerWidget();

    QWidget *logWindow() const;

    qint32 maxLineCount() const;
    void setMaxLineCount(const qint32 &maxLineCount);

public slots:
    void log(LoggerTaskManager::LogLevel level, const QString &text) override;
    void setFilter(LoggerTaskManager::LogLevels filter) override;

    void setLineFormat(LoggerTaskManager::LogLevel level, QTextCharFormat format);
    QTextCharFormat &getLineFormat(const LoggerTaskManager::LogLevel &level);

private slots:
    void refresh();

private:
    QWidget *m_logWindow;

    QPushButton *m_clearButton;
    QPushButton *m_infoFilterButton;
    QPushButton *m_warningFilterButton;
    QPushButton *m_criticalFilterButton;
    QPushButton *m_debugFilterButton;
    QButtonGroup *m_filterGroup;

    QTextEdit *m_logTextEdit;

    QMetaObject::Connection m_logAutoScrollConnection;
    QHash<LoggerTaskManager::LogLevel, QTextCharFormat> m_lineFormats;

    qint32 m_maxLineCount;
};

#endif // LoggerWidget_H
