#ifndef KSUTYPESDIALOGMAPPER_H
#define KSUTYPESDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class KsuTypesDialogMapper : public QWidget
{
    Q_OBJECT

public:
    KsuTypesDialogMapper(QSqlQueryModel *model, QWidget *parent = Q_NULLPTR);

    void setKsuTypeId(int ksuTypeId);

signals:
    void dataChanged();

private slots:
    void save();

private:
    void selectCurrentKsuType();
    void fillSAPParametrsTypes();
    void setCurrentSAPParametrsType(int sapParamId);
    bool checkUnfilledFields();

private:
    QSqlQueryModel *m_model;

    QLabel *m_codeLabel = nullptr;
    QSpinBox *m_codeSpinBox = nullptr;

    QLabel *m_ksuNameLabel = nullptr;
    QLineEdit *m_ksuNameLineEdit = nullptr;

    QLabel *m_manufacturerLabel = nullptr;
    QLineEdit *m_manufacturerLineEdit = nullptr;

    QLabel *m_sapParametrsLabel = nullptr;
    QComboBox *m_sapParametrsComboBox = nullptr;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QSqlQueryModel *m_sapParametrsModel = nullptr;

    int m_ksuTypeId = -1;
    bool m_operationIsUpdate = false;

};

#endif // KSUTYPESDIALOGMAPPER_H
