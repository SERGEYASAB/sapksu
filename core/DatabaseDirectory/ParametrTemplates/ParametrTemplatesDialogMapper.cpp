#include "ParametrTemplatesDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

// CRITICAL: Вылет происходит! Если сразу нажать в окне кнопку редактировать и нажать эскейп

ParametrTemplatesDialogMapper::ParametrTemplatesDialogMapper(QSqlQueryModel *model, bool disableType, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    m_code = new QLabel(tr("&Код"));
    m_protocolType = new QLabel(tr("&Протокол"));
    m_name = new QLabel(tr("&Название"));
    m_parametsConfigPath = new QLabel(tr("Путь конфигурации"));

    m_codeSpinBox = new QSpinBox();
    m_codeSpinBox->setRange(1, std::numeric_limits<int>::max());

    QSqlQueryModel *queryModel = qobject_cast<QSqlQueryModel *>(model);
    if (queryModel) {
        auto record = queryModel->record(queryModel->rowCount() - 1);
        auto lastCodeVariant = record.value("code");
        bool ok = false;
        int lastIntCode = lastCodeVariant.toInt(&ok);
        if (ok) {
            m_codeSpinBox->setValue(lastIntCode + 1);
        }
    }

    const QIcon m_protocol735Icon = QIcon(":/Images/crack-attack.png");
    const QIcon m_protocolModbusIcon = QIcon(":/Images/emblem-art.png");

    m_protocolTypeComboBox = new QComboBox();
    m_protocolTypeComboBox->insertItem(0, m_protocol735Icon, "7.35 1B");
    m_protocolTypeComboBox->insertItem(1, m_protocolModbusIcon, "Modbus");
    m_protocolTypeComboBox->setDisabled(disableType);

    m_nameLineEdit = new QLineEdit();
    m_parametsConfigPathLineEdit = new QLineEdit();
    m_parametsConfigPathLineEdit->setDisabled(true);

    m_code->setBuddy(m_codeSpinBox);
    m_protocolType->setBuddy(m_protocolTypeComboBox);
    m_name->setBuddy(m_nameLineEdit);
    m_parametsConfigPath->setBuddy(m_parametsConfigPathLineEdit);

    QMap<int, QString> delegateMap;
    delegateMap[0] = "7.35 1B";
    delegateMap[1] = "Modbus";

    FKComboBoxDelegate *delegate = new FKComboBoxDelegate(delegateMap, this);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(m_model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_codeSpinBox, 1);
    m_mapper->addMapping(m_protocolTypeComboBox, 2);
    m_mapper->addMapping(m_nameLineEdit, 3);
    m_mapper->addMapping(m_parametsConfigPathLineEdit, 4);
    m_mapper->setItemDelegate(delegate);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_code, m_codeSpinBox);
    formLayout->addRow(m_protocolType, m_protocolTypeComboBox);
    formLayout->addRow(m_name, m_nameLineEdit);
    formLayout->addRow(m_parametsConfigPath, m_parametsConfigPathLineEdit);

#ifndef QT_NO_DEBUG
    m_parametsConfigPath->setVisible(false);
    m_parametsConfigPathLineEdit->setVisible(false);
#endif

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &ParametrTemplatesDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &ParametrTemplatesDialogMapper::close);

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

void ParametrTemplatesDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
    m_codeSpinBox->setEnabled(!index.isValid());
}

void ParametrTemplatesDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QDir dir(DBConfig::parametrsTemplatesFolderPath());
    if (!dir.mkpath(dir.path()))
        return;

    QString name = m_nameLineEdit->text();
    QString configFileName = QString(QCryptographicHash::hash((name.toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
    QString configPath = dir.absoluteFilePath(configFileName);

    if (m_mapper->currentIndex() == -1) {
        QString sqlString = QString("INSERT INTO `KSUClassifierParametersTemplates` "
                                    "(`code`, `protocolType`, `name`, `parametrsConfigPath`)"
                                    "VALUES (%1, %2, '%3', '%4');")
                            .arg(m_codeSpinBox->value())
                            .arg(m_protocolTypeComboBox->currentIndex())
                            .arg(name)
                            .arg(configFileName);
        if (query.exec(sqlString)) {
            SapKsu::KSUProtocolType protocol = static_cast<SapKsu::KSUProtocolType>(m_protocolTypeComboBox->currentIndex());
            copyTemplate(protocol, configPath);
            emit dataChanged();
            close();
        } else {
            ErrorClassifier::instance()->showError(26);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(26) + query.lastError().databaseText());
        }
    } else {
        QSqlRecord &&record = m_model->record(m_mapper->currentIndex());

        QString oldConfigPath = dir.absoluteFilePath(record.value("parametrsConfigPath").toString());
        QString sqlString = QString("UPDATE `KSUClassifierParametersTemplates` \r\n"
                                    "SET code = %1, protocolType = %2, name = '%3', parametrsConfigPath = '%4' \r\n"
                                    "WHERE code = %5;")
                            .arg(m_codeSpinBox->value())
                            .arg(m_protocolTypeComboBox->currentIndex())
                            .arg(name)
                            .arg(configFileName)
                            .arg(record.value("code").toString());

        if (query.exec(sqlString)) {
            QFile::rename(oldConfigPath, configPath);
            emit dataChanged();
            close();
        } else {
            ErrorClassifier::instance()->showError(26);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(26) + query.lastError().databaseText());
        }
    }
}

bool ParametrTemplatesDialogMapper::copyTemplate(SapKsu::KSUProtocolType type, const QString &filePath)
{
    QString templateFilePath;
    if (type == SapKsu::Ksu735) {
        templateFilePath = ":/DBConfig/KSU735ParamTemplate.xml";
    } else if (type == SapKsu::Modbus) {
#ifdef SAP_KSU
        templateFilePath = ":/DBConfig/KSUModbusParamTemplate.xml";
#endif
#ifdef SAP_SU
        templateFilePath = ":/DBConfig/KSUModbusParamTemplate.xml";
#endif
#ifdef MECHOS
        templateFilePath = ":/DBConfig/Mechos/DefaultModbusParamTemplate.xml";
#endif
    }

    if (templateFilePath.isEmpty() || !QFile::exists(templateFilePath))
        return false;

    if (!QFile::copy(templateFilePath, filePath))
        return false;

    QFile::setPermissions(filePath,
                          QFile::ReadOwner |
                          QFile::WriteOwner |
                          QFileDevice::WriteOther |
                          QFileDevice::ReadOther);

    return true;
}
