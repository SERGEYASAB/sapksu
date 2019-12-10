#include "LoggerWidget.h"

LoggerWidget::LoggerWidget(const QString &configFileName, const QString &configGroupName)
    : ILogger(configFileName, configGroupName)
{
    m_logWindow = new QWidget();

    m_logTextEdit = new QTextEdit(m_logWindow);
    m_logTextEdit->setReadOnly(true);

    m_filterGroup = new QButtonGroup(m_logWindow);
    m_filterGroup->setExclusive(false);

    QSize iconSize = QSize(24,24);

    const QIcon infoIcon = QIcon(":/Images/dialog-information.png");
    m_infoFilterButton = new QPushButton(m_logWindow);
    m_infoFilterButton->setFixedSize(iconSize);
    m_infoFilterButton->setIconSize(m_infoFilterButton->size());
    m_infoFilterButton->setFlat(true);
    m_infoFilterButton->setCheckable(true);
    m_infoFilterButton->setIcon(infoIcon);
    m_infoFilterButton->setToolTip("Фильтр Info");
    m_infoFilterButton->setStatusTip(tr("Вывод информационных сообщений в лог"));

    const QIcon warningIcon = QIcon(":/Images/dialog-warning.png");
    m_warningFilterButton = new QPushButton(m_logWindow);
    m_warningFilterButton->setFixedSize(iconSize);
    m_warningFilterButton->setIconSize(m_warningFilterButton->size());
    m_warningFilterButton->setFlat(true);
    m_warningFilterButton->setCheckable(true);
    m_warningFilterButton->setIcon(warningIcon);
    m_warningFilterButton->setToolTip("Фильтр Warning");
    m_warningFilterButton->setStatusTip(tr("Вывод предупредительных сообщений в лог"));

    const QIcon criticalIcon = QIcon(":/Images/error.png");
    m_criticalFilterButton = new QPushButton(m_logWindow);
    m_criticalFilterButton->setFixedSize(iconSize);
    m_criticalFilterButton->setIconSize(m_criticalFilterButton->size());
    m_criticalFilterButton->setFlat(true);
    m_criticalFilterButton->setCheckable(true);
    m_criticalFilterButton->setIcon(criticalIcon);
    m_criticalFilterButton->setToolTip("Фильтр Critical");
    m_criticalFilterButton->setStatusTip(tr("Вывод сообщений в лог об ошибках"));

    const QIcon debugIcon = QIcon(":/Images/tools-report-bug.png");
    m_debugFilterButton = new QPushButton(m_logWindow);
    m_debugFilterButton->setFixedSize(iconSize);
    m_debugFilterButton->setIconSize(m_debugFilterButton->size());
    m_debugFilterButton->setFlat(true);
    m_debugFilterButton->setCheckable(true);
    m_debugFilterButton->setIcon(debugIcon);
    m_debugFilterButton->setToolTip("Фильтр Debug");
    m_debugFilterButton->setStatusTip(tr("Вывод отладочных сообщений в лог"));

    m_filterGroup->addButton(m_infoFilterButton, LoggerTaskManager::Info);
    m_filterGroup->addButton(m_warningFilterButton, LoggerTaskManager::Warning);
    m_filterGroup->addButton(m_criticalFilterButton, LoggerTaskManager::Critical);
    m_filterGroup->addButton(m_debugFilterButton, LoggerTaskManager::Debug);

    connect(m_filterGroup, static_cast<void (QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled),
            [this](int id, bool checked)
    {
        setFilter(checked ? filter() |= LoggerTaskManager::LogLevel(id) : filter() &= ~LoggerTaskManager::LogLevel(id));
    });

    const QIcon eraserIcon = QIcon(":/Images/eraser.png");
    m_clearButton = new QPushButton(m_logWindow);
    m_clearButton->setFixedSize(iconSize);
    m_clearButton->setIconSize(m_clearButton->size());
    m_clearButton->setFlat(true);
    m_clearButton->setIcon(eraserIcon);
    m_clearButton->setStatusTip(tr("Очистить содержимое лога"));

    connect(m_clearButton, &QPushButton::clicked, [this](bool) { m_logTextEdit->clear(); });

    //TODO: прочитать про лямбды, проверить оптимальность
    QCheckBox *autoscrollingCheckBox = new QCheckBox();
    connect(autoscrollingCheckBox, &QCheckBox::stateChanged, [this, autoscrollingCheckBox] () {
        if (autoscrollingCheckBox->isChecked())
            m_logAutoScrollConnection = connect(m_logTextEdit, &QTextEdit::textChanged,
                             [this]() { m_logTextEdit->moveCursor(QTextCursor::End);});
        else
            disconnect(m_logAutoScrollConnection);
    });

    autoscrollingCheckBox->setChecked(true);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(new QLabel(tr("Фильтры : ")));
    hlayout->addWidget(m_infoFilterButton);
    hlayout->addWidget(m_warningFilterButton);
    hlayout->addWidget(m_criticalFilterButton);
    hlayout->addSpacing(50);
    hlayout->addWidget(new QLabel(tr("Фильтр отладки: ")));
    hlayout->addWidget(m_debugFilterButton);

    hlayout->addStretch();
    hlayout->addWidget(new QLabel(tr("Очистить: ")));
    hlayout->addWidget(m_clearButton);
    hlayout->addSpacing(30);
    hlayout->addWidget(new QLabel(tr("Автопрокрутка: ")));
    hlayout->addWidget(autoscrollingCheckBox);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(m_logTextEdit);

    m_logWindow->setLayout(vlayout);
}


LoggerWidget::~LoggerWidget()
{
    saveProperties();
}

QWidget *LoggerWidget::logWindow() const
{
    return m_logWindow;
}

void LoggerWidget::setLineFormat(LoggerTaskManager::LogLevel level, QTextCharFormat format)
{
    m_lineFormats[level] = format;
}

QTextCharFormat &LoggerWidget::getLineFormat(const LoggerTaskManager::LogLevel &level)
{
    return m_lineFormats[level];
}

qint32 LoggerWidget::maxLineCount() const
{
    return m_maxLineCount;
}

void LoggerWidget::setMaxLineCount(const qint32 &maxLineCount)
{
    m_maxLineCount = maxLineCount;
}

void LoggerWidget::refresh()
{
    m_logTextEdit->clear();
    QVector<LoggerTaskManager::LogLine> &buf = LoggerTaskManager::getInstance()->getBuffer();
    quint32 lineCount = 0;
    for (int i = buf.size() - 1; i >= 0 && lineCount < m_maxLineCount; i--)
    {
        if (filter() & buf[i].level)
        {
            m_logTextEdit->moveCursor(QTextCursor::Start);
            m_logTextEdit->setCurrentCharFormat(m_lineFormats[buf[i].level]);
            m_logTextEdit->insertPlainText(modify(buf[i]) + "\n");
            lineCount++;
        }
    }
    m_logTextEdit->moveCursor(QTextCursor::End);
}

void LoggerWidget::setFilter(LoggerTaskManager::LogLevels filter)
{
    ILogger::setFilter(filter);

    m_filterGroup->blockSignals(true);
    m_infoFilterButton->setChecked(filter & LoggerTaskManager::Info);
    m_warningFilterButton->setChecked(filter & LoggerTaskManager::Warning);
    m_criticalFilterButton->setChecked(filter & LoggerTaskManager::Critical);
    m_debugFilterButton->setChecked(filter & LoggerTaskManager::Debug);
    m_filterGroup->blockSignals(false);
    refresh();
}

void LoggerWidget::log(LoggerTaskManager::LogLevel level, const QString &text)
{
    if (m_logTextEdit->document()->blockCount() >= m_maxLineCount)
    {
        QTextCursor cursor(m_logTextEdit->document()->firstBlock());
        cursor.select(QTextCursor::BlockUnderCursor);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    m_logTextEdit->setCurrentCharFormat(m_lineFormats[level]);
    m_logTextEdit->append(text);
}
