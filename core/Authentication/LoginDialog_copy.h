#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "Common/PrefixHeader.h"
#include "Logger/Logger2/LoggerTaskManager2.h"

class LoginDialog_copy : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSqlQueryModel* userModel READ userModel WRITE setuserModel NOTIFY userModelChanged)
    Q_PROPERTY(bool enablePasswordChange READ enablePasswordChange WRITE setEnablePasswordChange NOTIFY enablePasswordChanged)
    Q_PROPERTY(bool passwordChangeErr READ passwordChangeErr WRITE setPasswordChangeErr NOTIFY passwordChangeErrChanged)
    Q_PROPERTY(bool error READ error WRITE setError NOTIFY errorChanged)
    Q_PROPERTY(QString logRecord READ getLogRecord WRITE setLogRecord NOTIFY logged)
public:
    explicit LoginDialog_copy(QObject *parent = nullptr);
    static LoginDialog_copy *instance();
    QSqlQueryModel* userModel();
    void setuserModel(QSqlQueryModel* userModel);

    bool enablePasswordChange();
    void setEnablePasswordChange(bool enablePasswordChange);

    bool passwordChangeErr();
    void setPasswordChangeErr(bool passwordChangeErr);

    bool error();
    void setError(bool error);

    Q_INVOKABLE QString getLogRecord();
    Q_INVOKABLE int getUserId(int i);
    Q_INVOKABLE QString getUserName(int i);
    Q_INVOKABLE int getAccessLevel(int i);
    Q_INVOKABLE QString getDBPassword(int i);
    Q_INVOKABLE QString getDescription(int i);

    Q_INVOKABLE bool authorization(QString password, int index);
    Q_INVOKABLE void cancel();
    Q_INVOKABLE void updatePassword(QString password, QString newPassword, QString confirmNewPassword,int index);
    Q_INVOKABLE void openReports();
    Q_INVOKABLE void openUsers();
    Q_INVOKABLE void openTestsManager();
    Q_INVOKABLE void openTestRedactor();
    Q_INVOKABLE void setQuitOnLastWindowClosed(bool parameter);
    Q_INVOKABLE void changeTheme(int theme);
    Q_INVOKABLE int getTheme();
    Q_INVOKABLE void openExecutorWindow();
    Q_INVOKABLE void closeExecutorWindow();
    Q_INVOKABLE bool getPasswordCorrect();
    Q_INVOKABLE void changePasswordCorrect(bool passwordCorrect);
    Q_INVOKABLE QString getAccess();
    Q_INVOKABLE void changeAccess(QString access);
    Q_INVOKABLE QString getOpenedWindow();
    Q_INVOKABLE void changeOpenedWindow(QString openwindow);
   // Q_INVOKABLE bool
    //void updatePassword();
    void setLogRecord(const QString &logRecord);

private:

    //    QVBoxLayout *m_mainVerticalLayout;

//    QLabel *m_nameLabel = nullptr;
//    QComboBox *m_nameComboBox = nullptr;

//    QLabel *m_passwordLabel = nullptr;
//    QLineEdit *m_passwordLineEdit = nullptr;

//    QLabel *m_newPasswordLabel = nullptr;
//    QLineEdit *m_newPasswordLineEdit = nullptr;

//    QLabel *m_confirmNewPasswordLabel = nullptr;
//    QLineEdit *m_confirmNewPasswordLineEdit = nullptr;

    QSqlQueryModel *m_userModel = nullptr;
    bool m_passwordChangeErr;
    bool m_error;
    bool m_enablePasswordChange = false;
    int m_theme;
    QString m_logRecord = nullptr;

signals:
     void userModelChanged();
     void enablePasswordChanged();
     void passwordCorrectChanged();
     void passwordChangeErrChanged();
     void errorChanged();
     void logged();
private:
     //    void showMessage(const QString &title,
     //                     const QString &text,
     //                     const QString &informativeText = QString(),
     //                     QMessageBox::Icon icon = QMessageBox::Warning) const;

     //    bool checkUnfilledFields();
     QHash<LoggerTaskManager2::MessageType, QTextCharFormat> m_lineFormats;
     void setBindings();
public slots:
     void selectData();

private slots:
     void setupStylesheet(int stylesheet);

     //    void save();
     //    void cancel();
     //    void authorization();
     //    //void updatePassword();
     //    void clearUserInputs();
     //    void updateUserIconAccessLevel();
     //    void setSystemAccessLevel (int accessLevel);

     // QWidget interface
protected slots:
     /*
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;*/
};

#endif // LOGINDIALOG_H
