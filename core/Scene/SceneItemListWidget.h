#ifndef SCENEITEMLISTWIDGET_H
#define SCENEITEMLISTWIDGET_H

#include "Common/PrefixHeader.h"
#include "Scene.h"

class SceneItemListWidget : public QListWidget
{
public:
    SceneItemListWidget();

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dragMoveEvent(QDragMoveEvent *event) Q_DECL_OVERRIDE;
    void startDrag(Qt::DropActions supportedActions) Q_DECL_OVERRIDE;

private:
    QHash<QString, Scene::SceneItemType> m_stringTypes;
};

#endif // SCENEITEMLISTWIDGET_H
