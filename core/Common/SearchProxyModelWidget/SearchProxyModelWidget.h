#ifndef SEARCHPROXYMODELWIDGET_H
#define SEARCHPROXYMODELWIDGET_H

#include "Common/PrefixHeader.h"

class SearchProxyModelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchProxyModelWidget(QWidget *parent = nullptr);

    void setSourceModel(QAbstractItemModel *model);
    void setDefaultParams(QRegExp::PatternSyntax searchType = QRegExp::Wildcard, int column = 0, bool regSense = false);

    QSortFilterProxyModel *getProxyModel() const;

private slots:
    void filterRegExpChanged();
    void filterColumnChanged();
    void sortChanged();

private:
    QSortFilterProxyModel *proxyModel;

    QCheckBox *filterCaseSensitivityCheckBox;
    QCheckBox *sortCaseSensitivityCheckBox;
    QLabel *filterPatternLabel;
    QLabel *filterSyntaxLabel;
    QLabel *filterColumnLabel;
    QLineEdit *filterPatternLineEdit;
    QComboBox *filterSyntaxComboBox;
    QComboBox *filterColumnComboBox;
};

#endif // SEARCHPROXYMODELWIDGET_H
