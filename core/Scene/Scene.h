#ifndef SCENE_H
#define SCENE_H

#include "Common/PrefixHeader.h"

class ISceneItem;
class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    enum SceneItemType { Unknown, Text, Variable };

    Scene();
    virtual void insertWidget(ISceneItem *item, const QPoint &pos, const qreal &z = 0.0);
    virtual ISceneItem *selectedItem();
    QList<ISceneItem *> getWidgets();
    QGraphicsProxyWidget *getProxyWidget(ISceneItem *item);
    QList<QGraphicsProxyWidget *> getProxyWidgets();

public slots:
    void setRect(QRect rect);
    void setBorder(bool state);
    void selectItem(ISceneItem *item);

signals:
    void focusChanged();

protected:
    void helpEvent(QGraphicsSceneHelpEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QMap<ISceneItem *, QGraphicsProxyWidget *> m_widgets;
    ISceneItem *m_selectedItem;
    QGraphicsRectItem *m_selectionShape;
};

#endif // SCENE_H
