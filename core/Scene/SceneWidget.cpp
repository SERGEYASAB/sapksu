#include "SceneWidget.h"
#include "Scene.h"
#include "SceneView.h"
#include "Items/ISceneItem.h"
#include "Items/SceneTextWidget.h"
#include "Items/SceneVariableWidget.h"

SceneWidget::SceneWidget(const QString &filePath, QWidget *parent) : QWidget(parent)
{
    m_scene = new Scene();
    //m_scene->setSceneRect(QRectF(0, 0, 500, 500));

    loadConfiguration(filePath);
    traverseXml(m_xml);

    m_view = new SceneView(m_scene);
    connect(m_view, &SceneView::resized, m_scene, &Scene::setRect);

    QString name = filePath.mid(filePath.lastIndexOf('\\') + 1).left(filePath.lastIndexOf('.'));
    name = name.left(name.lastIndexOf("."));
    setWindowTitle(name);

    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_view);

    setLayout(m_mainLayout);
}

bool SceneWidget::loadConfiguration(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << tr("Не удается открыть файл представления");
        return false;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!m_xml.setContent(file.readAll(), &errorMsg, &errorLine, &errorColumn)) {
        qCritical() << tr("Конфигурационный файл поврежден") << errorMsg << errorLine << errorColumn;
        file.close();
        return false;
    }
    file.close();
    return true;
}

void SceneWidget::traverseXml(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();

    while(!domNode.isNull()) {
        if(domNode.isElement()) {
            QDomElement domElement = domNode.toElement();
            if(!domElement.isNull()) {
                ISceneItem *item = nullptr;
                if(domElement.tagName() == "View") {
                    if (domElement.hasAttribute("backgroundColor"))
                        m_scene->setBackgroundBrush(QBrush(QColor(domElement.attribute("backgroundColor"))));
                }
                else {
                    if(domElement.tagName() == "SceneTextWidget") {
                        item = new SceneTextWidget(domElement.attribute("text"));
                        m_scene->insertWidget(item,
                                              QPoint(domElement.attribute("x").toInt(), domElement.attribute("y").toInt()),
                                              domElement.attribute("z").toDouble());
                    }
                    else if(domElement.tagName() == "SceneVariableWidget") {
                        item = new SceneVariableWidget(domElement.attribute("alias"), false);
                        m_scene->insertWidget(item,
                                              QPoint(domElement.attribute("x").toInt(), domElement.attribute("y").toInt()),
                                              domElement.attribute("z").toDouble());
                    }
                    else if(domElement.tagName() == "SceneProperties") {
                        resize(domElement.attribute("sceneSizeX").toInt(), domElement.attribute("sceneSizeY").toInt());
                    }

                    if(item) {
                        QFont font;
                        font.fromString(domElement.attribute("font"));
                        item->setFont(font);
                        item->setFontColor(QColor(domElement.attribute("fontColor")));
                        item->setBackgroundColor(QColor(domElement.attribute("backgroundColor")));
                        item->setFrame(domElement.attribute("frame").toInt());
                        item->resize(domElement.attribute("width").toUInt(), domElement.attribute("height").toUInt());
                    }
                }
            }

        }
        traverseXml(domNode);
        domNode = domNode.nextSibling();
    }
}

void SceneWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_F5:

            ISceneItem *item = m_scene->selectedItem();
            if (!item)
                return;

            if (SceneVariableWidget *svw = dynamic_cast<SceneVariableWidget*>(item)) {
                svw->poll();
            }
            break;
    }
}

