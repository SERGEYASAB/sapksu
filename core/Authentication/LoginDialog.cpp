#include "LoginDialog.h"
#include "DBConfig/DBConfig.h"
#include "SystemUser.h"
#include "StartWindow/StartWindow.h"
#include "ReportManager/OperatorClickReport.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

LoginDialog::LoginDialog(QWidget *parent) : QWidget(parent)
{
    m_userModel = new QSqlQueryModel(this);

    selectData();

    const QIcon adminIcon = QIcon(":/Images/access_robot.png");
    const QIcon engineerIcon = QIcon(":/Images/access_electrical_engineer.png");
    const QIcon operatorIcon = QIcon(":/Images/access_graduated.png");
    const QIcon userIcon = QIcon(":/Images/access_user2.png");

    setFocusPolicy(Qt::StrongFocus);

    // Настройка иконки уровня доступа

    m_adminIconLabel = new QLabel(this);
    m_adminIconLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_adminIconLabel->setPixmap(QPixmap(":/Images/access_robot.png"));
    m_adminIconLabel->setStyleSheet("border:2px solid grey; border-radius: 5px;background-color: transparent;");
    m_adminIconLabel->setVisible(false);

    m_engineerIconLabel = new QLabel(this);
    m_engineerIconLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_engineerIconLabel->setPixmap(QPixmap(":/Images/access_electrical_engineer.png"));
    m_engineerIconLabel->setStyleSheet("border:2px solid grey; border-radius: 5px;background-color: transparent;");
    m_engineerIconLabel->setVisible(false);

    m_operatorIconLabel = new QLabel(this);
    m_operatorIconLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_operatorIconLabel->setPixmap(QPixmap(":/Images/access_graduated.png"));
    m_operatorIconLabel->setStyleSheet("border:2px solid grey; border-radius: 5px;background-color: transparent;");
    m_operatorIconLabel->setVisible(false);

    m_userIconLabel = new QLabel(this);
    m_userIconLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_userIconLabel->setPixmap(QPixmap(":/Images/access_user2.png"));
    m_userIconLabel->setStyleSheet("border:2px solid grey; border-radius: 5px;background-color: transparent;");
    m_userIconLabel->setVisible(false);

    // Ввод пользователя

    m_nameLabel = new QLabel(tr("&Имя пользователя"));
    m_nameComboBox = new QComboBox();
    m_nameComboBox->setModel(m_userModel);
    m_nameComboBox->setModelColumn(1);
    m_nameComboBox->setCurrentText(SystemUser::instance()->user());
    m_nameLabel->setBuddy(m_nameComboBox);

    connect(m_nameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LoginDialog::clearUserInputs);
    connect(m_nameComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &LoginDialog::updateUserIconAccessLevel);

    // Ввод пароля

    m_passwordLabel = new QLabel(tr("&Текущий пароль"));
    m_passwordLineEdit = new QLineEdit();
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);
#ifdef QT_DEBUG
    m_passwordLineEdit->setText("admin");
#endif
    m_passwordLabel->setBuddy(m_passwordLineEdit);

    m_newPasswordLabel = new QLabel(tr("&Новый пароль"));
    m_newPasswordLabel->setVisible(m_enablePasswordChange);
    m_newPasswordLineEdit = new QLineEdit();
    m_newPasswordLineEdit->setVisible(m_enablePasswordChange);
    m_newPasswordLineEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordLabel->setBuddy(m_newPasswordLineEdit);

    m_confirmNewPasswordLabel = new QLabel(tr("&Повторить пароль"));
    m_confirmNewPasswordLabel->setVisible(m_enablePasswordChange);
    m_confirmNewPasswordLineEdit = new QLineEdit();
    m_confirmNewPasswordLineEdit->setVisible(m_enablePasswordChange);
    m_confirmNewPasswordLineEdit->setEchoMode(QLineEdit::Password);
    m_confirmNewPasswordLabel->setBuddy(m_confirmNewPasswordLineEdit);

    // Кнопки
    m_okButton = new QPushButton(tr("Ок"));
    m_okButton->setDefault(true);
    m_okButton->setAutoDefault(true);
    m_okButton->setShortcut(Qt::Key_Return);
    m_cancelButton = new QPushButton(tr("Отмена"));
    m_changePassowrButton = new QPushButton(tr("&Изменить пароль"));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);
    buttonBox->addButton(m_okButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);
    buttonBox->addButton(m_changePassowrButton, QDialogButtonBox::HelpRole);
    connect(m_okButton, &QPushButton::clicked, this, &LoginDialog::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &LoginDialog::cancel);
    connect(m_changePassowrButton, &QPushButton::clicked, [this]()
    {
        //OperatorClickReport::instance()->write("Окно авторизации пользователя. Нажата клавиша ИЗМЕНИТЬ ПАРОЛЬ");

        m_enablePasswordChange = true;
        m_newPasswordLabel->setVisible(m_enablePasswordChange);
        m_newPasswordLineEdit->setVisible(m_enablePasswordChange);
        m_confirmNewPasswordLabel->setVisible(m_enablePasswordChange);
        m_confirmNewPasswordLineEdit->setVisible(m_enablePasswordChange);
        m_changePassowrButton->setVisible(!m_enablePasswordChange);

    });

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_nameLabel, m_nameComboBox);
    formLayout->addRow(m_passwordLabel, m_passwordLineEdit);
    formLayout->addRow(m_newPasswordLabel, m_newPasswordLineEdit);
    formLayout->addRow(m_confirmNewPasswordLabel, m_confirmNewPasswordLineEdit);

    m_mainVerticalLayout = new QVBoxLayout;
    m_mainVerticalLayout->addWidget(new QLabel(tr("Уровень доступа пользователя")), 0, Qt::AlignCenter);
    m_mainVerticalLayout->addSpacing(20);
    m_mainVerticalLayout->addWidget(m_userIconLabel, 0, Qt::AlignCenter);
    m_mainVerticalLayout->addWidget(m_operatorIconLabel, 0, Qt::AlignCenter);
    m_mainVerticalLayout->addWidget(m_engineerIconLabel, 0, Qt::AlignCenter);
    m_mainVerticalLayout->addWidget(m_adminIconLabel, 0, Qt::AlignCenter);
    m_mainVerticalLayout->addSpacing(20);
    m_mainVerticalLayout->addLayout(formLayout);
    m_mainVerticalLayout->addSpacing(20);
    m_mainVerticalLayout->addStretch();
    m_mainVerticalLayout->addWidget(buttonBox);

    updateUserIconAccessLevel();

    setLayout(m_mainVerticalLayout);
}

void LoginDialog::showMessage(const QString &title, const QString &text, const QString &informativeText, QMessageBox::Icon icon) const
{
    QMessageBox msgBox;
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setInformativeText(informativeText);

    QSpacerItem *horizontalSpacer = new QSpacerItem(400, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(400);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.exec();
}

bool LoginDialog::checkUnfilledFields()
{
    if (m_nameComboBox->currentIndex() == -1) {
        ErrorClassifier::instance()->showError(1);
        return false;
    }

    return true;
}

void LoginDialog::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    // Изменять запрос нельзя, т.к. в данном классе все завязано на порядке следования столбцов!

    QString sqlString = QString("SELECT Users.*, AccessLevels.name, AccessLevels.access"
                                "   FROM `Users` "
                                "   INNER JOIN AccessLevels "
                                "       ON Users.accessId = AccessLevels.id "
                                "ORDER BY `name` ASC");

    QSqlQuery query(sqlString, db);

    m_userModel->setQuery(query);
}

void LoginDialog::save()
{
    //OperatorClickReport::instance()->write("Окно авторизации пользователя. Нажата клавиша ОК");

    if (!checkUnfilledFields())
        return;

    switch (m_enablePasswordChange) {
        case true:
            updatePassword();
            break;
        case false:
            authorization();
            break;
    }
}

void LoginDialog::cancel()
{
    //OperatorClickReport::instance()->write("Окно авторизации пользователя. Нажата клавиша Отмена");

    switch (m_enablePasswordChange) {
        case true:
            clearUserInputs();
            break;
        case false: {

            SystemUser::instance()->setUser("Гость");
            SystemUser::instance()->setAccess(ACCESSLEVEL_USER);

            StartWindow *startWindow = new StartWindow;
            startWindow->setAttribute(Qt::WA_DeleteOnClose);
            startWindow->setWindowModality(Qt::NonModal);

            startWindow->setWindowTitle(titleString);

            startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
            startWindow->resize(QSize(1024, 768));
            startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
            startWindow->show();
            startWindow->activateWindow();

            close();

        }
    }
}

void LoginDialog::authorization()
{
    QString lineEditPassword = QString(QCryptographicHash::hash((m_passwordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
    QString databasePassword = m_userModel->index(m_nameComboBox->currentIndex(), 3).data().toString();

    if (lineEditPassword != databasePassword) {
        ErrorClassifier::instance()->showError(2);
        return;
    }

    setSystemAccessLevel(m_userModel->index(m_nameComboBox->currentIndex(), 5).data().toInt());

    StartWindow *startWindow = new StartWindow;
    startWindow->setAttribute(Qt::WA_DeleteOnClose);
    startWindow->setWindowModality(Qt::NonModal);

    startWindow->setWindowTitle(titleString);

    startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    startWindow->resize(QSize(1024, 768));
    startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
    startWindow->show();
    startWindow->activateWindow();

    close();
}

void LoginDialog::updatePassword()
{
    QString lineEditPassword = QString(QCryptographicHash::hash((m_passwordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());
    QString databasePassword = m_userModel->index(m_nameComboBox->currentIndex(), 3).data().toString();

    if (lineEditPassword != databasePassword) {
        ErrorClassifier::instance()->showError(2);
        return;
    }

    if (m_newPasswordLineEdit->text() != m_confirmNewPasswordLineEdit->text()) {
        ErrorClassifier::instance()->showError(3);
        return;
    }

    if (m_newPasswordLineEdit->text().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Ввод данных");
        QString msgString = tr("Вы действительно хотите оставить поле пароль пустым?");
        msgBox.setText(msgString);
        msgBox.setIcon(QMessageBox::Warning);

        QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
        msgBox.setDefaultButton(noButton);

        msgBox.exec();

        if (msgBox.clickedButton() == yesButton) {
            OperatorClickReport::instance()->write("Окно авторизации пользователя. Вы действительно хотите оставить поле пароль пустым? Нажата клавиша ДА");
            msgBox.close();
        } else if (msgBox.clickedButton() == noButton) {
            OperatorClickReport::instance()->write("Окно авторизации пользователя. Вы действительно хотите оставить поле пароль пустым? Нажата клавиша НЕТ");
            return;
        }
    }

    int userId = m_userModel->index(m_nameComboBox->currentIndex(), 0).data().toInt();
    QString newPassword = QString(QCryptographicHash::hash((m_newPasswordLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex());

    QString sqlString = QString("UPDATE `Users` SET password = '%1' WHERE id = %2;")
                        .arg(newPassword)
                        .arg(userId);

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    db.transaction();

    if (query.exec(sqlString)) {

        if (!db.commit()){
            db.rollback();
        }

        showMessage(tr("Ввод данных"), tr("Пароль успешно изменен авторизуйтесь повторно"));
        m_nameComboBox->setCurrentIndex(-1);
        selectData();
        OperatorClickReport::instance()->write(QStringLiteral("Авторизация: изменение пароля"));
        return;

    }  else {
        ErrorClassifier::instance()->showError(501);
    }

}

void LoginDialog::clearUserInputs()
{
    m_passwordLineEdit->clear();
    m_newPasswordLineEdit->clear();
    m_confirmNewPasswordLineEdit->clear();

    m_enablePasswordChange = false;
    m_newPasswordLabel->setVisible(m_enablePasswordChange);
    m_newPasswordLineEdit->setVisible(m_enablePasswordChange);
    m_confirmNewPasswordLabel->setVisible(m_enablePasswordChange);
    m_confirmNewPasswordLineEdit->setVisible(m_enablePasswordChange);

    m_changePassowrButton->setVisible(!m_enablePasswordChange);
}

void LoginDialog::updateUserIconAccessLevel()
{
    int accessLevel = m_userModel->index(m_nameComboBox->currentIndex(), 5).data().toInt();

    switch (accessLevel) {

        case ACCESSLEVEL_ADMIN:
        {
            m_engineerIconLabel->setVisible(false);
            m_operatorIconLabel->setVisible(false);
            m_userIconLabel->setVisible(false);
            m_adminIconLabel->setVisible(true);
            break;
        }
        case ACCESSLEVEL_ENGINEER:
        {
            m_adminIconLabel->setVisible(false);
            m_operatorIconLabel->setVisible(false);
            m_userIconLabel->setVisible(false);
            m_engineerIconLabel->setVisible(true);
            break;
        }
        case ACCESSLEVEL_OPERATOR:
        {
            m_adminIconLabel->setVisible(false);
            m_engineerIconLabel->setVisible(false);
            m_userIconLabel->setVisible(false);
            m_operatorIconLabel->setVisible(true);
            break;
        }
        case ACCESSLEVEL_USER:
        {
            m_adminIconLabel->setVisible(false);
            m_engineerIconLabel->setVisible(false);
            m_operatorIconLabel->setVisible(false);
            m_userIconLabel->setVisible(true);
            break;
        }
        default:
        {
            m_adminIconLabel->setVisible(false);
            m_engineerIconLabel->setVisible(false);
            m_operatorIconLabel->setVisible(false);
            m_userIconLabel->setVisible(true);
            break;
        }
    }
}

void LoginDialog::setSystemAccessLevel(int accessLevel)
{
    SystemUser *sysUser = SystemUser::instance();
    sysUser->setUser(m_userModel->index(m_nameComboBox->currentIndex(), 1).data().toString());
    sysUser->setAccess(m_userModel->index(m_nameComboBox->currentIndex(), 5).data().toInt());
    sysUser->setDescription(m_userModel->index(m_nameComboBox->currentIndex(), 4).data().toString());
    OperatorClickReport::instance()->write(QString("Авторизация: вход пользователя %1").arg(sysUser->user()));
}

void LoginDialog::closeEvent(QCloseEvent *event)
{
    OperatorClickReport::instance()->write(QStringLiteral("Авторизация: закрытие окна"));

    QWidget::closeEvent(event);
}

void LoginDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_D && (event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier))) {
        m_nameComboBox->setCurrentText(tr("Администратор"));
        m_passwordLineEdit->setText("admin");
        authorization();
    }
    QWidget::keyPressEvent(event);
}
