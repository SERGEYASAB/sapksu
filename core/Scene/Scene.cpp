#include "Scene.h"
#include "Items/ISceneItem.h"

Scene::Scene()
{
    m_selectionShape = Q_NULLPTR;
    m_selectedItem = nullptr;
}

void Scene::insertWidget(ISceneItem *item, const QPoint &pos, const qreal &z)
{
    m_widgets[item] = addWidget(item);
    m_widgets[item]->setPos(pos);
    m_widgets[item]->setZValue(z);
    update();
}

QList<ISceneItem *> Scene::getWidgets()
{
    return m_widgets.keys();
}

QGraphicsProxyWidget *Scene::getProxyWidget(ISceneItem *item)
{
    return m_widgets[item];
}

QList<QGraphicsProxyWidget *> Scene::getProxyWidgets()
{
    return m_widgets.values();
}

ISceneItem *Scene::selectedItem()
{
    return m_selectedItem;
}

void Scene::setRect(QRect rect)
{
    setSceneRect(rect);
}

void Scene::setBorder(bool state)
{
    if(m_selectedItem)
        m_selectedItem->setFrame(state);
}

void Scene::selectItem(ISceneItem *item)
{
    if (m_selectedItem)
        m_selectedItem->setFocused(false);
    m_selectedItem = item;

    if (item)
        m_selectedItem->setFocused(true);
    emit focusChanged();
}

void Scene::helpEvent(QGraphicsSceneHelpEvent *event)
{
    QGraphicsItem *item = itemAt(event->scenePos(), QTransform());
    if(item)
    {
        QGraphicsProxyWidget *pw = dynamic_cast<QGraphicsProxyWidget*>(item->topLevelWidget());
        if(m_widgets.values().contains(pw))
            item->setToolTip(m_widgets.key(pw)->toolTip());
    }
    QGraphicsScene::helpEvent(event);
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        ISceneItem *item = nullptr;
        if (itemAt(event->scenePos(), QTransform()))
        {
            QGraphicsProxyWidget *pw = dynamic_cast<QGraphicsProxyWidget*>(itemAt(event->scenePos(), QTransform())->topLevelWidget());
            if (pw && m_widgets.values().contains(pw))
            {
                item = m_widgets.key(pw);
                if (selectedItem() != item)
                    selectItem(item);
            }
        }
        else
            if (m_selectedItem)
                selectItem(nullptr);
    }
}
