#ifndef KSU735MANUALFULLTESTINGMODEL_H
#define KSU735MANUALFULLTESTINGMODEL_H

#include "Common/PrefixHeader.h"
#include "Ksu735ManualFullTestingParametr.h"

class Ksu735Node;

class Ksu735ManualFullTestingModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit Ksu735ManualFullTestingModel(Ksu735Node *node,
                                          Ksu735ManualFullTestingParametrsList &parametrsList,
                                          QObject *parent = nullptr);

    void updateAll();
    void updateEnabledParametrsList();
    void updateColumn(int column);
    void updateParametr(Ksu735ManualFullTestingParametr *parametr);

    bool showEnabledOnly() const;
    void setShowEnabledOnly(bool showEnabledOnly);

    Ksu735ManualFullTestingParametr *parametrByIndex(const QModelIndex &index) const;

    inline QColor okColor() const { return m_okColor; }
    inline QColor notSameColor() const { return m_notSameColor; }
    inline QColor errorColor() const { return m_errorColor; }

    void setOkColor(const QColor &color);
    void setNotSameColor(const QColor &color);
    void setErrorColor(const QColor &color);

    /// QAbstractTableModel interface
public:
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    void updateBackgound();

private:
    Ksu735Node *m_node = nullptr;
    Ksu735ManualFullTestingParametrsList &m_parametrsList;
    Ksu735ManualFullTestingParametrsList m_enabledParametrsList;

    bool m_showEnabledOnly = false;

    QColor m_okColor = Qt::green;
    QColor m_notSameColor = Qt::yellow;
    QColor m_errorColor = Qt::red;
};

#endif // KSU735MANUALFULLTESTINGMODEL_H
