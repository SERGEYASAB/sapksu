#ifndef PARAMETERALIASSELECTWIDGET_H
#define PARAMETERALIASSELECTWIDGET_H

#include "Common/PrefixHeader.h"

class ParameterAliasSelectModel;

class ParameterAliasSelectWidget : public QWidget
{
    Q_OBJECT

public:
    enum Button {
        OkButton =        1,
        CancelButton =    2,
        AddButton =       4,
        RemoveButton =    8,
        EditButton =      16
    };
    Q_DECLARE_FLAGS(Buttons, Button)
    Q_FLAG(Buttons)

    ParameterAliasSelectWidget(const QString &text = "", QWidget *parent = Q_NULLPTR);
    QTableView *getView();

public slots:
    void selectItem();
    void addItem();
    void removeItem();
    void editItem();
    void setupButtons(ParameterAliasSelectWidget::Buttons buttons);

signals:
    void itemSelected(QString alias);

protected slots:
    void resetQuery();
    void setupFilter();
    void changeControlsBehavior();

private:
    QHBoxLayout *m_hboxMainLayout;
    QVBoxLayout *m_vboxAliasListLayout;
    QVBoxLayout *m_vboxButtonLayout;
    QHBoxLayout *m_hboxFilterLayout;

    QTableView *m_aliasView;
    ParameterAliasSelectModel *m_aliasModel;
    QSortFilterProxyModel *m_proxyModel;
    QLineEdit *m_filterEdit;
    QLabel *m_searchIconLabel;
    QComboBox *m_filterColumnCombo;

    PushButtonPlus *m_okButton;
    PushButtonPlus *m_cancelButton;
    PushButtonPlus *m_addButton;
    PushButtonPlus *m_removeButton;
    PushButtonPlus *m_editButton;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ParameterAliasSelectWidget::Buttons)
#endif // PARAMETERALIASSELECTWIDGET_H
