#include "EditableSceneWidget.h"
#include "EditableScene.h"
#include "SceneItemListWidget.h"
#include "SceneView.h"
#include "Items/SceneTextWidget.h"
#include "Items/SceneVariableWidget.h"
#include "Common/ParametrAliases/ParameterAliasWidget.h"
#include "ReportManager/FaultReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

EditableSceneWidget::EditableSceneWidget(const QString &filePath, QWidget *parent) : QMainWindow(parent)
{
    m_statusBar = new QStatusBar();
    m_itemPosLabel = new QLabel(m_statusBar);
    m_statusBar->addPermanentWidget(m_itemPosLabel);
    setStatusBar(m_statusBar);

    QString name = filePath.mid(filePath.lastIndexOf('\\') + 1).left(filePath.lastIndexOf('.'));
    name = name.left(name.lastIndexOf("."));
    setWindowTitle(filePath.isEmpty() ? "Новое представление" : name);

    m_scene = new EditableScene();
    //  m_scene->setSceneRect(QRectF(0, 200, 100, 100));
    connect(m_scene, &EditableScene::focusChanged, this, &EditableSceneWidget::showItemProps);

    m_view = new SceneView(m_scene);
    connect(m_view, &SceneView::resized, m_scene, &EditableScene::setRect);

    m_filePath = filePath;

    loadConfiguration(filePath);
    traverseXml(m_xml);
    m_xml = makeXml();

    m_fileToolBar = new QToolBar();
    m_saveViewAction = new QAction(QIcon(":/Images/document-save.png"), tr("&Сохранить"), this);
    m_saveViewAction->setShortcuts(QKeySequence::Save);
    m_saveViewAction->setStatusTip(tr("Сохранить представление"));
    connect(m_saveViewAction, &QAction::triggered, this, &EditableSceneWidget::save);
    m_saveViewAction->setDisabled(filePath.isEmpty());
    m_fileToolBar->addAction(m_saveViewAction);

    m_saveAsViewAction = new QAction(QIcon(":/Images/document-save-as.png"), tr("Сохранить &как..."), this);
    m_saveAsViewAction->setShortcuts(QKeySequence::SaveAs);
    m_saveAsViewAction->setStatusTip(tr("Сохранить представление как..."));
    connect(m_saveAsViewAction, &QAction::triggered, this, &EditableSceneWidget::saveAs);
    m_fileToolBar->addAction(m_saveAsViewAction);
    addToolBar(m_fileToolBar);

    m_itemFontColorDialog = new QColorDialog();
    m_itemFontColorDialog->setOption(QColorDialog::ShowAlphaChannel);
    m_itemBackgroundColorDialog = new QColorDialog();
    m_itemBackgroundColorDialog->setOption(QColorDialog::ShowAlphaChannel);

    m_editToolBar = new QToolBar();
    m_bringToFrontAction = new QAction(QIcon(":/Images/layerup.png"),
                                       tr("Вперед"), this);
    m_bringToFrontAction->setStatusTip(tr("Перенести на передний план"));
    connect(m_bringToFrontAction, &QAction::triggered, m_scene, &EditableScene::bringToFront);
    m_editToolBar->addAction(m_bringToFrontAction);

    m_sendBackAction = new QAction(QIcon(":/Images/layerdown.png"), tr("Назад"), this);
    m_sendBackAction->setStatusTip(tr("Перенести на задний план"));
    connect(m_sendBackAction, &QAction::triggered, m_scene, &EditableScene::sendToBack);
    m_editToolBar->addAction(m_sendBackAction);

    m_removeItemAction = new QAction(QIcon(":/Images/dialog-error.png"), tr("Удалить"), this);
    m_removeItemAction->setStatusTip(tr("Удалить объект"));
    connect(m_removeItemAction, &QAction::triggered, this, &EditableSceneWidget::removeItem);
    m_editToolBar->addAction(m_removeItemAction);
    m_editToolBar->setDisabled(true);
    addToolBar(m_editToolBar);

    m_itemDock = new QDockWidget(tr("Элементы"));
    m_itemDock->setFeatures(QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::LeftDockWidgetArea, m_itemDock);
    m_sceneItemListWidget = new SceneItemListWidget();
    m_itemDock->setWidget(m_sceneItemListWidget);
    m_itemDock->setMinimumWidth(150);

    m_propDock = new QDockWidget(tr("Свойства элемента"));
    m_propDock->setFeatures(QDockWidget::DockWidgetMovable);
    m_propDock->setMinimumWidth(150);
    addDockWidget(Qt::LeftDockWidgetArea, m_propDock);
    setCentralWidget(m_view);
    showItemProps();
}

bool EditableSceneWidget::loadConfiguration(const QString &fileName)
{
    if (fileName.isEmpty())
        return false;

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

void EditableSceneWidget::traverseXml(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();

    while (!domNode.isNull()) {
        if (domNode.isElement()) {
            QDomElement domElement = domNode.toElement();
            if (!domElement.isNull()) {
                if (domElement.tagName() == "View") {
                    if (domElement.hasAttribute("backgroundColor"))
                        m_scene->setBackgroundBrush(QBrush(QColor(domElement.attribute("backgroundColor"))));
                } else {
                    ISceneItem *item = nullptr;
                    if (domElement.tagName() == "SceneTextWidget") {
                        item = new SceneTextWidget(domElement.attribute("text"), true);
                        m_scene->insertWidget(item,
                                              QPoint(domElement.attribute("x").toInt(), domElement.attribute("y").toInt()),
                                              domElement.attribute("z").toDouble());
                    }
                    else if (domElement.tagName() == "SceneVariableWidget") {
                        item = new SceneVariableWidget(domElement.attribute("alias"), true, true);
                        m_scene->insertWidget(item,
                                              QPoint(domElement.attribute("x").toInt(), domElement.attribute("y").toInt()),
                                              domElement.attribute("z").toDouble());
                    }
                    else if (domElement.tagName() == "SceneProperties") {
                        m_sceneSizeX = domElement.attribute("sceneSizeX", "500").toInt();
                        m_sceneSizeY = domElement.attribute("sceneSizeY", "500").toInt();
                        resize(m_sceneSizeX, m_sceneSizeY);
                    }
                    if (item) {
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

bool EditableSceneWidget::saveView(const QString &fileName, QDomDocument &doc)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        ErrorClassifier::instance()->showError(34);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(34));
        return false;
    }

    QTextStream stream(&file);
    m_xml = makeXml();
    m_xml.save(stream, 4);
    file.close();
    return true;
}

QDomDocument EditableSceneWidget::makeXml()
{
    QDomDocument xml;
    QDomElement root = xml.createElement("View");
    if(m_scene->backgroundBrush() == Qt::NoBrush)
        m_scene->setBackgroundBrush(QBrush(QColor(Qt::white)));
    root.setAttribute("backgroundColor", m_scene->backgroundBrush().color().name(QColor::HexArgb));
    xml.appendChild(root);
    QDomElement item;
    for (auto i : m_scene->getWidgets())
    {
        if(SceneTextWidget *sti = dynamic_cast<SceneTextWidget*>(i))
        {
            item = xml.createElement("SceneTextWidget");
            item.setAttribute("text", sti->getText());
        }
        else if (SceneVariableWidget *svi = dynamic_cast<SceneVariableWidget*>(i))
        {
            item = xml.createElement("SceneVariableWidget");
            item.setAttribute("alias", svi->getAlias());
        }
        item.setAttribute("x", m_scene->getProxyWidget(i)->x());
        item.setAttribute("y", m_scene->getProxyWidget(i)->y());
        item.setAttribute("z", m_scene->getProxyWidget(i)->zValue());

        item.setAttribute("font", i->getFont().toString());
        item.setAttribute("fontColor", i->getFontColor().name(QColor::HexArgb));
        item.setAttribute("backgroundColor", i->getBackgroundColor().name(QColor::HexArgb));

        item.setAttribute("frame", i->getFrame());
        item.setAttribute("width", i->width());
        item.setAttribute("height", i->height());
        root.appendChild(item);
    }
    item = xml.createElement("SceneProperties");
    item.setAttribute("sceneSizeX", m_sceneSizeX);
    item.setAttribute("sceneSizeY", m_sceneSizeY);
    root.appendChild(item);
    return xml;
}

void EditableSceneWidget::closeEvent(QCloseEvent *event)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Сохранение изменений"));
    msgBox.setText(tr("Сохранить изменения?"));
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Нет"), QMessageBox::NoRole);
    QPushButton *cancelButton = msgBox.addButton(tr("&Отмена"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelButton);

    if (m_filePath.isEmpty())
    {
        msgBox.exec();
        if (msgBox.clickedButton() == cancelButton) {
            event->ignore();
            return;
        }
        else if (msgBox.clickedButton() == yesButton) {
            if(saveAs())
                event->accept();
            else {
                event->ignore();
                return;
            }
        }
    }
    else
    {
        QDomDocument doc = makeXml();
        if (m_xml.toString(4).compare(doc.toString(4))) {
            msgBox.exec();

            if (msgBox.clickedButton() == cancelButton) {
                event->ignore();
                return;
            }
            else if (msgBox.clickedButton() == yesButton) {
                if(save())
                    event->accept();
                else {
                    event->ignore();
                    return;
                }
            }
        }
    }
    QMainWindow::closeEvent(event);
}

QWidget *EditableSceneWidget::createPropWidget()
{
    ISceneItem *item = m_scene->selectedItem();

    m_propWidget = new QWidget;
    m_propLayout = new QFormLayout;
    m_propWidget->setLayout(m_propLayout);

    if(!item) {
        m_propBackgroundColorButton = new QPushButton;
        auto setColor = [this]() {
            m_propBackgroundColorButton->setStyleSheet("background-color: " + m_scene->backgroundBrush().color().name(QColor::HexArgb) + ";\nborder: 2px solid black;");
        };
        setColor();
        connect(m_propBackgroundColorButton, &QPushButton::clicked, [=]()
        {
            m_itemBackgroundColorDialog->setCurrentColor(m_scene->backgroundBrush().color());
            m_itemBackgroundColorDialog->exec();
            if (m_itemBackgroundColorDialog->result() == QColorDialog::Accepted)
            {
                m_scene->setBackgroundBrush(QBrush(m_itemBackgroundColorDialog->selectedColor()));
                setColor();
            }
        });
        m_propLayout->addRow(new QLabel(tr("Цвет фона")), m_propBackgroundColorButton);

        m_sceneSizeSpinX = new QSpinBox;
        m_sceneSizeSpinY = new QSpinBox;

        auto setSceneSize = [=](){
            m_sceneSizeX = m_sceneSizeSpinX->value();
            m_sceneSizeY = m_sceneSizeSpinY->value();
            if (xLine)
                m_scene->removeItem(xLine);
            xLine = m_scene->addLine(QLineF(m_sceneSizeX, -5000, m_sceneSizeX, m_sceneSizeY), QPen(Qt::DashLine));
            if (yLine)
                m_scene->removeItem(yLine);
            yLine = m_scene->addLine(QLineF(-5000, m_sceneSizeY, m_sceneSizeX, m_sceneSizeY), QPen(Qt::DashLine));
        };

        m_sceneSizeSpinX->setRange(0, 1920);
        m_sceneSizeSpinX->setValue(m_sceneSizeX);
        connect(m_sceneSizeSpinX, &QSpinBox::editingFinished, setSceneSize);
        m_propLayout->addRow(new QLabel(tr("X")), m_sceneSizeSpinX);

        m_sceneSizeSpinY->setRange(0, 1080);
        m_sceneSizeSpinY->setValue(m_sceneSizeY);
        connect(m_sceneSizeSpinY, &QSpinBox::editingFinished, setSceneSize);
        m_propLayout->addRow(new QLabel(tr("Y")), m_sceneSizeSpinY);

        emit m_sceneSizeSpinX->editingFinished();
    }
    else {
        if (SceneTextWidget *stw = dynamic_cast<SceneTextWidget *>(item)) {
            m_propTextLabel = new QLabel(tr("Текст "));
            m_propTextEdit = new QLineEdit;
            static_cast<QLineEdit*>(m_propTextEdit)->setText(stw->getText());
            connect(static_cast<QLineEdit *>(m_propTextEdit), &QLineEdit::textChanged, stw, &SceneTextWidget::setText);
        } else if (SceneVariableWidget *svw = dynamic_cast<SceneVariableWidget *>(item)) {
            m_propTextLabel = new QLabel(tr("Переменная "));
            m_propTextEdit = new ParameterAliasWidget;
            static_cast<ParameterAliasWidget*>(m_propTextEdit)->setupAlias(svw->getAlias());

            connect(static_cast<ParameterAliasWidget *>(m_propTextEdit),    &ParameterAliasWidget::aliasSelected,
                    svw,                                                    &SceneVariableWidget::setAlias);
        }
        m_propLayout->addRow(m_propTextLabel, m_propTextEdit);

        m_propSpinX = new QSpinBox;
        m_propSpinX->setRange(0, 1700/*m_view->viewport()->width()*/);
        connect(m_propSpinX, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), m_scene, &EditableScene::setItemX);
        m_propLayout->addRow(new QLabel(tr("X ")), m_propSpinX);

        m_propSpinY = new QSpinBox;
        m_propSpinY->setRange(0, 1000/*m_view->viewport()->height()*/);
        connect(m_propSpinY, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), m_scene, &EditableScene::setItemY);
        m_propLayout->addRow(new QLabel(tr("Y ")), m_propSpinY);

        m_propSpinZ = new QSpinBox;
        connect(m_propSpinZ, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), m_scene, &EditableScene::setItemZ);
        m_propLayout->addRow(new QLabel(tr("Z ")), m_propSpinZ);

        auto setLocation = [this](int x, int y, int z) {
            m_propSpinX->setValue(x);
            m_propSpinY->setValue(y);
            m_propSpinZ->setValue(z);
        };
        setLocation(m_scene->itemPos().x(), m_scene->itemPos().y(), m_scene->itemZ());
        connect(m_scene, &EditableScene::itemMoved, setLocation);

        // Надо бы знать максимальный размер сцены
        //    connect(m_view, &SceneView::resized, [this](QRect rect)
        //    {
        //        if (m_scene->selectedItem()) {
        //            m_propSpinX->blockSignals(true);
        //            m_propSpinX->setRange(0, rect.width());
        //            m_propSpinX->blockSignals(false);
        //            m_propSpinY->blockSignals(true);
        //            m_propSpinY->setRange(0, rect.height());
        //            m_propSpinY->blockSignals(false);
        //        }
        //    });


        m_propWidthSpin = new QSpinBox;
        m_propWidthSpin->setRange(0, m_view->viewport()->width());
        connect(m_propWidthSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int w)
        {
            m_scene->selectedItem()->resize(w,m_scene->selectedItem()->size().height());
        });

        m_propLayout->addRow(new QLabel(tr("Ширина ")), m_propWidthSpin);

        m_propHeightSpin = new QSpinBox;
        m_propHeightSpin->setRange(0, m_view->viewport()->height());
        connect(m_propHeightSpin, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), [this](int h)
        {
            m_scene->selectedItem()->resize(m_scene->selectedItem()->size().width(), h);
        });

        auto setSize = [this](QSize size) {
            m_propWidthSpin->setValue(size.width());
            m_propHeightSpin->setValue(size.height());
        };
        setSize(item->size());
        connect(item, &ISceneItem::resized, setSize);

        m_propLayout->addRow(new QLabel(tr("Высота ")), m_propHeightSpin);

        m_propFontCombo = new QFontComboBox;
        m_propFontCombo->setCurrentFont(item->getFont());
        connect(m_propFontCombo, &QFontComboBox::currentFontChanged, [this](const QFont &font)
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item && item->getFont() != m_propFontCombo->currentFont())
            {
                QFont resizedFont = font;
                resizedFont.setPointSize(item->getFont().pointSize());
                item->setFont(resizedFont);
            }
        });
        m_propLayout->addRow(new QLabel(tr("Шрифт ")), m_propFontCombo);

        m_propFontSizeCombo = new QComboBox;
        for (int i = 8; i < 30; i = i + 2)
            m_propFontSizeCombo->addItem(QString().setNum(i));
        QIntValidator *validator = new QIntValidator(2, 64, this);
        m_propFontSizeCombo->setValidator(validator);
        m_propFontSizeCombo->setCurrentText(QString::number(item->getFont().pointSize()));
        connect(m_propFontSizeCombo, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentTextChanged), [this](const QString &size)
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item && item->getFont().pointSize() != size.toInt())
            {
                QFont font = item->getFont();
                font.setPointSize(size.toInt());
                item->setFont(font);
            }
        });
        m_propLayout->addRow(new QLabel(tr("Размер шрифта ")), m_propFontSizeCombo);

        m_propTextColorButton = new QPushButton;
        auto setTextColor = [this]() {
            m_propTextColorButton->setStyleSheet("background-color: " + m_scene->selectedItem()->getFontColor().name(QColor::HexArgb) + ";\nborder: 2px solid black;");
        };
        setTextColor();
        connect(m_propTextColorButton, &QPushButton::clicked, [this, setTextColor]()
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item) {
                m_itemFontColorDialog->setCurrentColor(item->getFontColor());
                m_itemFontColorDialog->exec();
                if (m_itemFontColorDialog->result() == QColorDialog::Accepted) {
                    m_scene->selectedItem()->setFontColor(m_itemFontColorDialog->selectedColor());
                    setTextColor();
                }
            }
        });
        m_propLayout->addRow(new QLabel(tr("Цвет шрифта")), m_propTextColorButton);

        m_propBackgroundColorButton = new QPushButton;
        auto setBackgroundColor = [this]() {
            m_propBackgroundColorButton->setStyleSheet("background-color: " + m_scene->selectedItem()->getBackgroundColor().name(QColor::HexArgb) + ";\nborder: 2px solid black;");
        };
        setBackgroundColor();
        connect(m_propBackgroundColorButton, &QPushButton::clicked, [this, setBackgroundColor]()
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item) {
                m_itemBackgroundColorDialog->setCurrentColor(item->getBackgroundColor());
                m_itemBackgroundColorDialog->exec();
                if (m_itemBackgroundColorDialog->result() == QColorDialog::Accepted)
                {
                    m_scene->selectedItem()->setBackgroundColor(m_itemBackgroundColorDialog->selectedColor());
                    setBackgroundColor();
                }
            }
        });
        m_propLayout->addRow(new QLabel(tr("Цвет фона")), m_propBackgroundColorButton);

        m_propBoldCheck = new QCheckBox;
        m_propBoldCheck->setChecked(item->getFont().bold());
        connect(m_propBoldCheck, &QCheckBox::toggled, [this](bool state)
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item && state != item->getFont().bold())
            {
                QFont font = item->getFont();
                font.setBold(state);
                item->setFont(font);
            }
        });
        m_propLayout->addRow(new QLabel(tr("Жирный ")), m_propBoldCheck);
        m_propLayout->setAlignment(m_propBoldCheck, Qt::AlignHCenter);

        m_propItalicCheck = new QCheckBox;
        m_propItalicCheck->setChecked(item->getFont().italic());
        connect(m_propItalicCheck, &QCheckBox::toggled, [this](bool state)
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item && state != item->getFont().italic())
            {
                QFont font = item->getFont();
                font.setItalic(state);
                item->setFont(font);
            }
        });
        m_propLayout->addRow(new QLabel(tr("Курсив ")), m_propItalicCheck);
        m_propLayout->setAlignment(m_propItalicCheck, Qt::AlignHCenter);

        m_propUnderlineCheck = new QCheckBox;
        m_propUnderlineCheck->setChecked(item->getFont().underline());
        connect(m_propUnderlineCheck, &QCheckBox::toggled, [this](bool state)
        {
            ISceneItem *item = m_scene->selectedItem();
            if (item && state != item->getFont().underline())
            {
                QFont font = item->getFont();
                font.setUnderline(state);
                item->setFont(font);
            }
        });
        m_propLayout->addRow(new QLabel(tr("Подчеркнутый ")), m_propUnderlineCheck);
        m_propLayout->setAlignment(m_propUnderlineCheck, Qt::AlignHCenter);

        m_propBorderCheck = new QCheckBox;
        m_propBorderCheck->setChecked(item->getFrame());
        connect(m_propBorderCheck, &QCheckBox::toggled, m_scene, &EditableScene::setBorder);
        m_propLayout->addRow(new QLabel(tr("Рамка ")), m_propBorderCheck);
        m_propLayout->setAlignment(m_propBorderCheck, Qt::AlignHCenter);
    }
    return m_propWidget;
}

bool EditableSceneWidget::save()
{
    if (m_filePath.isEmpty())
        return false ;

    QDomDocument doc = makeXml();
    statusBar()->showMessage(tr("Файл успешно сохранен..."), 5000);
    return saveView(m_filePath, doc);
}

bool EditableSceneWidget::saveAs()
{
    /*QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить представление как..."),
                                                    QDir::currentPath() + "/Views",
                                                    "Файл конфигурации (*.conf)");
    if (!filePath.isEmpty()) {
        QDomDocument doc = makeXml();
        saveView(filePath, doc);
        m_filePath = filePath;
        if (!m_saveViewAction->isEnabled())
            m_saveViewAction->setEnabled(true);
        setWindowTitle(m_filePath.mid(m_filePath.lastIndexOf('/') + 1));*/
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("Имя файла"), tr("Введите имя файла: "), QLineEdit::Normal, tr("Новое представление"), &ok);
    if(ok) {
        if(fileName.isEmpty()) {
            ErrorClassifier::instance()->showError(35);
            return false;
        }
        QFile file(QCoreApplication::applicationDirPath() + QDir::separator() +
                   "Views" + QDir::separator() + fileName + ".conf");
        if (file.exists()) {
            ErrorClassifier::instance()->showError(36);
            return false;
        }
        else {
            m_filePath = file.fileName();
            if(!save())
                return false;
        }
    }
    else
        return false;
    setWindowTitle(m_filePath.mid(m_filePath.lastIndexOf('/') + 1));
    m_saveViewAction->setEnabled(true);
    statusBar()->showMessage(tr("Файл успешно сохранен..."), 5000);
    return true;
}

void EditableSceneWidget::removeItem()
{
    m_scene->removeWidget(m_scene->selectedItem());
}

void EditableSceneWidget::showItemProps()
{
    ISceneItem *item = m_scene->selectedItem();
    if (item) {
        m_editToolBar->setEnabled(true);
    } else {
        m_itemPosLabel->clear();
        m_editToolBar->setDisabled(true);
    }

    if(m_propDock->widget())
    {
        m_propDock->widget()->close();
        delete m_propWidget;
    }
    m_propDock->setWidget(createPropWidget());
    m_propWidget->show();
}
