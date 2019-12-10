#ifndef USERSDIALOGMAPPER_H
#define USERSDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class UsersDialogMapper : public QWidget
{
    Q_OBJECT

public:
    UsersDialogMapper(QSqlQueryModel *model, bool disablePasswordInput = false, QWidget *parent = Q_NULLPTR);

    void setUserId(int userId);

signals:
    void dataChanged();

private slots:
    void save();

private:
    void selectCurrentUser();
    void fillAccessLevelsTypes();
    void setCurrentAccessLevelType(int accessId);
    bool checkUnfilledFields();

private:
    QSqlQueryModel *m_model;

    QLabel *m_nameLabel = nullptr;
    QLineEdit *m_nameLineEdit = nullptr;

    QLabel *m_accessLevelLebel = nullptr;
    QComboBox *m_accessLevelComboBox = nullptr;

    QLabel *m_passwordLabel = nullptr;
    QLineEdit *m_passwordLineEdit = nullptr;

    QLabel *m_confirmPasswordLabel = nullptr;
    QLineEdit *m_confirmPasswordLineEdit = nullptr;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QSqlQueryModel *m_accessLevelsModel = nullptr;

    int m_userId = -1;
    bool m_operationIsUpdate = false;

};


#endif // USERSDIALOGMAPPER_H
