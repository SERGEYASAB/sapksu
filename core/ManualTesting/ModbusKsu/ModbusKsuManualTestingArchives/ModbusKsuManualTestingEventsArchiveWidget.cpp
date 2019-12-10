#include "ModbusKsuManualTestingEventsArchiveWidget.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuEventsArchive.h"

#include "ModbusKsuManualTestingEventsArchiveModel.h"

ModbusKsuManualTestingEventsArchiveWidget::ModbusKsuManualTestingEventsArchiveWidget(ModbusKsuNode *node, QWidget *parent) :
    ModbusKsuManualTestingArchivesForm(parent),
    m_node(node)
{
    m_model = new ModbusKsuManualTestingEventsArchiveModel(recordsModel(), this);

    tableView()->setModel(m_model);
    tableView()->setSelectionBehavior(QTableView::SelectRows);
    tableView()->setSelectionMode(QTableView::SelectionMode::SingleSelection);

    QHeaderView *verticalHeader = tableView()->verticalHeader();
    verticalHeader->setDefaultSectionSize(22);

    QHeaderView *horizontalHeader = tableView()->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);
}

void ModbusKsuManualTestingEventsArchiveWidget::clear()
{
    recordsModel().clean();
    m_model->updateValues();
}

void ModbusKsuManualTestingEventsArchiveWidget::readArchives(int count, int offset)
{
    startStopTest();
    if (!testing())
        return;

    clear();

    IModbusKsuArchive::ModbusKsuArchiveType archiveType = IModbusKsuArchive::ModbusKsuArchiveType::EventsArchive;

    if (!m_node->moveArchiveRecordToZeroPosition(archiveType)) {
        recordsModel().setLastError(m_node->lastError());
        m_model->updateValues();
        startStopTest();
        return;
    }
    if (offset != 0) {
        if (!m_node->moveArchiveRecordToNPosition(offset, archiveType)) {
            recordsModel().setLastError(m_node->lastError());
            m_model->updateValues();
            startStopTest();
            return;
        }
    }

    bool ok;
    bool lastArchiveWasEmpty = false;
    for (int i = 0; i < count || count == -1; i++) {
        if (!testing()) {
            break;
        }
        QApplication::processEvents(QEventLoop::AllEvents, 100);

        IModbusKsuArchive *archiveRecord = m_node->readArchive(archiveType, &ok);
        ModbusKsuEventsArchive *archive = qobject_cast<ModbusKsuEventsArchive *>(archiveRecord);

        QApplication::processEvents(QEventLoop::AllEvents, 100);

        if (!ok) {
            recordsModel().setLastError(m_node->lastError());
            m_model->updateValues();
            break;
        }

        if (archiveRecord->isEmpty() && lastArchiveWasEmpty)
            break;

        lastArchiveWasEmpty = archiveRecord->isEmpty();


        ModbusKsuManualTestingArchiveRecord record;
        record.number = offset + i;
        record.archiveRecord = archive;

        recordsModel().appendRecord(record);

        QApplication::processEvents(QEventLoop::AllEvents, 100);
        m_model->updateValues();

        if (!m_node->moveArchiveRecordToNextPosition(archiveType)) {
            recordsModel().setLastError(m_node->lastError());
            m_model->updateValues();
            break;
        }
    }

    if (testing()) {
        startStopTest();
    }
}
