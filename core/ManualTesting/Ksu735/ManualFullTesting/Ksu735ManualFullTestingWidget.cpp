#include "Ksu735ManualFullTestingWidget.h"
#include "ui_Ksu735ManualFullTestingWidget.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsVariable/Ksu735Variable.h"

#include "Ksu735ManualFullTestingModel.h"
#include "Ksu735ManualFullTestingValueDelegate.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/OperatorClickReport.h"

#include "Common/QMessageBox/ScriptDelayMessageBox/ProgressBarMessageBox.h"

#include "Authentication/SystemUser.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/IOServer.h"
#include "DBConfig/DBConfig.h"

#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"

static const int RequestsSleepTime = 100;

static const QString okColorKey         = "Ksu735ManualFullTestingWidget_OkColor";
static const QString notSameColorKey    = "Ksu735ManualFullTestingWidget_NotSameColor";
static const QString errorColorKey      = "Ksu735ManualFullTestingWidget_ErrorColor";

Ksu735ManualFullTestingWidget::Ksu735ManualFullTestingWidget(Ksu735Node *node, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Ksu735ManualFullTestingWidget),
    m_node(node)
{
    ui->setupUi(this);
    ui->loadSettingsButton->hide();
    ui->saveSettingsButton->hide();

    connect(ui->showAllCheckBox, &QCheckBox::stateChanged,
            this, &Ksu735ManualFullTestingWidget::showAllCheckBoxChanged);
    connect(ui->enableAllCheckBox, &QCheckBox::stateChanged,
            this, &Ksu735ManualFullTestingWidget::enableAllCheckBoxChanged);
    connect(ui->enableDefaultButton, &QPushButton::clicked,
            this, &Ksu735ManualFullTestingWidget::enableDefault);

    connect(ui->writeDefaultButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::writeDefault);
    connect(ui->readButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::readAll);
    connect(ui->autotestButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::autotest);
    connect(ui->clearButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::clear);

    connect(ui->writeMinimumButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::writeAllMinimums);
    connect(ui->writeMaximumButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::writeAllMaximums);
    connect(ui->writeAboveMaximumButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::writeAboveMaximum);
    connect(ui->writeLessMinimumButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::writeLessMinimum);

    connect(ui->writeButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::writeAll);
    connect(ui->clearSettingsButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::clearWriting);

    connect(ui->okColorButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::okColorButton);
    connect(ui->notSameColorButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::notSameColorButton);
    connect(ui->errorColorButton, &QPushButton::clicked, this, &Ksu735ManualFullTestingWidget::errorColorButton);

    for (auto variable : m_node->ksu0x20Variables()) {
        Ksu735ManualFullTestingParametr *parametr = new Ksu735ManualFullTestingParametr(variable);
        parametr->enabled = variable->isEnabled();
        m_parametrsList.append(parametr);
    }

    m_model = new Ksu735ManualFullTestingModel(node, m_parametrsList, this);

    auto appSettings = AppSettings::settings();
    QColor okColor = QColor(appSettings->value(okColorKey, QColor(Qt::green).name(QColor::HexRgb)).toString());
    QColor notSameColor = QColor(appSettings->value(notSameColorKey, QColor(Qt::yellow).name(QColor::HexRgb)).toString());
    QColor errorColor = QColor(appSettings->value(errorColorKey, QColor(Qt::red).name(QColor::HexRgb)).toString());

    m_model->setOkColor(okColor);
    ui->okColorButton->setStyleSheet("background-color: " + okColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
    m_model->setNotSameColor(notSameColor);
    ui->notSameColorButton->setStyleSheet("background-color: " + notSameColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
    m_model->setErrorColor(errorColor);
    ui->errorColorButton->setStyleSheet("background-color: " + errorColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    ui->searchFormLayout->addWidget(searchWidget);

    ui->tableView->setModel(searchWidget->getProxyModel());
    ui->tableView->setSelectionBehavior(QTableView::SelectRows);
    ui->tableView->setSelectionMode(QTableView::SelectionMode::SingleSelection);
    //    ui->tableView->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    //    ui->tableView->setItemDelegateForColumn(1, new Ksu735Manual0x20TestingVariableDelegate(node, m_model, this));

    QMap<int, QString> delegateMap;
    delegateMap[0] = "XXXX";
    delegateMap[1] = "XXX.X";
    delegateMap[2] = "XX.XX";
    delegateMap[3] = "X.XXX";
    delegateMap[4] = ".XXXX";
    delegateMap[5] = "XX/XX";
    delegateMap[6] = "BIN";
    delegateMap[7] = "CCCC";
    FKComboBoxDelegate *formatDelegate = new FKComboBoxDelegate(delegateMap, this);
    ui->tableView->setItemDelegateForColumn(3, formatDelegate);

    QMap<int, QString> keyValueMap;
    keyValueMap[0] = "R";
    keyValueMap[1] = "R/W";
    FKComboBoxDelegate *writeAccessDelegate = new FKComboBoxDelegate(keyValueMap, this);
    ui->tableView->setItemDelegateForColumn(4, writeAccessDelegate);

    Ksu735ManualFullTestingValueDelegate *valueDelegate = new Ksu735ManualFullTestingValueDelegate(m_model, this);
    ui->tableView->setItemDelegateForColumn(9, valueDelegate);
    ui->tableView->setItemDelegateForColumn(10, valueDelegate);
    ui->tableView->setItemDelegateForColumn(11, valueDelegate);
    ui->tableView->setItemDelegateForColumn(12, valueDelegate);

    ui->tableView->setItemDelegateForColumn(5, valueDelegate);
    ui->tableView->setItemDelegateForColumn(6, valueDelegate);
    ui->tableView->setItemDelegateForColumn(7, valueDelegate);

    QHeaderView *verticalHeader = ui->tableView->verticalHeader();
    verticalHeader->setDefaultSectionSize(22);
    verticalHeader->hide();

    QHeaderView *horizontalHeader = ui->tableView->horizontalHeader();
    horizontalHeader->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader->setSectionResizeMode(1, QHeaderView::Fixed);
    horizontalHeader->setStretchLastSection(true);

    horizontalHeader->resizeSections(QHeaderView::ResizeToContents);
    horizontalHeader->resizeSection(1, 70);
    horizontalHeader->resizeSection(2, 500);
    horizontalHeader->resizeSection(5, 50);
    horizontalHeader->resizeSection(6, 50);
    horizontalHeader->resizeSection(10, 50);
    horizontalHeader->resizeSection(11, 50);

    ui->progressBar->hide();
    ui->progressBar->setContentsMargins(QMargins());

    ui->progressLine->resize(24, 0);
    ui->progressLine->setContentsMargins(QMargins());
    ui->progressLine->setMinimumHeight(24);

    m_progressBarMessageBox = new ProgressBarMessageBox(this);
}

Ksu735ManualFullTestingWidget::~Ksu735ManualFullTestingWidget()
{
    delete ui;

    for (auto parametr : m_parametrsList) {
        delete parametr;
    }
    m_parametrsList.clear();
}

void Ksu735ManualFullTestingWidget::stopTesting()
{
    if (m_testing)
        startStopTest();
}

void Ksu735ManualFullTestingWidget::showAllCheckBoxChanged(int state)
{
    if (state == Qt::CheckState::Unchecked) {
        m_model->setShowEnabledOnly(true);
    } else {
        m_model->setShowEnabledOnly(false);
    }
}

void Ksu735ManualFullTestingWidget::enableAllCheckBoxChanged(int state)
{
    for (auto parametr : m_parametrsList) {
        parametr->enabled = state == Qt::CheckState::Checked;
    }

    m_model->updateEnabledParametrsList();
}

void Ksu735ManualFullTestingWidget::enableDefault()
{
    for (auto parametr : m_parametrsList) {
        parametr->enabled = parametr->variable()->isEnabled();
    }

    const QSignalBlocker blocker(ui->enableAllCheckBox);
    ui->enableAllCheckBox->setChecked(false);

    m_model->updateEnabledParametrsList();
}

void Ksu735ManualFullTestingWidget::writeDefault()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->defaultValue());
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать значения по-умолчанию (протокол 7.35)\""));
}

void Ksu735ManualFullTestingWidget::readAll()
{
    startStopTest();
    if (!m_testing)
        return;

    //Debug
    //for (int i = 0; i < m_parametrsList.count(); i++) {
    //if (i > 3)
    //m_parametrsList.at(i)->enabled = false;
    //}

    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs();

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        readOneParametr(parametr);
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
        setProgressBarProgress(double(ui->progressBar->value()) / double(ui->progressBar->maximum()) * 100);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Считать все параметры (протокол 7.35)\""));
}

bool Ksu735ManualFullTestingWidget::autotest()
{
    bool autotestresult = true;

    startStopTest();
    if (!m_testing)
        return false;

    if (autotestingmodeFlag)
        m_progressBarMessageBox->show();


    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        if (!autotestParametr(parametr)) {
            autotestresult = false;
        }

        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
        setProgressBarProgress(double(ui->progressBar->value()) / double(ui->progressBar->maximum()) * 100);
    }

    if (m_testing) {
        startStopTest();
    }

    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Автотест (протокол 7.35)\""));

    return autotestresult;
}

void Ksu735ManualFullTestingWidget::clear()
{
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить (протокол 7.35)\""));
    clearAll();
}

void Ksu735ManualFullTestingWidget::writeAllMinimums()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->minValue());
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все минимумы (протокол 7.35)\""));
}

void Ksu735ManualFullTestingWidget::writeAllMaximums()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->maxValue());
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все максимумы (протокол 7.35)\""));
}

void Ksu735ManualFullTestingWidget::writeAboveMaximum()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->maxValue().toFloat() + 1.0);
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все максимумы (протокол 7.35)\""));

}

void Ksu735ManualFullTestingWidget::writeLessMinimum()
{
    startStopTest();
    if (!m_testing)
        return;

    bool ignoreReadOnly = ui->ignoreReadOnlyCheckBox->isChecked();
    Ksu735ManualFullTestingParametrsList enabledParametrsList = enabledParametrs(ignoreReadOnly);

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(enabledParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : enabledParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->variable()->minValue().toFloat() - 1.0);
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все минимумы (протокол 7.35)\""));

}

void Ksu735ManualFullTestingWidget::writeAll()
{
    startStopTest();
    if (!m_testing)
        return;

    Ksu735ManualFullTestingParametrsList writedParametrsList;
    for (auto parametr : m_parametrsList) {
        if (parametr->enabled && parametr->writingValue.isValid() && !parametr->writingValue.isNull()) {
            writedParametrsList.append(parametr);
        }
    }

    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(writedParametrsList.size());
    ui->progressBar->setValue(0);

    int parametrsCount = 0;
    for (auto parametr : writedParametrsList) {
        if (!m_testing) {
            break;
        }

        writeOneParametr(parametr, parametr->writingValue);
        m_model->updateParametr(parametr);

        processEventsAndSleep(RequestsSleepTime);
        //        QApplication::processEvents(QEventLoop::AllEvents, RequestsSleepTime);

        parametrsCount++;
        ui->progressBar->setValue(parametrsCount);
    }

    if (m_testing) {
        startStopTest();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Записать все параметры (протокол 7.35)\""));
}

void Ksu735ManualFullTestingWidget::clearWriting()
{
    for (auto parametr : m_parametrsList) {
        parametr->clearWritingValue();
    }
    OperatorClickReport::instance()->write(QStringLiteral("Ручное тестирование: нажата кнопка \"Очистить записываемые параметры (протокол 7.35)\""));
    m_model->updateColumn(8);
}

void Ksu735ManualFullTestingWidget::okColorButton()
{
    QColorDialog colorDialog(m_model->okColor());
    colorDialog.exec();
    if (colorDialog.result() == QColorDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        ui->okColorButton->setStyleSheet("background-color: " + selectedColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
        m_model->setOkColor(selectedColor);
        AppSettings::settings()->setValue(okColorKey, selectedColor.name(QColor::HexRgb));
    }
}

void Ksu735ManualFullTestingWidget::notSameColorButton()
{
    QColorDialog colorDialog(m_model->notSameColor());
    colorDialog.exec();
    if (colorDialog.result() == QColorDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        ui->notSameColorButton->setStyleSheet("background-color: " + selectedColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
        m_model->setNotSameColor(selectedColor);
        AppSettings::settings()->setValue(notSameColorKey, selectedColor.name(QColor::HexRgb));
    }
}

void Ksu735ManualFullTestingWidget::errorColorButton()
{
    QColorDialog colorDialog(m_model->errorColor());
    colorDialog.exec();
    if (colorDialog.result() == QColorDialog::Accepted) {
        QColor selectedColor = colorDialog.selectedColor();
        ui->errorColorButton->setStyleSheet("background-color: " + selectedColor.name(QColor::HexRgb) + ";\nborder: 2px solid black;");
        m_model->setErrorColor(selectedColor);
        AppSettings::settings()->setValue(errorColorKey, selectedColor.name(QColor::HexRgb));
    }
}

void Ksu735ManualFullTestingWidget::startStopTest()
{
    QWidgetList widgets;
    widgets << ui->showAllCheckBox << ui->enableAllCheckBox
            << ui->enableDefaultButton << ui->writeDefaultButton
            << ui->autotestButton << ui->clearButton

            << ui->writeMinimumButton << ui->writeMaximumButton
            << ui->writeAboveMaximumButton

            << ui->writeButton << ui->clearSettingsButton;

    m_testing = !m_testing;
    for (auto widget : widgets)
        widget->setEnabled(!m_testing);

    ui->progressBar->setVisible(m_testing);

    if (m_testing) {
        clearAll();
        ui->readButton->setText("Стоп");
    } else {
        ui->readButton->setText("Чтение");
    }
}

void Ksu735ManualFullTestingWidget::readOneParametr(Ksu735ManualFullTestingParametr *parametr)
{
    bool use = parametr->variable()->useMinMax();
    parametr->variable()->setUseMinMax(false);

    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->readedValue = parametr->variable()->descriptionValue();
        parametr->status = SapKsu::FullTestingStatus::Ok;
        parametr->variable()->setUseMinMax(use);
    } else {
        parametr->readedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        parametr->variable()->setUseMinMax(use);
    }
}

bool Ksu735ManualFullTestingWidget::writeOneParametr(Ksu735ManualFullTestingParametr *parametr, const QVariant &value)
{
    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->oldReadedValue = parametr->variable()->value();
    } else {
        parametr->oldReadedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    QVariant writingValue = value;
    if (!writingValue.isValid() || writingValue.isNull()) {
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = QStringLiteral("Неверное значение записываемого параметра");
    }

    if (m_node->writeVariable(parametr->variable(), writingValue, INode::OperationTimingType::Sync)) {
        parametr->writedValue = writingValue;
    } else {
        parametr->writedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->readedValue = parametr->variable()->value();
    } else {
        parametr->readedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    parametr->statusString.clear();
    if (compareReadedAndWritedValue(parametr->readedValue, parametr->writedValue, parametr->variable()->format())) {
        parametr->status = SapKsu::FullTestingStatus::Ok;
    } else {
        parametr->status = SapKsu::FullTestingStatus::NotSame;
    }

    return true;
}

bool Ksu735ManualFullTestingWidget::autotestParametr(Ksu735ManualFullTestingParametr *parametr)
{
    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->oldReadedValue = parametr->variable()->value();
    } else {
        parametr->oldReadedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    QVariant writingValue = incrementValue(parametr->oldReadedValue, parametr->variable());

    if (!writingValue.isValid() || writingValue.isNull()) {
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = QStringLiteral("Неверное значение записываемого параметра");
    }

    if (m_node->writeVariable(parametr->variable(), writingValue, INode::OperationTimingType::Sync)) {
        parametr->writedValue = writingValue;
    } else {
        parametr->writedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
        parametr->readedValue = parametr->variable()->value();
    } else {
        parametr->readedValue = QVariant();
        parametr->status = SapKsu::FullTestingStatus::Error;
        parametr->statusString = m_node->lastError().description();
        return false;
    }

    parametr->statusString.clear();

    if (compareReadedAndWritedValue(parametr->readedValue, parametr->writedValue, parametr->variable()->format())) {
        parametr->status = SapKsu::FullTestingStatus::Ok;

        if (!m_node->writeVariable(parametr->variable(), parametr->oldReadedValue, INode::OperationTimingType::Sync)) {
            parametr->status = SapKsu::FullTestingStatus::Error;
            parametr->statusString = QStringLiteral("Не удалось записать исходное значение");
            return false;
        }

        if (m_node->readVariable(parametr->variable(), INode::OperationTimingType::Sync)) {
            QVariant readedValue = parametr->variable()->value();

            if (!compareReadedAndWritedValue(readedValue, parametr->oldReadedValue, parametr->variable()->format())) {
                parametr->status = SapKsu::FullTestingStatus::Error;
                parametr->statusString = QStringLiteral("Не удалось записать исходное значение");
                return false;
            }

        } else {
            parametr->status = SapKsu::FullTestingStatus::Error;
            parametr->statusString = QStringLiteral("Не удалось записать исходное значение");
            return false;
        }


    } else {
        parametr->status = SapKsu::FullTestingStatus::NotSame;
    }

    return true;
}

QVariant Ksu735ManualFullTestingWidget::incrementValue(const QVariant &value, Ksu735Variable *variable, bool ignoreMaximum)
{
    SapKsu::Ksu735ParametrFormat format = variable->format();

    if (format == SapKsu::Ksu735ParametrFormat::XXXX || format >= SapKsu::Ksu735ParametrFormat::XX1XX) {
        quint32 uintValue = value.toUInt();
        uintValue++;
        if (!ignoreMaximum && uintValue > variable->maxValue().toUInt()) {
            return variable->minValue();
        }
        return QVariant::fromValue(uintValue);
    } else {
        double doubleValue = value.toDouble();
        doubleValue += 1.0 / SapKsu::paramertValueDivisorForFormat(format);
        if (!ignoreMaximum && doubleValue > variable->maxValue().toDouble()) {
            return variable->minValue();
        }
        return QVariant::fromValue(doubleValue);
    }
}

void Ksu735ManualFullTestingWidget::clearAll()
{
    for (auto parametr : m_parametrsList) {
        parametr->clear();
    }
    m_model->updateAll();
}

Ksu735ManualFullTestingParametrsList Ksu735ManualFullTestingWidget::enabledParametrs(bool writeOnly) const
{
    Ksu735ManualFullTestingParametrsList enabledParametrsList;
    for (auto parametr : m_parametrsList) {
        if (parametr->enabled) {
            if (!writeOnly || (writeOnly && parametr->variable()->isWritable()))
                enabledParametrsList.append(parametr);
        }
    }
    return enabledParametrsList;
}

//TODO: возможно следует это убрать нафиг
void Ksu735ManualFullTestingWidget::processEventsAndSleep(int sleepTime) const
{
    //    auto start = std::chrono::high_resolution_clock::now();
    QApplication::processEvents(QEventLoop::AllEvents, sleepTime);
    //    auto end = std::chrono::high_resolution_clock::now();
    //    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    //    auto msec = sleepTime - elapsed;
    //    if (msec > 0) {
    //        QThread::msleep(msec);
    //    }
}

bool Ksu735ManualFullTestingWidget::compareReadedAndWritedValue(const QVariant &rval,
                                                                const QVariant &lval,
                                                                SapKsu::Ksu735ParametrFormat format)
{
    if (rval == lval) {
        return true;
    } else {
        QMetaType::Type type = static_cast<QMetaType::Type>(rval.type());
        if ((type == QMetaType::Double || type == QMetaType::Float) && lval.canConvert(type)) {
            double rDoubleValue = rval.value<double>();
            double lDoubleValue = lval.value<double>();
            double epsilon = 1.0 / SapKsu::paramertValueDivisorForFormat(format);
            if (fabs(lDoubleValue - rDoubleValue) < epsilon)
                return true;
        }
    }

    return false;
}

ProgressBarMessageBox *Ksu735ManualFullTestingWidget::getProgressBarMessageBox() const
{
    return m_progressBarMessageBox;
}

int Ksu735ManualFullTestingWidget::getProgressBarProgress() const
{
    return progressBarProgress;
}

void Ksu735ManualFullTestingWidget::setProgressBarProgress(int value)
{
    if (progressBarProgress == value)
        return;

    progressBarProgress = value;

    emit progressBarChanged(value);
}

bool Ksu735ManualFullTestingWidget::getAutotestingmodeFlag() const
{
    return autotestingmodeFlag;
}

void Ksu735ManualFullTestingWidget::setAutotestingmodeFlag(bool value)
{
    autotestingmodeFlag = value;
}
