#include "CustomizableTableView.h"
#include "AppSettings/AppSettings.h"

CustomizableTableView::CustomizableTableView(const QString &objectName, QWidget *parent)
{
    setObjectName(objectName);
    setParent(parent);
}

CustomizableTableView::~CustomizableTableView()
{
    saveColumnParameters();
}

void CustomizableTableView::setModel(QAbstractItemModel *model)
{
    if (model) {
        QTableView::setModel(model);
        loadColumnParameters();
    }
}

void CustomizableTableView::hideEvent(QHideEvent *event)
{
    //    saveColumnParameters();
    QTableView::hideEvent(event);
}

void CustomizableTableView::closeEvent(QCloseEvent *event)
{
    //    saveColumnParameters();
    QTableView::closeEvent(event);
}

void CustomizableTableView::loadColumnParameters()
{
    auto settings = AppSettings::settings();
    restoreGeometry(settings->value(objectName() + "/geometry", 100).toByteArray());
    if (horizontalHeader())
        for (int i = 0; i < horizontalHeader()->count(); i++)
            setColumnWidth(i, settings->value(objectName() + QString("/column%1Width").arg(i), 100).toInt());
}

void CustomizableTableView::saveColumnParameters()
{
    auto settings = AppSettings::settings();
    settings->beginGroup(objectName());
    settings->setValue("geometry", saveGeometry());
    if (horizontalHeader())
        for (int i = 0; i < horizontalHeader()->count(); i++)
            settings->setValue(QString("column%1Width").arg(i), columnWidth(i));
    settings->endGroup();
    settings->sync();
}
