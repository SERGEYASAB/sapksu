#ifndef KSULOGWIDGET_H
#define KSULOGWIDGET_H

#include "Common/PrefixHeader.h"
#include "Logger/Logger2/LoggerTaskManager2.h"

class KsuLogWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KsuLogWidget(int maxLines = 1000, QWidget *parent = nullptr);
    ~KsuLogWidget();

protected slots:
    void log(QTime time, LoggerTaskManager2::MessageType type, const QString& text);

private:
    QVBoxLayout *m_mainLay = nullptr;
    QTextEdit *m_logEdit = nullptr;
    QSpinBox *m_logDeep = nullptr;
    QPushButton *m_acceptPushButton = nullptr;

    int m_maxLineCount;
    QHash<LoggerTaskManager2::MessageType, QTextCharFormat> m_lineFormats;
};

#endif // KSULOGWIDGET_H
