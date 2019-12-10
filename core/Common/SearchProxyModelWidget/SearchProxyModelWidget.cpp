#include "SearchProxyModelWidget.h"

SearchProxyModelWidget::SearchProxyModelWidget(QWidget *parent) : QWidget(parent)
{
    proxyModel = new QSortFilterProxyModel;

    sortCaseSensitivityCheckBox = new QCheckBox(tr("Разрешить фильтр"));
    filterCaseSensitivityCheckBox = new QCheckBox(tr("С учетом регистра"));

    filterPatternLineEdit = new QLineEdit;
    filterPatternLabel = new QLabel(tr("&Фильтры:"));
    filterPatternLabel->setBuddy(filterPatternLineEdit);

    filterSyntaxComboBox = new QComboBox;
    filterSyntaxComboBox->addItem(tr("Регулярное выражение"), QRegExp::RegExp);
    filterSyntaxComboBox->addItem(tr("Подстановка"), QRegExp::Wildcard);
    filterSyntaxComboBox->addItem(tr("Фиксированная строка"), QRegExp::FixedString);
    filterSyntaxLabel = new QLabel(tr("Поиск:"));
    filterSyntaxLabel->setBuddy(filterSyntaxComboBox);

    filterColumnComboBox = new QComboBox;
    filterColumnLabel = new QLabel(tr("Колонка:"));
    filterColumnLabel->setBuddy(filterColumnComboBox);

    connect(filterPatternLineEdit, &QLineEdit::textChanged,
            this, &SearchProxyModelWidget::filterRegExpChanged);

    typedef void (QComboBox::*QComboIntSignal)(int);
    connect(filterSyntaxComboBox, static_cast<QComboIntSignal>(&QComboBox::currentIndexChanged),
            this, &SearchProxyModelWidget::filterRegExpChanged);
    connect(filterColumnComboBox, static_cast<QComboIntSignal>(&QComboBox::currentIndexChanged),
            this, &SearchProxyModelWidget::filterColumnChanged);
    connect(filterCaseSensitivityCheckBox, &QAbstractButton::toggled,
            this, &SearchProxyModelWidget::filterRegExpChanged);
    connect(sortCaseSensitivityCheckBox, &QAbstractButton::toggled,
            this, &SearchProxyModelWidget::sortChanged);

    QGridLayout *proxyLayout = new QGridLayout;
    proxyLayout->addWidget(filterPatternLabel, 1, 0);
    proxyLayout->addWidget(filterPatternLineEdit, 1, 1, 1, 2);
    proxyLayout->addWidget(filterSyntaxLabel, 2, 0);
    proxyLayout->addWidget(filterSyntaxComboBox, 2, 1, 1, 2);
    proxyLayout->addWidget(filterColumnLabel, 3, 0);
    proxyLayout->addWidget(filterColumnComboBox, 3, 1, 1, 2);
    proxyLayout->addWidget(filterCaseSensitivityCheckBox, 4, 0, 1, 2);
    //proxyLayout->addWidget(sortCaseSensitivityCheckBox, 4, 2);

    setLayout(proxyLayout);

    setWindowTitle(tr("Basic Sort/Filter Model"));
    resize(500, 450);

    filterPatternLineEdit->setText("");
    sortCaseSensitivityCheckBox->setChecked(true);

}

void SearchProxyModelWidget::setSourceModel(QAbstractItemModel *model)
{
    QStringList headers;

    for(int i = 0; i < model->columnCount(); i++) { //Как-то бы игнорировать скрытые столбцы...
        headers.append(model->headerData(i, Qt::Horizontal).toString());
    }

    filterColumnComboBox->addItems(headers);

    proxyModel->setSourceModel(model);
}

void SearchProxyModelWidget::setDefaultParams(QRegExp::PatternSyntax searchType, int column, bool regSense)
{
    filterSyntaxComboBox->setCurrentIndex(searchType == QRegExp::RegExp ? 0 : searchType == QRegExp::Wildcard ? 1 : searchType == QRegExp::FixedString ? 2 : -1);
     //ToDo: если делать фильтрацию скрытых столбцов, тут тоже переделать
    filterColumnComboBox->setCurrentIndex(column);
    filterCaseSensitivityCheckBox->setChecked(regSense);
}

void SearchProxyModelWidget::filterRegExpChanged()
{
    QRegExp::PatternSyntax syntax =
            QRegExp::PatternSyntax(filterSyntaxComboBox->itemData(
                                       filterSyntaxComboBox->currentIndex()).toInt());
    Qt::CaseSensitivity caseSensitivity =
            filterCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive
                                                       : Qt::CaseInsensitive;

    QRegExp regExp(filterPatternLineEdit->text(), caseSensitivity, syntax);
    proxyModel->setFilterRegExp(regExp);
}

void SearchProxyModelWidget::filterColumnChanged()
{
    proxyModel->setFilterKeyColumn(filterColumnComboBox->currentIndex());
}

void SearchProxyModelWidget::sortChanged()
{
    proxyModel->setSortCaseSensitivity(
                sortCaseSensitivityCheckBox->isChecked() ? Qt::CaseSensitive
                                                         : Qt::CaseInsensitive);
}

QSortFilterProxyModel *SearchProxyModelWidget::getProxyModel() const
{
    return proxyModel;
}
