#include "EditableScene.h"
#include "Items/ISceneItem.h"
#include "Items/SceneTextWidget.h"
#include "Items/SceneVariableWidget.h"

EditableScene::EditableScene()
{

}

void EditableScene::insertWidget(ISceneItem *widget, const QPoint &pos, const qreal &z)
{
    Scene::insertWidget(widget, pos, z);
    QGraphicsProxyWidget *pw = m_widgets[widget];
    pw->setWindowFlags(Qt::FramelessWindowHint);
    //    pw->graphicsItem()->setFlags(QGraphicsItem::ItemIsMovable |
    //                                 QGraphicsItem::ItemIsFocusable);
    //    pw->setFocusPolicy(Qt::StrongFocus);
}

void EditableScene::removeWidget(ISceneItem *widget)
{
    if(m_selectedItem == widget) {
        selectItem(nullptr);
    }
    if(m_widgets.contains(widget)) {
        widget->close();
        removeItem(m_widgets[widget]->graphicsItem());
        //Сам виджет эта штука тоже удаляет
        delete m_widgets[widget];
        m_widgets.remove(widget);
    }
}

QPoint EditableScene::itemPos()
{
    if(selectedItem())
        return m_widgets[selectedItem()]->pos().toPoint();
    else
        return QPoint();
}

int EditableScene::itemZ()
{
    if(selectedItem())
        return m_widgets[selectedItem()]->zValue();
    else
        return 0;
}

void EditableScene::setItemX(int x)
{
    if(selectedItem()) {
        QGraphicsProxyWidget *pw = m_widgets[m_selectedItem];
        moveItem(x, pw->y(), pw->zValue());
    }
}

void EditableScene::setItemY(int y)
{
    if(selectedItem()) {
        QGraphicsProxyWidget *pw = m_widgets[m_selectedItem];
        moveItem(pw->x(), y, pw->zValue());
    }
}

void EditableScene::setItemZ(int z)
{
    if(selectedItem()) {
        QGraphicsProxyWidget *pw = m_widgets[m_selectedItem];
        moveItem(pw->x(), pw->y(), z);
    }
}

void EditableScene::moveItem(int x, int y, int z)
{
    if(!selectedItem())
        return;

    QGraphicsProxyWidget *pw = m_widgets[selectedItem()];
    if (pw->x() != x || pw->y() != y || pw->zValue() != z) {
        pw->setPos((int)x, (int)y);
        pw->setZValue(z);
        emit itemMoved(x, y, z);
    }
}


void EditableScene::bringToFront()
{
    if(!m_selectedItem)
        return;

    qreal z = 0;
    foreach(QGraphicsItem *item, m_widgets[m_selectedItem]->topLevelItem()->collidingItems()) {
        if(item->zValue() >= z)
            z = item->zValue() + 1;
    }
    moveItem(m_widgets[m_selectedItem]->x(), m_widgets[m_selectedItem]->y(), z);
}

void EditableScene::sendToBack()
{
    if(!m_selectedItem)
        return;

    qreal z = 0;
    foreach(QGraphicsItem *item, m_widgets[m_selectedItem]->topLevelItem()->collidingItems()) {
        if(item->zValue() <= z)
            z = item->zValue() - 1;
    }
    moveItem(m_widgets[m_selectedItem]->x(), m_widgets[m_selectedItem]->y(), z);
}

void EditableScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        ISceneItem *item = nullptr;
        if (itemAt(event->scenePos(), QTransform()))
        {
            Scene::SceneItemType type = Scene::Unknown;

            QGraphicsProxyWidget *pw = dynamic_cast<QGraphicsProxyWidget*>(itemAt(event->scenePos(), QTransform())->topLevelWidget());
            if (pw && m_widgets.values().contains(pw))
            {
                item = m_widgets.key(pw);

                //Если курсор ресайзовый, пропускаем евент
                if (pw->cursor().shape() == Qt::SizeFDiagCursor)
                {
                    QGraphicsScene::mousePressEvent(event);
                    return;
                }

                if (dynamic_cast<SceneTextWidget *>(m_widgets.key(pw)))
                    type = Scene::Text;
                else if(dynamic_cast<SceneVariableWidget *>(m_widgets.key(pw)))
                    type = Scene::Variable;
                else
                    type = Scene::Unknown;
            }

            if (item && type != Scene::Unknown)
            {
                if (selectedItem() != item)
                    selectItem(item);
                QByteArray itemData;
                QDataStream dataStream(&itemData, QIODevice::WriteOnly);
                QPoint point = item->mapFromGlobal(event->screenPos());

                //Извращение
                //Как иначе - не знаю. Раньше mousePress был переопределен у самих виджетов сцены
                //Теперь ими рулит сама сцена
                //В dropEvent event->source() не возвращает ничего, при предыдущей схеме он возвращал виджет сцены
                //event->widget() возвращает непонятную хрень - это НЕ указатель ни на сцену, ни на вьюшку, ни на виджеты сцены, ни на их прокси
                //Поэтому пока передаю указатель так
                dataStream << type << point << (quintptr)item;

                QMimeData *mimeData = new QMimeData;
                mimeData->setData("scene/sceneitemtype", itemData);

                QDrag *drag = new QDrag(this);
                drag->setMimeData(mimeData);
                drag->setHotSpot(point);
                drag->setPixmap(item->grab());
                item->hide();
                drag->exec(Qt::MoveAction);
                item->show();
            }
        }
        else
            if (m_selectedItem)
                selectItem(nullptr);
    }
    //  QGraphicsScene::mousePressEvent(event);
}

void EditableScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasFormat("scene/sceneitemtype"))
        event->setAccepted(true);
    else
        event->ignore();
}

void EditableScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if(event->mimeData()->hasFormat("scene/sceneitemtype"))
    {
        event->setAccepted(true);
        if(event->proposedAction() == Qt::MoveAction) {
            QByteArray data = event->mimeData()->data("scene/sceneitemtype");
            QDataStream dataStream(&data, QIODevice::ReadOnly);
            QPoint sourcePoint;
            int type = Scene::Unknown;
            dataStream>>type>>sourcePoint;
            QPoint targetPoint(event->scenePos().x() - sourcePoint.x(), event->scenePos().y() - sourcePoint.y());
            emit itemMoved(targetPoint.x(), targetPoint.y(), m_widgets[selectedItem()]->zValue());
        }
    }
    else
        event->ignore();
}

void EditableScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasFormat("scene/sceneitemtype"))
    {
        QByteArray data = event->mimeData()->data("scene/sceneitemtype");
        QDataStream dataStream(&data, QIODevice::ReadOnly);
        QPoint sourcePoint;
        int type = Scene::Unknown;
        int ptr = 0;
        dataStream >> type >> sourcePoint >> ptr;
        ISceneItem *item = (ISceneItem *)ptr;

        if (event->proposedAction() == Qt::CopyAction)
        {
            if (type == Scene::Text)
                item = new SceneTextWidget(tr("Новый текст"), true);

            else if (type == Scene::Variable)
                item = new SceneVariableWidget(tr("Новая переменная"), true);

            insertWidget(item, QPoint(int(event->scenePos().x()), int(event->scenePos().y())));
            selectItem(item);
            event->setDropAction(Qt::CopyAction);
        }
        else if (event->proposedAction() == Qt::MoveAction)
        {
            //BUG: когда начнем проверять указатель на пустоту прежде чем вызывать у него методы?)
            //а раньше тут был вылет
            auto widget = m_widgets.value(item, nullptr);
            if (widget) {
                widget->setPos(event->scenePos().x() - sourcePoint.x(),
                               event->scenePos().y() - sourcePoint.y());

                event->setDropAction(Qt::MoveAction);

                emit itemMoved(widget->x(), widget->y(), widget->zValue());
            }
        }
        update();
        event->accept();
    }
    // QGraphicsScene::dropEvent(event);
}

void EditableScene::keyPressEvent(QKeyEvent *event)
{
    Qt::KeyboardModifiers modifiers = event->modifiers();

    QGraphicsProxyWidget *pw;
    if (selectedItem())
        pw = m_widgets[m_selectedItem];
    switch (event->key())
    {
        case Qt::Key_Delete: {
            removeWidget(m_selectedItem);
            break;
        }

        case Qt::Key_Left: {
            if (pw) {
                QPointF currentPos = pw->pos();
                pw->setPos(modifiers & Qt::ControlModifier ?
                               currentPos.x() - 10 : currentPos.x() - 1,
                           currentPos.y());
                emit itemMoved(pw->x(), pw->y(), pw->zValue());
            }
            break;
        }

        case Qt::Key_Right: {
            if (pw) {
                QPointF currentPos = pw->pos();
                pw->setPos(modifiers & Qt::ControlModifier ?
                               currentPos.x() + 10 : currentPos.x() + 1,
                           currentPos.y());
                emit itemMoved(pw->x(), pw->y(), pw->zValue());
            }
            break;
        }

        case Qt::Key_Up: {
            if (pw) {
                QPointF currentPos = pw->pos();
                pw->setPos(currentPos.x(),
                           modifiers & Qt::ControlModifier ?
                               currentPos.y() - 10 : currentPos.y() - 1);
                emit itemMoved(pw->x(), pw->y(), pw->zValue());
            }
            break;
        }

        case Qt::Key_Down: {
            if (pw) {
                QPointF currentPos = pw->pos();
                pw->setPos(currentPos.x(),
                           modifiers & Qt::ControlModifier ?
                               currentPos.y() + 10 : currentPos.y() + 1);
                emit itemMoved(pw->x(), pw->y(), pw->zValue());
            }
            break;
        }
    }
}
