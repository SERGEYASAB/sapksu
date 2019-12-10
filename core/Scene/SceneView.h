#ifndef SCENEVIEW_H
#define SCENEVIEW_H

#include "Common/PrefixHeader.h"

class SceneView : public QGraphicsView
{
    Q_OBJECT
public:
    SceneView(QWidget *parent = Q_NULLPTR);
    SceneView(QGraphicsScene *scene, QWidget *parent = Q_NULLPTR);

signals:
    void resized(QRect rect);

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // SCENEVIEW_H
