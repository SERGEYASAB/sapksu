#include "SAPParametrsDialogMapper.h"
#include "DBConfig/DBConfig.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

SAPParametrsDialogMapper::SAPParametrsDialogMapper(QSqlQueryModel *model, bool disableType, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    m_code = new QLabel(tr("&Код"));
    m_name = new QLabel(tr("&Название"));
    m_description = new QLabel(tr("&Описание"));
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

    m_nameLineEdit = new QLineEdit();
    m_descriptionLineEdit = new QLineEdit();
    m_parametsConfigPathLineEdit = new QLineEdit();
    m_parametsConfigPathLineEdit->setDisabled(true);

    m_code->setBuddy(m_codeSpinBox);
    m_name->setBuddy(m_nameLineEdit);
    m_description->setBuddy(m_descriptionLineEdit);
    m_parametsConfigPath->setBuddy(m_parametsConfigPathLineEdit);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(m_model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_codeSpinBox, 1);
    m_mapper->addMapping(m_nameLineEdit, 2);
    m_mapper->addMapping(m_descriptionLineEdit, 3);
    m_mapper->addMapping(m_parametsConfigPathLineEdit, 4);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_code, m_codeSpinBox);
    formLayout->addRow(m_name, m_nameLineEdit);
    formLayout->addRow(m_description, m_descriptionLineEdit);
    formLayout->addRow(m_parametsConfigPath, m_parametsConfigPathLineEdit);

//#ifndef QT_NO_DEBUG
//    m_parametsConfigPath->setVisible(false);
//    m_parametsConfigPathLineEdit->setVisible(false);
//#endif

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &SAPParametrsDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &SAPParametrsDialogMapper::close);

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

void SAPParametrsDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
    m_codeSpinBox->setEnabled(!index.isValid());
    if (!index.isValid()) {
        m_codeSpinBox->setMinimum(20);
    }
}

void SAPParametrsDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QDir dir(DBConfig::sapParametrsFolderPath());
    if (!dir.mkpath(dir.path()))
        return;

    QString name = m_nameLineEdit->text();
    QString configFileName = QString(QCryptographicHash::hash((name.toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
    QString configPath = dir.absoluteFilePath(configFileName);

    if (m_mapper->currentIndex() == -1) {
        QString sqlString = QString("INSERT INTO `SAPParametrs` "
                                    "(`code`, `name`, `description`, `parametrsConfigPath`)"
                                    "VALUES (%1, '%2', '%3', '%4');")
                .arg(m_codeSpinBox->value())
                .arg(name)
                .arg(m_descriptionLineEdit->text())
                .arg(configFileName);

        if (query.exec(sqlString)) {

            copyTemplate(configPath);
            emit dataChanged();
            close();
        } else {
            ErrorClassifier::instance()->showError(28);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(28) + query.lastError().databaseText());
        }
    } else {
        QSqlRecord &&record = m_model->record(m_mapper->currentIndex());

        QString oldConfigPath = dir.absoluteFilePath(record.value("parametrsConfigPath").toString());
        QString sqlString = QString("UPDATE `SAPParametrs` \r\n"
                                    "SET name = '%1', description = '%2', parametrsConfigPath = '%3' \r\n"
                                    "WHERE id = %4;")
                .arg(name)
                .arg(m_descriptionLineEdit->text())
                .arg(configFileName)
                .arg(record.value("id").toString());

        if (query.exec(sqlString)) {
            QFile::rename(oldConfigPath, configPath);
            emit dataChanged();
            close();
        } else {
            ErrorClassifier::instance()->showError(28);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(28) + query.lastError().databaseText());
        }
    }
}

bool SAPParametrsDialogMapper::copyTemplate(const QString &filePath)
{
    QString templateFilePath;
#ifdef SAP_KSU
    templateFilePath = ":/DBConfig/SAPParametrs/KSU_Default_Template.xml";
#endif

#ifdef SAP_SU
    templateFilePath = ":/DBConfig/SAPParametrs/SAPSU_Default_Template.xml";
#endif

#ifdef MECHOS
    templateFilePath = ":/DBConfig/SAPParametrs/Mechos_Default_Template.xml";
#endif

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
