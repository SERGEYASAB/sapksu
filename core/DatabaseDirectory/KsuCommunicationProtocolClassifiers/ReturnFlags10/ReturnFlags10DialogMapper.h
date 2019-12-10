#ifndef RETURNFLAGS10DIALOGMAPPER_H
#define RETURNFLAGS10DIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class ReturnFlags10DialogMapper : public QWidget
{
    Q_OBJECT

public:
    explicit ReturnFlags10DialogMapper(int comProtocolCode, QAbstractItemModel *model, QWidget *parent = Q_NULLPTR);

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    void save();

private:
    QLabel *m_code = nullptr;
    QLabel *m_name = nullptr;
    QLabel *m_format = nullptr;
    QLabel *m_alias = nullptr;

    QLineEdit *m_codeLineEdit = nullptr;
    QLineEdit *m_nameLineEdit = nullptr;
    QComboBox *m_formatComboBox = nullptr;
    QLineEdit *m_aliasLineEdit = nullptr;

    QPushButton *m_saveButton = nullptr;
    QPushButton *m_cancelButton = nullptr;

    QDataWidgetMapper *m_mapper = nullptr;

signals:
    void dataChanged();

private:
    int m_comProtocolCode;
};

#endif // RETURNFLAGS10DIALOGMAPPER_H
