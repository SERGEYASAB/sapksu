#ifndef EDITABLESCENE_H
#define EDITABLESCENE_H

#include "Common/PrefixHeader.h"
#include "Scene.h"

class EditableScene : public Scene
{
    Q_OBJECT
public:
    EditableScene();

    void insertWidget(ISceneItem *item, const QPoint &pos, const qreal &z = 0) override;
    void removeWidget(ISceneItem *item);
    QPoint itemPos();
    int itemZ();

    void moveItem(int x, int y, int z);
    void setItemX(int x);
    void setItemY(int y);
    void setItemZ(int z);

public slots:
    void bringToFront();
    void sendToBack();

signals:
    void itemMoved(int x, int y, int z);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent * event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QHBoxLayout *m_mainLayout;

};

#endif // EDITABLESCENE_H
