#include "KsuLogger.h"

KsuLogger::KsuLogger(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    setLayout(lay);

    m_logTable = new QTableView(this);
    lay->addWidget(m_logTable);


}

void KsuLogger::log(QTime time, LoggerTaskManager2::MessageType type, const QString &text)
{
//m_logTable->append(text);
}
