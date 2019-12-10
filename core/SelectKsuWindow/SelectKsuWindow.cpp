#include "SelectKsuWindow.h"
#include "DBConfig/DBConfig.h"

#include "KSUPortConnection/KSUPortConnectionWidget.h"
#include "StartWindow/StartWindowTesting.h"
#include "IOServer/IOServer.h"
#include "ReportManager/OperatorClickReport.h"
#include "SelectKsuWindow/KsuInfoWindow.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualTestingParametr.h"
#include "ErrorClassifier/ErrorClassifier.h"

SelectKsuWindow::SelectKsuWindow(QWidget *parent) : QDialog(parent)
{
    m_ksuTypeLabel = new QLabel(tr("Название КСУ"));
    m_ksuTypeComboBox = new QComboBox();

    m_ksuSoftwareVersionLabel = new QLabel(tr("Версия ПО КСУ"));
    m_ksuSoftwareVersionComboBox = new QComboBox();

    m_ksuTypeModel = new QSqlQueryModel(this);
    m_ksuSoftwareVersionModel = new QSqlQueryModel(this);

    m_ksuAddressLabel = new QLabel(tr("Адрес КСУ"));
    m_ksuAddressSpinBox = new QSpinBox();

    m_ksuAddressSpinBox->setMinimum(0);
    m_ksuAddressSpinBox->setMaximum(255);
    m_ksuAddressSpinBox->setValue(1);

    m_startSearchAddressLabel = new QLabel(tr("Начальный адрес поиска"));
    m_startSearchAddressSpinBox = new QSpinBox();

    m_startSearchAddressSpinBox->setMinimum(0);
    m_startSearchAddressSpinBox->setMaximum(255);

    m_timeoutLabel = new QLabel(tr("Интервал между запросами мсек."));
    m_timeoutSpinbox = new QSpinBox();

    m_timeoutSpinbox->setMinimum(0);
    m_timeoutSpinbox->setMaximum(100);

    selectKsuTypes();
    selectKsuSoftwareVersion(-1);

    m_ksuTypeComboBox->setModel(m_ksuTypeModel);
    m_ksuTypeComboBox->setModelColumn(2);

    m_ksuSoftwareVersionComboBox->setModel(m_ksuSoftwareVersionModel);
    m_ksuSoftwareVersionComboBox->setModelColumn(1);



    connect(m_ksuTypeComboBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &SelectKsuWindow::currenKsuTypeChanged);
    connect(m_ksuSoftwareVersionComboBox, static_cast<void(QComboBox::*)(int index)>(&QComboBox::currentIndexChanged), this, &SelectKsuWindow::currentSoftwareVersionChanged);

    m_manufacturerLabel = new QLabel(tr("Завод изготовитель"));
    m_manufacturerLineEdit = new QLineEdit("");
    m_manufacturerLineEdit->setDisabled(true);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_ksuTypeLabel, m_ksuTypeComboBox);
    formLayout->addRow(m_manufacturerLabel, m_manufacturerLineEdit);
    formLayout->addRow(m_ksuSoftwareVersionLabel, m_ksuSoftwareVersionComboBox);
    // TODO: search hiden
    QGroupBox *searchSettingsgroupBox = new QGroupBox(tr("Настройка адреса" /* Поиск КСУ по дресу*/));
    QVBoxLayout *searchVBoxLayout = new QVBoxLayout;

    QFormLayout *addressFormLayout = new QFormLayout;
    addressFormLayout->addRow(m_ksuAddressLabel, m_ksuAddressSpinBox);
    // TODO: search hiden
    //    addressFormLayout->addRow(m_startSearchAddressLabel, m_startSearchAddressSpinBox);
    //    addressFormLayout->addRow(m_timeoutLabel, m_timeoutSpinbox);d

    m_beginSearchButton = new QPushButton(tr("Поиск КСУ"));
    m_cancelSearchButton = new QPushButton(tr("Отменить"));

    connect(m_beginSearchButton, &QPushButton::clicked, this, &SelectKsuWindow::search);

    QDialogButtonBox *searchButtonBox = new QDialogButtonBox(Qt::Horizontal);
    searchButtonBox->addButton(m_beginSearchButton, QDialogButtonBox::ActionRole);
    searchButtonBox->addButton(m_cancelSearchButton, QDialogButtonBox::RejectRole);

    searchVBoxLayout->addLayout(addressFormLayout);
    searchVBoxLayout->addSpacing(20);
    // TODO: search hiden
    //    searchVBoxLayout->addWidget(searchButtonBox);

    searchSettingsgroupBox->setLayout(searchVBoxLayout);

    QGroupBox *connectionSettingsgroupBox = new QGroupBox(tr("Настройка подключения"));
    QVBoxLayout *vbox = new QVBoxLayout;
    m_portSettings = new KSUPortConnectionWidget;
    vbox->addWidget(m_portSettings);
    connectionSettingsgroupBox->setLayout(vbox);

    errorMessageLabel = new QLabel;
    errorMessageLabel->clear();
    errorMessageLabel->setStyleSheet("color: darkred");

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_okButton = new QPushButton(tr("Выполнить"));
    m_okButton->setDefault(true);
    m_okButton->setAutoDefault(true);
    m_okButton->setShortcut(Qt::Key_Return);

    m_cancelButton = new QPushButton(tr("Отменить"));

    buttonBox->addButton(m_okButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_okButton, &QPushButton::clicked, this, &SelectKsuWindow::okButtonPressed);
    connect(m_cancelButton, &QPushButton::clicked, this, &SelectKsuWindow::cancelButtonPressed);
    connect(this, &SelectKsuWindow::destroyed, this, &SelectKsuWindow::cancelButtonPressed);

    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(searchSettingsgroupBox);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(connectionSettingsgroupBox);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(errorMessageLabel, 0, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

#ifdef SAP_KSU
    m_ksuTypeComboBox->setCurrentIndex(-1);
    m_ksuSoftwareVersionComboBox->setCurrentIndex(-1);
#endif
#ifdef SAP_SU
    m_ksuTypeComboBox->setCurrentIndex(0);
    currenKsuTypeChanged(0);
    m_ksuSoftwareVersionComboBox->setCurrentIndex(0);
#endif
#ifdef MECHOS
    m_ksuTypeComboBox->setCurrentIndex(0);
    currenKsuTypeChanged(0);
    m_ksuSoftwareVersionComboBox->setCurrentIndex(0);
#endif

}

void SelectKsuWindow::selectKsuTypes()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `KsuTypes` WHERE `id` IN (SELECT DISTINCT `ksuTypeId` FROM `KsuSoftwareVersions`) ORDER BY `id` ASC");

    QSqlQuery query(sqlString, db);

    m_ksuTypeModel->setQuery(query);
}

void SelectKsuWindow::selectKsuSoftwareVersion(int idKsuType)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `KsuSoftwareVersions` "
                                "WHERE `ksuTypeId` = %1 "
                                "ORDER BY `softwareVersion` ASC").arg(idKsuType);

    QSqlQuery query(sqlString, db);

    m_ksuSoftwareVersionModel->setQuery(query);
    if (m_ksuSoftwareVersionModel->rowCount() > 0) {
        m_ksuSoftwareVersionComboBox->setCurrentIndex(0);
    } else {
        m_ksuSoftwareVersionComboBox->setCurrentIndex(-1);
    }
}

void SelectKsuWindow::okButtonPressed()
{
    int index = m_ksuSoftwareVersionComboBox->currentIndex();
    if (index < 0 || index >= m_ksuSoftwareVersionModel->rowCount()) {
        ErrorClassifier::instance()->showError(50);
        return;
    }

    if (!checkUnfilledFields())
        return;

    QMessageBox msgBox;

    msgBox.setWindowTitle("Питание контроллера КСУ");
    msgBox.setText("ВНИМАНИЕ!");

#ifdef SAP_KSU
    msgBox.setInformativeText(QString("На контроллер будет подано напряжение в соответствии с выбранным типом КСУ:\n"
                                      "%1.\n"
                                      "Убедитесь в правильности подключенного типа КСУ.\n"
                                      "Неверный выбор типа КСУ приведет к неисправности подключенного КСУ\n"
                                      "Вы действительно хотите продолжить?").arg(m_ksuTypeComboBox->currentText()));
#endif
#ifdef SAP_SU
    msgBox.setInformativeText(QString("На станцию управления будет подано питающее напряжение.\n"
                                      "Убедитесь в правильности подключения станции управления.\n"
                                      "Вы действительно хотите продолжить?").arg(m_ksuTypeComboBox->currentText()));
#endif
#ifdef MECHOS
    msgBox.setInformativeText(QString("На станцию управления будет подано питающее напряжение.\n"
                                      "Убедитесь в правильности подключения станции управления.\n"
                                      "Вы действительно хотите продолжить?").arg(m_ksuTypeComboBox->currentText()));
#endif

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(400);
    msgBox.setDefaultButton(QMessageBox::No);

    QPushButton *okButton = msgBox.addButton("Да", QMessageBox::AcceptRole);
    QPushButton *noButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);

    msgBox.exec();

    if (msgBox.clickedButton() == noButton) {
        return;
    }

    m_portSettings->writeSettings();

    disconnect(this, &SelectKsuWindow::destroyed, this, &SelectKsuWindow::cancelButtonPressed);
    close();

    emit ksuSelected();

    QSqlRecord record = m_ksuSoftwareVersionModel->record(index);
    IOServer::createIOServer(record.value(QStringLiteral("id")).toInt(), m_ksuAddressSpinBox->value());

    accept();
}

void SelectKsuWindow::cancelButtonPressed()
{
    emit ksuUnselected();
    reject();
}

void SelectKsuWindow::currenKsuTypeChanged(int index)
{
    if (index < 0 || index >= m_ksuTypeModel->rowCount()) {
        return;
    }

    QSqlRecord record = m_ksuTypeModel->record(index);
    m_manufacturerLineEdit->setText(record.value(QStringLiteral("manufacturer")).toString());
    bool ok = false;
    int id = record.value(QStringLiteral("id")).toInt(&ok);
    ok ? m_currentKsuTypeId = id : m_currentKsuTypeId = -1;

    selectKsuSoftwareVersion(m_currentKsuTypeId);

    //    int rowCount = m_ksuTypeModel->rowCount();

    //    for (int i = 0; i < rowCount; i++) {

    //        QSqlRecord record = m_ksuTypeModel->record(i);
    //        QString iKsuType = record.value(QStringLiteral("ksuName")).toString();

    //        if (index == iKsuType) {

    //            m_manufacturerLineEdit->setText(record.value(QStringLiteral("manufacturer")).toString());

    //            bool ok = false;
    //            int id = record.value(QStringLiteral("id")).toInt(&ok);
    //            ok ? m_currentKsuTypeId = id : m_currentKsuTypeId = -1;

    //            break;
    //        }
    //    }

    //    selectKsuSoftwareVersion(m_currentKsuTypeId);
}

void SelectKsuWindow::currentSoftwareVersionChanged(int index)
{
    if (index < 0 || index >= m_ksuSoftwareVersionModel->rowCount()) {
        return;
    }

    QSqlRecord record = m_ksuSoftwareVersionModel->record(index);
    bool ok = false;

    if (record.value(QStringLiteral("protocolType")).toInt(&ok)) {
        m_currentKsuProtocolType = SapKsu::Modbus ;
    } else {
        m_currentKsuProtocolType = SapKsu::Ksu735 ;
    }


    //    int rowCount = m_ksuSoftwareVersionModel->rowCount();

    //    for (int i = 0; i < rowCount; i++) {

    //        QSqlRecord record = m_ksuSoftwareVersionModel->record(i);
    //        QString iSowtwareVersion = record.value(QStringLiteral("softwareVersion")).toString();

    //        if (index == iSowtwareVersion) {

    //            bool ok = false;

    //            if (record.value(QStringLiteral("protocolType")).toInt(&ok)) {
    //                m_currentKsuProtocolType = SapKsu::Modbus ;
    //            } else {
    //                m_currentKsuProtocolType = SapKsu::Ksu735 ;
    //            }

    //            break;
    //        }
    //    }
}

bool SelectKsuWindow::checkUnfilledFields()
{
    errorMessageLabel->clear();

    if (m_ksuTypeComboBox->currentIndex() == -1) {
        errorMessageLabel->setText(tr("Необходимо выбрать тип КСУ"));
        return false;
    }

    if (m_ksuSoftwareVersionComboBox->currentIndex() == -1) {
        errorMessageLabel->setText(tr("Необходимо выбрать версию ПО КСУ"));
        return false;
    }

    return true;
}

void SelectKsuWindow::search()
{
    if (!checkUnfilledFields())
        return;

    qDebug() << m_ksuAddressSpinBox->value();
    qDebug() << m_startSearchAddressSpinBox->value();
    qDebug() << m_timeoutSpinbox->value();
    qDebug() << m_currentKsuProtocolType;
    qDebug() << m_portSettings->portName();
    qDebug() << m_portSettings->baudRate();
    qDebug() << m_portSettings->dataBits();
    qDebug() << m_portSettings->parity();
    qDebug() << m_portSettings->stopBits();
    qDebug() << m_portSettings->flowControl();
    qDebug() << m_portSettings->characterSpacing();
}

void SelectKsuWindow::closeEvent(QCloseEvent *event)
{
    OperatorClickReport::instance()->write(QStringLiteral("Окно настройки тестирования: закрытие окна"));
    QWidget::closeEvent(event);
}

void SelectKsuWindow::showEvent(QShowEvent *event)
{
    activateWindow();
    QDialog::showEvent(event);
}
