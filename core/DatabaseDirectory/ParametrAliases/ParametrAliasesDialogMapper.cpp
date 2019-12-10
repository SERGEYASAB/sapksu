#include "ParametrAliasesDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

ParametrAliasesDialogMapper::ParametrAliasesDialogMapper(QSqlQueryModel *model, bool disableAlias, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    m_alias = new QLabel(tr("&Псевдоним параметра"));
    m_description = new QLabel(tr("&Описание"));

    m_aliasLineEdit = new QLineEdit();
    m_aliasLineEdit->setDisabled(disableAlias);

    QRegExp regExp("[0-9a-zA-Z_]+");
    QValidator *validator = new QRegExpValidator(regExp, this);
    m_aliasLineEdit->setValidator(validator);

    m_descriptionLineEdit = new QLineEdit();

    m_alias->setBuddy(m_aliasLineEdit);
    m_description->setBuddy(m_descriptionLineEdit);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_aliasLineEdit, 1);
    m_mapper->addMapping(m_descriptionLineEdit, 2);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_alias, m_aliasLineEdit);
    formLayout->addRow(m_description, m_descriptionLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &ParametrAliasesDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &ParametrAliasesDialogMapper::close);

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

void ParametrAliasesDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}

void ParametrAliasesDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    if (!checkUnfilledFields())
        return;

    if (m_mapper->currentIndex() == -1) {

        QString sqlString = QString("INSERT INTO `ParametrAliases` (`alias`, `description`)"
                                    "VALUES ('%1', '%2');")
                .arg(m_aliasLineEdit->text())
                .arg(m_descriptionLineEdit->text());

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

        QString sqlString = QString("UPDATE `ParametrAliases`"
                                    "SET alias = '%1', description = '%2'"
                                    "WHERE id = %3;")
                .arg(m_aliasLineEdit->text())
                .arg(m_descriptionLineEdit->text())
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

bool ParametrAliasesDialogMapper::checkUnfilledFields()
{
    if (m_aliasLineEdit->text().trimmed().isEmpty()) {
        ErrorClassifier::instance()->showError(25);
        return false;
    }

    //    if (m_sapParametrsComboBox->currentIndex() == -1) {
    //        showMessage(tr("Ввод данных"), tr("Необходимо выбрать параметры САП."));
    //        return false;
    //    }

    return true;
}
