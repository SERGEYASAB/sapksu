#ifndef SUTYPESDIALOGMAPPER_H
#define SUTYPESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class SUTypesDialogMapper: public QWidget
{
    Q_OBJECT

public:
    SUTypesDialogMapper(QSqlQueryModel *model, bool disableAlias = false, QWidget *parent = Q_NULLPTR);

    void setCurrentModelIndex(const QModelIndex &index);

signals:
    void dataChanged();

private slots:
    void save();
    bool checkUnfilledFields();

private:
    QSqlQueryModel *m_model;

    QLabel *m_codeLabel = nullptr;
    QSpinBox *m_codeSpinBox = nullptr;

    QLabel *m_SUNameLabel = nullptr;
    QLineEdit *m_SUNameLineEdit = nullptr;

    QLabel *m_manufacturerLabel = nullptr;
    QLineEdit *m_manufacturerLineEdit = nullptr;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QDataWidgetMapper *m_mapper;
};

#endif // SUTYPESDIALOGMAPPER_H
