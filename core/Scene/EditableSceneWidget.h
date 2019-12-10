#ifndef EDITABLESCENEWIDGET_H
#define EDITABLESCENEWIDGET_H

#include "Common/PrefixHeader.h"

class EditableScene;
class SceneItemListWidget;
class SceneView;

class EditableSceneWidget : public QMainWindow
{
public:
    explicit EditableSceneWidget(const QString &filePath = "", QWidget *parent = nullptr);

    bool loadConfiguration(const QString &fileName);
    void traverseXml(const QDomNode &node);
    void saveConfiguration();
    bool save();
    bool saveAs();

public slots:
    void removeItem();

protected:


private:
    QDomDocument makeXml();
    bool saveView(const QString &fileName, QDomDocument &doc);
    void closeEvent(QCloseEvent *event);

    EditableScene *m_scene;
    SceneItemListWidget *m_sceneItemListWidget;
    SceneView *m_view;
    QDomDocument m_xml;
    QDockWidget *m_itemDock;
    QToolBar *m_fileToolBar;
    QToolBar *m_fontToolBar;
    QToolBar *m_editToolBar;
    QToolBar *m_commonToolBar;
    QString m_filePath;
    QColorDialog *m_itemFontColorDialog;
    QColorDialog *m_itemBackgroundColorDialog;

    QAction *m_saveViewAction;
    QAction *m_saveAsViewAction;
    QAction *m_itemFontColorAction;
    QAction *m_bringToFrontAction;
    QAction *m_sendBackAction;
    QAction *m_removeItemAction;

    QStatusBar *m_statusBar;
    QLabel *m_itemPosLabel;


    QDockWidget *m_propDock;
    QWidget *m_propWidget;
    QFormLayout *m_propLayout;
    QLabel *m_propTextLabel;
    QWidget *m_propTextEdit;
    QSpinBox *m_propSpinX;
    QSpinBox *m_propSpinY;
    QSpinBox *m_propSpinZ;
    QSpinBox *m_propWidthSpin;
    QSpinBox *m_propHeightSpin;
    QFontComboBox *m_propFontCombo;
    QComboBox *m_propFontSizeCombo;
    QPushButton *m_propTextColorButton;
    QPushButton *m_propBackgroundColorButton = nullptr;
    QCheckBox *m_propBoldCheck;
    QCheckBox *m_propItalicCheck;
    QCheckBox *m_propUnderlineCheck;
    QCheckBox *m_propBorderCheck;
    QSpinBox *m_sceneSizeSpinX = nullptr;
    QSpinBox *m_sceneSizeSpinY = nullptr;
    QGraphicsLineItem *xLine = nullptr;
    QGraphicsLineItem *yLine = nullptr;
    int m_sceneSizeX;
    int m_sceneSizeY;
    QWidget *createPropWidget();

private slots:
    void showItemProps();
};

#endif // EDITABLESCENEWIDGET_H
