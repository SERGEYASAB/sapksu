#ifndef PARAMETRTEMPLATESDIALOGMAPPER_H
#define PARAMETRTEMPLATESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class ParametrTemplatesDialogMapper : public QWidget
{
    Q_OBJECT

public:
    explicit ParametrTemplatesDialogMapper(QSqlQueryModel *model, bool disableType = false, QWidget *parent = Q_NULLPTR);

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    void save();
    bool copyTemplate(SapKsu::KSUProtocolType type, const QString &filePath);

private: //methods
    void showQueryError(const QSqlQuery &query);

private:
    QLabel *m_code = nullptr;
    QLabel *m_name = nullptr;
    QLabel *m_protocolType = nullptr;
    QLabel *m_parametsConfigPath = nullptr;

    QSpinBox *m_codeSpinBox = nullptr;
    QLineEdit *m_nameLineEdit = nullptr;
    QComboBox *m_protocolTypeComboBox = nullptr;
    QLineEdit *m_parametsConfigPathLineEdit = nullptr;

    QPushButton *m_saveButton = nullptr;
    QPushButton *m_cancelButton = nullptr;

    QDataWidgetMapper *m_mapper = nullptr;
    QSqlQueryModel *m_model = nullptr;

signals:
    void dataChanged();
};

#endif // PARAMETRTEMPLATESDIALOGMAPPER_H
