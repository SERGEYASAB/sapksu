#include "KsuTypesDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

KsuTypesDialogMapper::KsuTypesDialogMapper(QSqlQueryModel *model, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    m_sapParametrsModel = new QSqlQueryModel(this);

    fillSAPParametrsTypes();
    selectCurrentKsuType();

    m_codeLabel = new QLabel(tr("&Код"));
    m_ksuNameLabel = new QLabel(tr("&Тип КСУ"));
    m_manufacturerLabel = new QLabel(tr("&Завод-изготовитель"));
    m_sapParametrsLabel = new QLabel(tr("&Параметры САП"));

    m_codeSpinBox = new QSpinBox();
    m_codeSpinBox->setRange(20, std::numeric_limits<int>::max());

    if (model) {
        auto record = model->record(model->rowCount() - 1);
        auto lastCodeVariant = record.value("code");
        bool ok = false;
        int lastIntCode = lastCodeVariant.toInt(&ok);
        if (ok) {
            m_codeSpinBox->setValue(lastIntCode + 1);
        }
    }

    m_ksuNameLineEdit = new QLineEdit();
    m_manufacturerLineEdit = new QLineEdit();

    m_sapParametrsComboBox = new QComboBox();
    m_sapParametrsComboBox->setModel(m_sapParametrsModel);
    m_sapParametrsComboBox->setModelColumn(1);
    m_sapParametrsComboBox->setCurrentIndex(-1);

    m_codeLabel->setBuddy(m_codeSpinBox);
    m_ksuNameLabel->setBuddy(m_ksuNameLineEdit);
    m_manufacturerLabel->setBuddy(m_manufacturerLineEdit);
    m_sapParametrsLabel->setBuddy(m_sapParametrsComboBox);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_codeLabel, m_codeSpinBox);
    formLayout->addRow(m_ksuNameLabel, m_ksuNameLineEdit);
    formLayout->addRow(m_manufacturerLabel, m_manufacturerLineEdit);
    formLayout->addRow(m_sapParametrsLabel, m_sapParametrsComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &KsuTypesDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &KsuTypesDialogMapper::close);

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

void KsuTypesDialogMapper::setKsuTypeId(int ksuTypeId)
{
    m_ksuTypeId = ksuTypeId;
    selectCurrentKsuType();
}

void KsuTypesDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    if (!checkUnfilledFields())
        return;

    bool ok;
    int sapParamId = m_sapParametrsModel->index(m_sapParametrsComboBox->currentIndex(), 0).data().toInt(&ok);
    if (!ok)
        return;

    if (m_operationIsUpdate) {

        QString sqlString = QString("UPDATE `KsuTypes` "
                                    "SET code = %1, ksuName = '%2', "
                                    "manufacturer = '%3', "
                                    "sapParamId = %4 "
                                    "WHERE id = %5;")
                            .arg(m_codeSpinBox->value())
                            .arg(m_ksuNameLineEdit->text())
                            .arg(m_manufacturerLineEdit->text())
                            .arg(sapParamId)
                            .arg(m_ksuTypeId);

        db.transaction();

        if (query.exec(sqlString)) {

            if (!db.commit()){
                db.rollback();
            }

            emit dataChanged();
            close();
            return;

        }  else {
            ErrorClassifier::instance()->showError(501);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
        }
    } else {
        QString sqlString = QString("INSERT INTO `KsuTypes` "
                                    "("
                                    "`code`, "
                                    "`ksuName`, "
                                    "`manufacturer`,"
                                    "`sapParamId`"
                                    ") "
                                    "VALUES (%1, '%2', '%3', %4);")
                            .arg(m_codeSpinBox->value())
                            .arg(m_ksuNameLineEdit->text())
                            .arg(m_manufacturerLineEdit->text())
                            .arg(sapParamId);

        db.transaction();

        if (query.exec(sqlString)) {
            if (!db.commit()){
                db.rollback();
            }

            emit dataChanged();
            close();
            return;

        } else {
            ErrorClassifier::instance()->showError(501);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501) + query.lastError().databaseText());
        }
    }
}

void KsuTypesDialogMapper::selectCurrentKsuType()
{
    if (m_ksuTypeId == -1)
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM KsuTypes WHERE id = %1").arg(m_ksuTypeId);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect() || !query.first())
        return;

    m_operationIsUpdate = true;
    m_codeSpinBox->setMinimum(1);

    m_codeSpinBox->setValue(query.value(QStringLiteral("code")).toInt());
    m_ksuNameLineEdit->setText(query.value(QStringLiteral("ksuName")).toString());
    m_manufacturerLineEdit->setText(query.value(QStringLiteral("manufacturer")).toString());

    bool ok;
    int sapParamId = query.value(QStringLiteral("sapParamId")).toInt(&ok);

    if (ok) {
        setCurrentSAPParametrsType(sapParamId);
    }

    m_codeSpinBox->setEnabled(false);
    m_sapParametrsComboBox->setEnabled(false);
}

void KsuTypesDialogMapper::fillSAPParametrsTypes()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT id, name FROM SAPParametrs ORDER BY code ASC");
    QSqlQuery query(sqlString, db);

    m_sapParametrsModel->setQuery(query);
}

void KsuTypesDialogMapper::setCurrentSAPParametrsType(int sapParamId)
{
    if (sapParamId == -1) {
        m_sapParametrsComboBox->setCurrentIndex(-1);
        return;
    }

    int rowCount = m_sapParametrsModel->rowCount();
    for (int i = 0; i < rowCount; i++) {
        QSqlRecord record = m_sapParametrsModel->record(i);
        bool ok;
        int id = record.value(QStringLiteral("id")).toInt(&ok);

        if (ok && sapParamId == id) {
            m_sapParametrsComboBox->setCurrentIndex(i);
            break;
        }
    }

}

bool KsuTypesDialogMapper::checkUnfilledFields()
{
    if (m_ksuNameLineEdit->text().trimmed().isEmpty()) {
        ErrorClassifier::instance()->showError(23);
        return false;
    }

    if (m_sapParametrsComboBox->currentIndex() == -1) {
        ErrorClassifier::instance()->showError(2);
        return false;
    }

    return true;
}
