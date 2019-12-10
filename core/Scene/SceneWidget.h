#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include "Common/PrefixHeader.h"

class Scene;
class SceneView;

class SceneWidget : public QWidget
{
public:
    explicit SceneWidget(const QString &filePath, QWidget *parent = nullptr);

    bool loadConfiguration(const QString &fileName);
    void traverseXml(const QDomNode &node);

protected:
    QHBoxLayout *m_mainLayout;
    Scene *m_scene;
    SceneView *m_view;
    QDomDocument m_xml;

    QAction *m_pollVarAction;

    void keyPressEvent(QKeyEvent *event) override;
};

#endif // SCENEWIDGET_H
