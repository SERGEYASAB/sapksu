#ifndef LOGGER2TABLEMODEL_H
#define LOGGER2TABLEMODEL_H

#include "Common/PrefixHeader.h"

class Logger2TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    Logger2TableModel();
    void append(const QColor &color, const QString &text);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    //    virtual Qt::ItemFlags flags(const QModelIndex & index) const override;
    //    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    //    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    struct LogLine {
        QColor color;
        QString text;
    };

    QList<LogLine> m_logLines;
};

#endif // LOGGER2TABLEMODEL_H
