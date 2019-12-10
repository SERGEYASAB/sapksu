#include "SceneView.h"

SceneView::SceneView(QGraphicsScene *scene, QWidget *parent) : QGraphicsView (scene, parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void SceneView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    emit resized(this->viewport()->rect());
}
