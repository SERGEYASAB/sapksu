#ifndef PARAMETRALIASESDIALOGMAPPER_H
#define PARAMETRALIASESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class ParametrAliasesDialogMapper: public QWidget
{
    Q_OBJECT

public:
    ParametrAliasesDialogMapper(QSqlQueryModel *model, bool disableAlias = false, QWidget *parent = Q_NULLPTR);

    void setCurrentModelIndex(const QModelIndex &index);

signals:
    void dataChanged();

private slots:
    void save();
    bool checkUnfilledFields();

private:
    QSqlQueryModel *m_model;

    QLabel *m_alias;
    QLabel *m_description;

    QLineEdit *m_aliasLineEdit;
    QLineEdit *m_descriptionLineEdit;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QDataWidgetMapper *m_mapper;
};

#endif // PARAMETRALIASESDIALOGMAPPER_H
