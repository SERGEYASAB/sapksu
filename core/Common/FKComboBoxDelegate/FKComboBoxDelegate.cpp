#include "FKComboBoxDelegate.h"

void FKComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool ok = true;
    QString displayText = option.text;
    int key = index.data(Qt::EditRole).toInt(&ok);
    if ((index.column() == m_section || m_section == -1) && ok) {
        if (m_keyValueMap.contains(key)) {
            displayText = m_keyValueMap.value(key);
        }
    }
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = displayText;

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);
}

QWidget *FKComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == m_section || m_section == -1) {
        QComboBox *editor = new QComboBox(parent);
        editor->addItems(m_keyValueMap.values());
        return editor;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void FKComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    bool ok = true;
    int key = index.data(Qt::EditRole).toInt(&ok);

    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);

    if (!(index.column() == m_section || m_section == -1) || !ok || !comboBox)
        return QStyledItemDelegate::setEditorData(editor, index);

    int row = 0;
    for (auto iterKey : m_keyValueMap.keys()) {
        if (key == iterKey) {
            comboBox->setCurrentIndex(row);
            break;
        }
        row++;
    }
}

void FKComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = qobject_cast<QComboBox *>(editor);

    if (!(index.column() == m_section || m_section == -1) || !comboBox || !model)
        return QStyledItemDelegate::setModelData(editor, model, index);

    int selectedKey = m_keyValueMap.keys().at(comboBox->currentIndex());
    model->setData(index, selectedKey);
}

const QMap<int, QString> &FKComboBoxDelegate::keyValueMap() const
{
    return m_keyValueMap;
}

void FKComboBoxDelegate::setKeyValueMap(const QMap<int, QString> &keyValueMap)
{
    m_keyValueMap = keyValueMap;
}

int FKComboBoxDelegate::section() const
{
    return m_section;
}

void FKComboBoxDelegate::setSection(int section)
{
    m_section = section;
}
