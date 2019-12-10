#ifndef CUSTOMIZABLETABLEVIEW_H
#define CUSTOMIZABLETABLEVIEW_H

#include "Common/PrefixHeader.h"

class CustomizableTableView : public QTableView
{
    Q_OBJECT

public:
    explicit CustomizableTableView(const QString &objectName, QWidget* parent = Q_NULLPTR);
    ~CustomizableTableView();
    void setModel(QAbstractItemModel *model);

protected:
    void hideEvent(QHideEvent *event);
    void closeEvent(QCloseEvent *event) override;

private:
    void loadColumnParameters();
    void saveColumnParameters();
};

#endif // CUSTOMIZABLETABLEVIEW_H
