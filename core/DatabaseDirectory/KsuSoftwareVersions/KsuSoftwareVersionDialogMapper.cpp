#include "KsuSoftwareVersionDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "ErrorClassifier/ErrorClassifier.h"

KsuSoftwareVersionDialogMapper::KsuSoftwareVersionDialogMapper(QWidget *parent) :
    QWidget(parent)
{
    m_ksuTypesModel = new QSqlQueryModel(this);
    m_communicationProtocolsModel = new QSqlQueryModel(this);
    m_classifierTemplatesModel = new QSqlQueryModel(this);

    fillKSUTypes();
    fillCommunicationProtocols();
    fillClassifierTemplates();

    selectCurrentSoftwareVersion();

    m_ksuTypeComboBox = new QComboBox();
    m_ksuTypeComboBox->setModel(m_ksuTypesModel);
    m_ksuTypeComboBox->setModelColumn(1);

    m_ksuTypeLabel = new QLabel(tr("Тип Ксу"));
    m_ksuTypeLabel->setBuddy(m_ksuTypeComboBox);

    m_softwareVersionLineEdit = new QLineEdit;
    m_softwareVersionLabel = new QLabel(tr("Версия ПО КСУ"));
    m_softwareVersionLabel->setBuddy(m_softwareVersionLineEdit);


    const QIcon m_protocol735Icon = QIcon(":/Images/crack-attack.png");
    const QIcon m_protocolModbusIcon = QIcon(":/Images/emblem-art.png");

    m_protocolTypeComboBox = new QComboBox();
    m_protocolTypeComboBox->insertItem(0, m_protocol735Icon, "7.35 1B");
    m_protocolTypeComboBox->insertItem(1, m_protocolModbusIcon, "Modbus");
    connect(m_protocolTypeComboBox,     static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,                       &KsuSoftwareVersionDialogMapper::protocolTypeChanged);

    m_protocolTypeLabel = new QLabel(tr("Тип протокола"));
    m_protocolTypeLabel->setBuddy(m_protocolTypeComboBox);

    m_communicationProtocolComboBox = new QComboBox();
    m_communicationProtocolComboBox->setModel(m_communicationProtocolsModel);
    m_communicationProtocolComboBox->setModelColumn(1);

    m_communicationProtocolLabel = new QLabel(tr("Протокол обмена"));
    m_communicationProtocolLabel->setBuddy(m_communicationProtocolComboBox);

    m_classifierTemplateComboBox = new QComboBox();
    m_classifierTemplateComboBox->setModel(m_classifierTemplatesModel);
    m_classifierTemplateComboBox->setModelColumn(1);
    connect(m_classifierTemplateComboBox,   static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,                           &KsuSoftwareVersionDialogMapper::classifierTemplateChanged);

    m_classifierTemplateLabel = new QLabel(tr("Классификатор"));
    m_classifierTemplateLabel->setBuddy(m_classifierTemplateComboBox);

    m_classifierNameLineEdit = new QLineEdit();
    m_classifierNameLabel = new QLabel(tr("Название классификатора"));
    m_classifierNameLabel->setBuddy(m_classifierNameLineEdit);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_ksuTypeLabel, m_ksuTypeComboBox);
    formLayout->addRow(m_softwareVersionLabel, m_softwareVersionLineEdit);
    formLayout->addRow(m_protocolTypeLabel, m_protocolTypeComboBox);
    formLayout->addRow(m_communicationProtocolLabel, m_communicationProtocolComboBox);
    formLayout->addRow(m_classifierTemplateLabel, m_classifierTemplateComboBox);
    formLayout->addRow(m_classifierNameLabel, m_classifierNameLineEdit);


    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &KsuSoftwareVersionDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &KsuSoftwareVersionDialogMapper::close);

    vlayout->addLayout(formLayout);
    vlayout->addWidget(buttonBox);

    setLayout(vlayout);

    resize(400, 150);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Редактор параметров"));
}

void KsuSoftwareVersionDialogMapper::setKSUSoftwareVersionId(int softwareVersionId)
{
    m_softwareVersionId = softwareVersionId;
    selectCurrentSoftwareVersion();
}

void KsuSoftwareVersionDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QDir ksuTemplatesDir(DBConfig::parametrsTemplatesFolderPath());
    QDir ksuParametrsDir(DBConfig::ksuParametrsFolderPath());
    if (!ksuParametrsDir.mkpath(ksuParametrsDir.path()))
        return;

    if (!checkUnfilledFields())
        return;

    bool ok;
    int ksuTypeId = m_ksuTypesModel->index(m_ksuTypeComboBox->currentIndex(), 0).data().toInt(&ok);
    if (!ok)
        return;

    int comProtocolCode = m_communicationProtocolsModel->index(
                              m_communicationProtocolComboBox->currentIndex(), 0
                              ).data().toInt(&ok);
    if (!ok)
        return;

    QString parametrsClassifierName;
    QString classifierNameText = m_classifierNameLineEdit->text().trimmed();
    if (classifierNameText.isEmpty()) {
        parametrsClassifierName = "NULL";
    } else {
        parametrsClassifierName = '"' + classifierNameText + '"';
    }

    int classifierTemplateIndex = m_classifierTemplateComboBox->currentIndex();

    int parametrsTemplateId = m_classifierTemplatesModel->index(classifierTemplateIndex, 0).data().toInt(&ok);
    QString parametrsTemplateIdString = QString::number(parametrsTemplateId);
    if (!ok || m_classifierTemplateComboBox->currentIndex() == -1) {
        parametrsTemplateIdString = "NULL";
        parametrsTemplateId = -1;
    }

    QString templateFileName = m_classifierTemplatesModel->index(classifierTemplateIndex, 2).data().toString();
    QString templateFilePath = ksuTemplatesDir.absoluteFilePath(templateFileName);

    QString parametrsFileName = m_parametsConfigFileName;
    if (m_parametsConfigFileName.isEmpty()) {
        parametrsFileName = QUuid::createUuid().toString();
    }
    QString parametrsFilePath = ksuParametrsDir.absoluteFilePath(parametrsFileName);

    if (m_operationIsUpdate) {
        QString sqlString = QString("UPDATE KsuSoftwareVersions \r\n"
                                    "SET softwareVersion = '%1', \r\n"
                                    "protocolType = %2, \r\n"
                                    "parametrsClassifierName = %3, \r\n"
                                    "parametrsConfigPath = '%4', \r\n"
                                    "ksuTypeId = %5, \r\n"
                                    "comProtocolCode = %6, \r\n"
                                    "parametrsTemplateId = %7 \r\n"
                                    "WHERE id = %8;")
                            .arg(m_softwareVersionLineEdit->text().trimmed())
                            .arg(m_protocolTypeComboBox->currentIndex())
                            .arg(parametrsClassifierName)
                            .arg(parametrsFileName)
                            .arg(ksuTypeId)
                            .arg(comProtocolCode)
                            .arg(parametrsTemplateIdString)
                            .arg(m_softwareVersionId);

        db.transaction();
        if (query.exec(sqlString)) {
            bool copyResult = true;

            if (m_currentClassifierTemplateId != -1 || parametrsTemplateId != m_currentClassifierTemplateId) {
                copyResult = copyClassifierTemplate(templateFilePath, parametrsFilePath, true);
            }
            if (copyResult) {
                if (!db.commit()){
                    db.rollback();
                }

                emit dataChanged();
                close();
                return;
            }
        } else {
            ErrorClassifier::instance()->showError(501);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + "\n" +
                                           query.lastError().databaseText());
        }
    } else {
        QString sqlString = QString("INSERT INTO KsuSoftwareVersions("
                                    "softwareVersion, protocolType, parametrsClassifierName, parametrsConfigPath, "
                                    "ksuTypeId, comProtocolCode, parametrsTemplateId) "
                                    "VALUES ('%1', %2, %3, '%4', %5, %6, %7)")
                            .arg(m_softwareVersionLineEdit->text().trimmed())
                            .arg(m_protocolTypeComboBox->currentIndex())
                            .arg(parametrsClassifierName)
                            .arg(parametrsFileName)
                            .arg(ksuTypeId)
                            .arg(comProtocolCode)
                            .arg(parametrsTemplateIdString);

        db.transaction();
        if (query.exec(sqlString)) {
            if (copyClassifierTemplate(templateFilePath, parametrsFilePath)) {
                if (!db.commit()){
                    db.rollback();
                }

                emit dataChanged();
                close();
                return;
            }
        } else {
            ErrorClassifier::instance()->showError(501);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + "\n" +
                                           query.lastError().databaseText());
        }
    }
}

void KsuSoftwareVersionDialogMapper::protocolTypeChanged(int protocolType)
{
    m_protocolType = static_cast<SapKsu::KSUProtocolType>(protocolType);

    fillCommunicationProtocols();
    fillClassifierTemplates();

    if (m_operationIsUpdate) {
        setCurrentCommunicationProtocol(-1);
        setCurrentClassifierTemplate(-1);
    } else {
        m_communicationProtocolComboBox->setCurrentIndex(0);
        m_classifierTemplateComboBox->setCurrentIndex(0);
    }
}

void KsuSoftwareVersionDialogMapper::classifierTemplateChanged(int index)
{
    Q_UNUSED(index)

    if (!m_changeClassifierWarningShown && m_operationIsUpdate) {
        if (!showChangeClassifierTemplateWarning()) {
            setCurrentClassifierTemplate(m_currentClassifierTemplateId);
            return;
        } else {
            m_changeClassifierWarningShown = true;
        }
    }
}

void KsuSoftwareVersionDialogMapper::selectCurrentSoftwareVersion()
{
    if (m_softwareVersionId == -1)
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM KsuSoftwareVersions WHERE id = %1").arg(m_softwareVersionId);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect() || !query.first())
        return;

    m_operationIsUpdate = true;

    bool ok;
    int currentKsuTypeId = query.value(QStringLiteral("ksuTypeId")).toInt(&ok);
    if (ok) {
        setCurrentKSUType(currentKsuTypeId);
    }

    m_softwareVersionLineEdit->setText(query.value(QStringLiteral("softwareVersion")).toString());
    m_classifierNameLineEdit->setText(query.value(QStringLiteral("parametrsClassifierName")).toString());
    m_parametsConfigFileName = query.value(QStringLiteral("parametrsConfigPath")).toString();

    int protocolType = query.value(QStringLiteral("protocolType")).toInt(&ok);
    m_protocolType = static_cast<SapKsu::KSUProtocolType>(protocolType);
    if (ok) {
        m_protocolTypeComboBox->setCurrentIndex(m_protocolType);
    }
    m_protocolTypeComboBox->setEnabled(false);

    int currentCommunicationProtocolCode = query.value(QStringLiteral("comProtocolCode")).toInt(&ok);
    if (ok) {
        setCurrentCommunicationProtocol(currentCommunicationProtocolCode);
    }

    int currentClassifierTemplateId = query.value(QStringLiteral("parametrsTemplateId")).toInt(&ok);
    if (ok) {
        m_currentClassifierTemplateId = currentClassifierTemplateId;
    } else {
        m_currentClassifierTemplateId = -1;
    }
    setCurrentClassifierTemplate(m_currentClassifierTemplateId);
}

bool KsuSoftwareVersionDialogMapper::copyClassifierTemplate(const QString &sourcePath,
                                                            const QString &destinationPath,
                                                            bool replaceIfExist)
{
    if (sourcePath.isEmpty() || !QFile::exists(sourcePath))
        return false;

    if (QFile::exists(destinationPath)) {
        if (replaceIfExist)
            QFile::remove(destinationPath);
        else
            return false;
    }

    if (!QFile::copy(sourcePath, destinationPath))
        return false;

    QFile createdFile(destinationPath);
    createdFile.setPermissions(QFile::ReadOwner |
                               QFile::WriteOwner |
                               QFileDevice::WriteOther |
                               QFileDevice::ReadOther);

    return true;
}

void KsuSoftwareVersionDialogMapper::fillKSUTypes()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT id, ksuName FROM KsuTypes ORDER BY code ASC");
    QSqlQuery query(sqlString, db);

    m_ksuTypesModel->setQuery(query);
}

void KsuSoftwareVersionDialogMapper::setCurrentKSUType(int ksuTypeId)
{
    if (ksuTypeId == -1) {
        m_ksuTypeComboBox->setCurrentIndex(-1);
        return;
    }

    int rowCount = m_ksuTypesModel->rowCount();
    for (int i = 0; i < rowCount; i++) {
        QSqlRecord &&record = m_ksuTypesModel->record(i);
        bool ok;
        int id = record.value(QStringLiteral("id")).toInt(&ok);

        if (ok && ksuTypeId == id) {
            m_ksuTypeComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void KsuSoftwareVersionDialogMapper::fillCommunicationProtocols()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT code, name FROM CommunicationProtocols WHERE protocolType = %1 ORDER BY code ASC")
                        .arg(m_protocolType);
    QSqlQuery query(sqlString, db);

    m_communicationProtocolsModel->setQuery(query);
}

void KsuSoftwareVersionDialogMapper::setCurrentCommunicationProtocol(int protocolCode)
{
    if (protocolCode == -1) {
        m_communicationProtocolComboBox->setCurrentIndex(-1);
        return;
    }

    int rowCount = m_communicationProtocolsModel->rowCount();
    for (int i = 0; i < rowCount; i++) {
        QSqlRecord record = m_communicationProtocolsModel->record(i);
        bool ok;
        int code = record.value(QStringLiteral("code")).toInt(&ok);

        if (ok && protocolCode == code) {
            m_communicationProtocolComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void KsuSoftwareVersionDialogMapper::fillClassifierTemplates()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT id, name, parametrsConfigPath FROM KSUClassifierParametersTemplates \r\n"
                                "WHERE protocolType = %1 ORDER BY code ASC")
                        .arg(m_protocolType);
    QSqlQuery query(sqlString, db);

    if (m_classifierTemplateComboBox) m_classifierTemplateComboBox->blockSignals(true);
    m_classifierTemplatesModel->setQuery(query);
    if (m_classifierTemplateComboBox) m_classifierTemplateComboBox->blockSignals(false);
}

void KsuSoftwareVersionDialogMapper::setCurrentClassifierTemplate(int templateId)
{
    if (templateId == -1) {
        m_classifierTemplateComboBox->blockSignals(true);
        m_classifierTemplateComboBox->setCurrentIndex(-1);
        m_classifierTemplateComboBox->blockSignals(false);
        return;
    }

    int rowCount = m_classifierTemplatesModel->rowCount();
    for (int i = 0; i < rowCount; i++) {
        QSqlRecord record = m_classifierTemplatesModel->record(i);
        bool ok;
        int id = record.value(QStringLiteral("id")).toInt(&ok);

        if (ok && templateId == id) {
            m_classifierTemplateComboBox->blockSignals(true);
            m_classifierTemplateComboBox->setCurrentIndex(i);
            m_classifierTemplateComboBox->blockSignals(false);
            break;
        }
    }
}

bool KsuSoftwareVersionDialogMapper::showChangeClassifierTemplateWarning()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Предупреждение"));
    msgBox.setText("Сменить классификатор параметров?");
    msgBox.setInformativeText("При смене классификатора параметров все настройки, относящиеся к данной версии ПО КСУ будут утеряны. "
                              "Для подтверждения смены классификатора параметров нажмите \"Да\"");

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(400);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    msgBox.addButton(tr("&Нет"), QMessageBox::NoRole);

    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.exec();
    return msgBox.clickedButton() == yesButton;
}

bool KsuSoftwareVersionDialogMapper::checkUnfilledFields()
{
    if (m_ksuTypeComboBox->currentIndex() == -1) {
        ErrorClassifier::instance()->showError(18);
        return false;
    }

    if (m_softwareVersionLineEdit->text().trimmed().isEmpty()) {
        ErrorClassifier::instance()->showError(19);
        return false;
    }

    if (m_communicationProtocolComboBox->currentIndex() == -1) {
        ErrorClassifier::instance()->showError(20);
        return false;
    }

    QDir ksuParametrsDir(DBConfig::ksuParametrsFolderPath());
    QString parametrsConfigPath = ksuParametrsDir.absoluteFilePath(m_parametsConfigFileName);

    QFileInfo fileInfo(parametrsConfigPath);
    if (m_classifierTemplateComboBox->currentIndex() == -1 && !fileInfo.isFile()) {
        ErrorClassifier::instance()->showError(21);
        return false;
    }

    return true;
}
