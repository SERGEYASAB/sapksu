#include "Ksu735Manual0x30TestingWidget.h"
#include "ui_Ksu735Manual0x30TestingWidget.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "Ksu735Manual0x30TestingModel.h"
#include "ReportManager/OperatorClickReport.h"

Ksu735Manual0x30TestingWidget::Ksu735Manual0x30TestingWidget(Ksu735Node *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ksu735Manual0x30TestingWidget),
    m_node(node)
{
    ui->setupUi(this);

    ui->recordCountSpinBox->setMinimum(1);
    ui->recordCountSpinBox->setMaximum(6);
    ui->recordCountSpinBox->setValue(6);

    ui->offsetSpinBox->setMinimum(1);
    ui->offsetSpinBox->setMaximum(200);
    ui->offsetSpinBox->setValue(1);

    connect(ui->requestButton, &QPushButton::clicked, this, &Ksu735Manual0x30TestingWidget::request0x30);

    m_model = new Ksu735Manual0x30TestingModel(m_recordsModel, this);

    ui->tableView->setModel(m_model);
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SelectionMode::SingleSelection);

    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    verticalHeader->setDefaultSectionSize(22);
    verticalHeader->hide();

    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setStretchLastSection(true);

    horizontalHeader->resizeSections(QHeaderView::ResizeToContents);
    horizontalHeader->resizeSection(1, 100);
    horizontalHeader->resizeSection(2, 500);
    horizontalHeader->resizeSection(3, 400);
}

Ksu735Manual0x30TestingWidget::~Ksu735Manual0x30TestingWidget()
{
    delete ui;
}

void Ksu735Manual0x30TestingWidget::request0x30()
{
    quint8 count = static_cast<quint8>(ui->recordCountSpinBox->value());
    quint8 offset = static_cast<quint8>(ui->offsetSpinBox->value());

    bool ok;
    auto recordsList = m_node->readArchive(count, offset, &ok);

    m_recordsModel.clean();

    QList<Ksu735Manual0x30TestingArchiveRecord> modelRecords;
    for (auto record : recordsList) {
        Ksu735Manual0x30TestingArchiveRecord modelRecord;
        modelRecord.number = offset;
        modelRecord.archiveRecord = record;
        modelRecords.append(modelRecord);

        offset++;
    }
    m_recordsModel.setRecords(modelRecords);

    if (!ok) {
        m_recordsModel.setLastError(m_node->lastError());
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Опросить по функции 0х30\""));
    m_model->updateValues();
}
