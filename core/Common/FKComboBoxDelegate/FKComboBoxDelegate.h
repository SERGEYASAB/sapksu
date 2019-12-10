#ifndef FKCOMBOBOXDELEGATE_H
#define FKCOMBOBOXDELEGATE_H

#include "Common/PrefixHeader.h"

class FKComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    FKComboBoxDelegate(QObject *parent = Q_NULLPTR) :
        QStyledItemDelegate(parent), m_section(-1) {}
    FKComboBoxDelegate(int section = 0, QObject *parent = Q_NULLPTR) :
        QStyledItemDelegate(parent), m_section(section) {}
    FKComboBoxDelegate(const QMap<int, QString> &keyValueMap, QObject *parent = Q_NULLPTR) :
        QStyledItemDelegate(parent), m_keyValueMap(keyValueMap), m_section(-1) {}
    FKComboBoxDelegate(QMap<int, QString> &&keyValueMap, QObject *parent = Q_NULLPTR) :
        QStyledItemDelegate(parent), m_keyValueMap(keyValueMap), m_section(-1) {}
    FKComboBoxDelegate(const QMap<int, QString> &keyValueMap, int section = -1, QObject *parent = Q_NULLPTR) :
        QStyledItemDelegate(parent), m_keyValueMap(keyValueMap), m_section(section) {}
    FKComboBoxDelegate(QMap<int, QString> &&keyValueMap, int section = -1, QObject *parent = Q_NULLPTR) :
        QStyledItemDelegate(parent), m_keyValueMap(keyValueMap), m_section(section) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    const QMap<int, QString> &keyValueMap() const;
    void setKeyValueMap(const QMap<int, QString> &keyValueMap);

    int section() const;
    void setSection(int section);

private:
    QMap<int, QString> m_keyValueMap;
    int m_section;
};

#endif // FKCOMBOBOXDELEGATE_H
