#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "Common/PrefixHeader.h"

class LoginDialog : public QWidget
{

    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

private:

    QVBoxLayout *m_mainVerticalLayout;

    QLabel *m_adminIconLabel = nullptr;
    QLabel *m_engineerIconLabel = nullptr;
    QLabel *m_operatorIconLabel = nullptr;
    QLabel *m_userIconLabel = nullptr;

    QLabel *m_nameLabel = nullptr;
    QComboBox *m_nameComboBox = nullptr;

    QLabel *m_passwordLabel = nullptr;
    QLineEdit *m_passwordLineEdit = nullptr;

    QLabel *m_newPasswordLabel = nullptr;
    QLineEdit *m_newPasswordLineEdit = nullptr;

    QLabel *m_confirmNewPasswordLabel = nullptr;
    QLineEdit *m_confirmNewPasswordLineEdit = nullptr;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_changePassowrButton;

    QSqlQueryModel *m_userModel = nullptr;

    bool m_enablePasswordChange = false;

private:
    void showMessage(const QString &title,
                     const QString &text,
                     const QString &informativeText = QString(),
                     QMessageBox::Icon icon = QMessageBox::Warning) const;

    bool checkUnfilledFields();

public slots:
    void selectData();

private slots:
    void save();
    void cancel();
    void authorization();
    void updatePassword();
    void clearUserInputs();
    void updateUserIconAccessLevel();
    void setSystemAccessLevel (int accessLevel);

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // LOGINDIALOG_H
