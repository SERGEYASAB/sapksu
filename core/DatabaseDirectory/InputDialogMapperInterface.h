#ifndef INPUTDIALOGMAPPERINTERFACE_H
#define INPUTDIALOGMAPPERINTERFACE_H

#include "Common/PrefixHeader.h"

class InputDialogMapperInterface : public QWidget
{
    Q_OBJECT

public:
    explicit InputDialogMapperInterface(QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);
    virtual ~InputDialogMapperInterface();

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    virtual void save() = 0;

protected:
    QLabel *m_code;
    QLabel *m_description;

    QLineEdit *m_codeLineEdit;
    QLineEdit *m_descriptionLineEdit;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QDataWidgetMapper *m_mapper;
    QSqlQueryModel *m_model;

signals:
    void dataChanged();
};

#endif // INPUTDIALOGMAPPERINTERFACE_H
