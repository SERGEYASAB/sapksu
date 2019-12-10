#ifndef SAPPARAMETRSDIALOGMAPPER_H
#define SAPPARAMETRSDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class SAPParametrsDialogMapper : public QWidget
{
    Q_OBJECT

public:
    explicit SAPParametrsDialogMapper(QSqlQueryModel *model, bool disableType = false, QWidget *parent = Q_NULLPTR);

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    void save();
    bool copyTemplate(const QString &filePath);

private:
    void showQueryError(const QSqlQuery &query);

private:
    QLabel *m_code = nullptr;
    QLabel *m_name = nullptr;
    QLabel *m_description = nullptr;
    QLabel *m_parametsConfigPath = nullptr;

    QSpinBox *m_codeSpinBox = nullptr;
    QLineEdit *m_nameLineEdit = nullptr;
    QLineEdit *m_descriptionLineEdit = nullptr;
    QLineEdit *m_parametsConfigPathLineEdit = nullptr;

    QPushButton *m_saveButton = nullptr;
    QPushButton *m_cancelButton = nullptr;

    QDataWidgetMapper *m_mapper = nullptr;
    QSqlQueryModel *m_model = nullptr;

signals:
    void dataChanged();
};

#endif // SAPPARAMETRSDIALOGMAPPER_H
