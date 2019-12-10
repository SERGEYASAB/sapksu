#include "KsuLogWidget.h"

KsuLogWidget::KsuLogWidget(int maxLines, QWidget *parent) : QWidget(parent), m_maxLineCount(maxLines)
{
    m_logDeep = new QSpinBox(this);
    m_logDeep->setValue(maxLines);
    m_logDeep->setMaximum(1000);
    m_logDeep->setMinimum(1);

    m_logEdit = new QTextEdit(this);
    m_logEdit->setReadOnly(true);

    QPushButton *m_acceptPushButton = new QPushButton(this);
    m_acceptPushButton->setText("Применить");
    connect(m_acceptPushButton, &QPushButton::clicked, [this]()
    {
        m_logEdit->document()->setMaximumBlockCount(m_logDeep->value());
        m_maxLineCount = m_logDeep->value();
    });

    QPushButton *clearButton = new QPushButton(this);
    clearButton->setText("Очистить");
    connect(clearButton, &QPushButton::clicked, [this]() { m_logEdit->clear(); });

    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addStretch();
    hBoxLayout->addWidget(new QLabel(tr("Глубина лога: ")));
    hBoxLayout->addWidget(m_logDeep);
    hBoxLayout->addSpacing(10);
    hBoxLayout->addWidget(m_acceptPushButton);
    hBoxLayout->addSpacing(50);
    hBoxLayout->addWidget(clearButton);


    m_mainLay = new QVBoxLayout;
    m_mainLay->addLayout(hBoxLayout);
    m_mainLay->addWidget(m_logEdit);

    setLayout(m_mainLay);

    QTextCharFormat infoFormat;
    infoFormat.setFont(QFont("Times New Roman", 14));
    infoFormat.setForeground(QBrush(Qt::darkMagenta));
    m_lineFormats[LoggerTaskManager2::Info] = infoFormat;

    QTextCharFormat txFormat;
    txFormat.setFont(QFont("Times New Roman", 14));
    txFormat.setForeground(QBrush(Qt::darkBlue));
    m_lineFormats[LoggerTaskManager2::Tx] = txFormat;

    QTextCharFormat rxFormat;
    rxFormat.setFont(QFont("Times New Roman", 14));
    rxFormat.setForeground(QBrush(Qt::darkGreen));
    m_lineFormats[LoggerTaskManager2::Rx] = rxFormat;

    QTextCharFormat errorFormat;
    errorFormat.setFont(QFont("Times New Roman", 14));
    errorFormat.setForeground(QBrush(Qt::red));
    m_lineFormats[LoggerTaskManager2::Error] = errorFormat;

    connect(LoggerTaskManager2::getInstance(), &LoggerTaskManager2::timeToLog, this, &KsuLogWidget::log);
}

KsuLogWidget::~KsuLogWidget()
{

}

void KsuLogWidget::log(QTime time, LoggerTaskManager2::MessageType type, const QString &text)
{
        if (m_logEdit->document()->lineCount() >= m_maxLineCount)
        {
            QTextCursor cursor(m_logEdit->document()->firstBlock());
            cursor.select(QTextCursor::LineUnderCursor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
        }

    QString prefix;
    switch (type) {
        case LoggerTaskManager2::Info:
            break;

        case LoggerTaskManager2::Rx:
            prefix = " Rx: ";
            break;

        case LoggerTaskManager2::Tx:
            prefix = " Tx: ";
            break;

        case LoggerTaskManager2::Error:
            prefix = " Error: ";
            break;

        default:
            break;
    }

    m_logEdit->setCurrentCharFormat(m_lineFormats[type]);
    m_logEdit->append(time.toString("hh.mm.ss.zzz") + prefix + text);
}
