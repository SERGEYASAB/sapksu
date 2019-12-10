#ifndef TESTKITDIALOGMAPPER_H
#define TESTKITDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class TestKitDialogMapper : public QWidget
{
    Q_OBJECT

public:
    explicit TestKitDialogMapper(QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    void save();

private:

    QLabel *m_id;
    QLabel *m_name;
    QLabel *m_description;
    QLabel *m_buttonType;
    QLabel *m_protocolType;

    QLineEdit *m_nameLineEdit;
    QLineEdit *m_descriptionLineEdit;
    QComboBox *m_buttonTypeComboBox;
    QComboBox *m_protocolTypeComboBox;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QDataWidgetMapper *m_mapper;

signals:
    void dataChanged();
};

#endif // TESTKITDIALOGMAPPER_H
