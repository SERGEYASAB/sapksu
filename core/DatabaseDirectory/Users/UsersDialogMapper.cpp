#include "UsersDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

UsersDialogMapper::UsersDialogMapper(QSqlQueryModel *model, bool disablePasswordInput, QWidget *parent)
    : QWidget(parent), m_model(model)
{
    m_accessLevelsModel = new QSqlQueryModel(this);

    fillAccessLevelsTypes();
    selectCurrentUser();

    m_nameLabel = new QLabel(tr("&Имя пользователя"));
    m_nameLineEdit = new QLineEdit();
    m_nameLabel->setBuddy(m_nameLineEdit);

    m_accessLevelLebel = new QLabel(tr("&Уровень доступа"));
    m_accessLevelComboBox = new QComboBox();
    m_accessLevelComboBox->setModel(m_accessLevelsModel);
    m_accessLevelComboBox->setModelColumn(1);
    m_accessLevelComboBox->setCurrentIndex(-1);
    m_accessLevelLebel->setBuddy(m_accessLevelComboBox);


    m_passwordLabel = new QLabel(tr("&Задайте пароль"));
    m_passwordLabel->setVisible(!disablePasswordInput);
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
    m_passwordLineEdit->setVisible(!disablePasswordInput);
    m_passwordLabel->setBuddy(m_passwordLineEdit);

    m_confirmPasswordLabel = new QLabel(tr("&Подтвердите пароль"));
    m_confirmPasswordLabel->setVisible(!disablePasswordInput);
    m_confirmPasswordLineEdit = new QLineEdit();
    m_confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordLineEdit->setVisible(!disablePasswordInput);
    m_confirmPasswordLabel->setBuddy(m_confirmPasswordLineEdit);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_nameLabel, m_nameLineEdit);
    formLayout->addRow(m_accessLevelLebel, m_accessLevelComboBox);
    formLayout->addRow(m_passwordLabel, m_passwordLineEdit);
    formLayout->addRow(m_confirmPasswordLabel, m_confirmPasswordLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &UsersDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &UsersDialogMapper::close);

    vlayout->addLayout(formLayout);
    vlayout->addWidget(buttonBox);

    setLayout(vlayout);

    setFixedSize(QSize(400, 150));

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Редактор параметров"));
}

void UsersDialogMapper::setUserId(int ksuTypeId)
{
    m_userId = ksuTypeId;
    selectCurrentUser();
}

void UsersDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    if (!checkUnfilledFields())
        return;

    bool ok;
    int accessId = m_accessLevelsModel->index(m_accessLevelComboBox->currentIndex(), 0).data().toInt(&ok);
    if (!ok)
        return;

    if (m_operationIsUpdate) {

        QString sqlString = QString("UPDATE `Users` "
                                    "SET name = '%1', accessId = '%2' "
                                    "WHERE id = %3;")
                .arg(m_nameLineEdit->text())
                .arg(accessId)
                .arg(m_userId);

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

        QString password = QString(QCryptographicHash::hash((m_passwordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());

        QString sqlString = QString("INSERT INTO `Users` "
                                    "("
                                    "`name`, "
                                    "`accessId`, "
                                    "`password`"
                                    ") "
                                    "VALUES ('%1', %2, '%3');")
                .arg(m_nameLineEdit->text())
                .arg(accessId)
                .arg(password);


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

void UsersDialogMapper::selectCurrentUser()
{
    if (m_userId == -1)
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM Users WHERE id = %1").arg(m_userId);
    QSqlQuery query(sqlString, db);

    if (!query.isActive() || !query.isSelect() || !query.first())
        return;

    m_operationIsUpdate = true;

    m_nameLineEdit->setText(query.value(QStringLiteral("name")).toString());

    bool ok;
    int access = query.value(QStringLiteral("accessId")).toInt(&ok);

    if (ok) {
        setCurrentAccessLevelType(access);
    }
}

void UsersDialogMapper::fillAccessLevelsTypes()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT id, name FROM AccessLevels ORDER BY name ASC");
    QSqlQuery query(sqlString, db);

    m_accessLevelsModel->setQuery(query);
}

void UsersDialogMapper::setCurrentAccessLevelType(int accessId)
{
    if (accessId == -1) {
        m_accessLevelComboBox->setCurrentIndex(-1);
        return;
    }

    int rowCount = m_accessLevelsModel->rowCount();
    for (int i = 0; i < rowCount; i++) {
        QSqlRecord record = m_accessLevelsModel->record(i);
        bool ok;
        int id = record.value(QStringLiteral("id")).toInt(&ok);

        if (ok && accessId == id) {
            m_accessLevelComboBox->setCurrentIndex(i);
            break;
        }
    }

}

bool UsersDialogMapper::checkUnfilledFields()
{
    if (m_nameLineEdit->text().trimmed().isEmpty()) {
        ErrorClassifier::instance()->showError(30);
        return false;
    }

    if (m_accessLevelComboBox->currentIndex() == -1) {
        ErrorClassifier::instance()->showError(31);
        return false;
    }

    if (m_passwordLineEdit->text() != m_confirmPasswordLineEdit->text()) {
        ErrorClassifier::instance()->showError(32);
        return false;
    }

    return true;
}
