#include "SUTypesDialogMapper.h"
#include "DBConfig/DBConfig.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

SUTypesDialogMapper::SUTypesDialogMapper(QSqlQueryModel *model, bool disableAlias, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    m_codeLabel = new QLabel(tr("Код"));
    m_SUNameLabel = new QLabel(tr("Название СУ"));
    m_manufacturerLabel = new QLabel(tr("Производитель СУ"));

    m_codeSpinBox = new QSpinBox();
    m_codeSpinBox->setRange(1, std::numeric_limits<int>::max());

    m_SUNameLineEdit = new QLineEdit();
    m_manufacturerLineEdit = new QLineEdit();

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_codeSpinBox, 1);
    m_mapper->addMapping(m_SUNameLineEdit, 2);
    m_mapper->addMapping(m_manufacturerLineEdit, 3);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_codeLabel, m_codeSpinBox);
    formLayout->addRow(m_SUNameLabel, m_SUNameLineEdit);
    formLayout->addRow(m_manufacturerLabel, m_manufacturerLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &SUTypesDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &SUTypesDialogMapper::close);

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

void SUTypesDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}

void SUTypesDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    if (!checkUnfilledFields())
        return;

    if (m_mapper->currentIndex() == -1) {

        QString sqlString = QString("INSERT INTO `SUTypes` (`code`, `SUName`, `manufacturer`)"
                                    "VALUES ('%1', '%2', '%3');")
                            .arg(m_codeSpinBox->value())
                            .arg(m_SUNameLineEdit->text())
                            .arg(m_manufacturerLineEdit->text());

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

        QString sqlString = QString("UPDATE `SUTypes`"
                                    "SET code = '%1', SUName = '%2', manufacturer = '%3'"
                                    "WHERE id = %4;")
                            .arg(m_codeSpinBox->value())
                            .arg(m_SUNameLineEdit->text())
                            .arg(m_manufacturerLineEdit->text())
                            .arg(m_model->record(m_mapper->currentIndex()).value("id").toString());

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

bool SUTypesDialogMapper::checkUnfilledFields()
{
    if (m_SUNameLabel->text().trimmed().isEmpty()) {
        ErrorClassifier::instance()->showError(29);
        return false;
    }

    return true;
}
