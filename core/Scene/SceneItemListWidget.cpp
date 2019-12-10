#include "SceneItemListWidget.h"

SceneItemListWidget::SceneItemListWidget()
{
    m_stringTypes[tr("Текст")] = Scene::Text;
    m_stringTypes[tr("Переменная")] = Scene::Variable;

    for(auto i : m_stringTypes.keys())
        addItem(i);

    setDragDropMode(QListWidget::DragOnly);
}

void SceneItemListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("scene/sceneitemtype"))
        event->accept();
    else
        event->ignore();
}

void SceneItemListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("scene/sceneitemtype")) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    } else {
        event->ignore();
    }
}

void SceneItemListWidget::startDrag(Qt::DropActions supportedActions)
{
    Q_UNUSED(supportedActions)

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    Scene::SceneItemType type = m_stringTypes[currentItem()->text()];
    dataStream << type;

    QMimeData *mimeData = new QMimeData;
    mimeData->setData("scene/sceneitemtype", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->exec(Qt::CopyAction);
}

