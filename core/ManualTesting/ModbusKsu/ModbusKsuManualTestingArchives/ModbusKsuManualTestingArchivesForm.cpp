#include "ModbusKsuManualTestingArchivesForm.h"
#include "ui_ModbusKsuManualTestingArchivesForm.h"
#include "ReportManager/OperatorClickReport.h"

ModbusKsuManualTestingArchivesForm::ModbusKsuManualTestingArchivesForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModbusKsuManualTestingArchivesForm)
{
    ui->setupUi(this);

    connect(ui->requestButton, &QPushButton::clicked, this, &ModbusKsuManualTestingArchivesForm::requestButtonClicked);
    connect(ui->readAllButton, &QPushButton::clicked, this, &ModbusKsuManualTestingArchivesForm::readAllButtonClicked);
    connect(ui->clearButton, &QPushButton::clicked, this, &ModbusKsuManualTestingArchivesForm::clearButtonClicked);
}

ModbusKsuManualTestingArchivesForm::~ModbusKsuManualTestingArchivesForm()
{
    delete ui;
}

void ModbusKsuManualTestingArchivesForm::stopTesting()
{
    if (m_testing) {
        startStopTest();
    }
}

QTableView *ModbusKsuManualTestingArchivesForm::tableView() const
{
    return ui->tableView;
}

QSpinBox *ModbusKsuManualTestingArchivesForm::recordCountSpinBox() const
{
    return ui->recordCountSpinBox;
}

QSpinBox *ModbusKsuManualTestingArchivesForm::offsetSpinBox() const
{
    return ui->offsetSpinBox;
}

void ModbusKsuManualTestingArchivesForm::startStopTest()
{
    QWidgetList widgets;
    widgets << ui->recordCountSpinBox << ui->offsetSpinBox
            << ui->readAllButton << ui->clearButton;

    m_testing = !m_testing;
    for (auto widget : widgets)
        widget->setEnabled(!m_testing);

    if (m_testing) {
        ui->requestButton->setText("Стоп");
    } else {
        ui->requestButton->setText("Опросить");
    }
}

void ModbusKsuManualTestingArchivesForm::request()
{
    int count = recordCountSpinBox()->value();
    int offset = offsetSpinBox()->value();
    readArchives(count, offset);
}

void ModbusKsuManualTestingArchivesForm::readAll()
{
    int offset = offsetSpinBox()->value();
    readArchives(-1, offset);
}

void ModbusKsuManualTestingArchivesForm::requestButtonClicked()
{
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Считать архив\""));
    request();
}

void ModbusKsuManualTestingArchivesForm::readAllButtonClicked()
{
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Считать все архивы\""));
    readAll();
}

void ModbusKsuManualTestingArchivesForm::clearButtonClicked()
{
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить\""));
    clear();
}
