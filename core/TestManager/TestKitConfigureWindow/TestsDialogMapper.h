#ifndef TESTSDIALOGMAPPER_H
#define TESTSDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class TestsDialogMapper : public QWidget
{
    Q_OBJECT

public:
    explicit TestsDialogMapper(QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    Q_INVOKABLE QString deleteMe() {qDebug() << "DeletMe is runing"; return "DeletMe";}

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    void save();
    void copyTemplate();

private:

    QLabel *m_id;
    QLabel *m_name;
    QLabel *m_description;
    QLabel *m_path;
    QLabel *m_buttonType;
    QLabel *m_protocolType;

    QLineEdit *m_nameLineEdit;
    QLineEdit *m_descriptionLineEdit;
    QLineEdit *m_pathLineEdit;
    QComboBox *m_buttonTypeComboBox;
    QComboBox *m_protocolTypeComboBox;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QDataWidgetMapper *m_mapper;

signals:
    void dataChanged();

};

#endif // TESTSDIALOGMAPPER_H
