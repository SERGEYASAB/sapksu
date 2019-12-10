#include "ParameterAliasSelectWidget.h"
#include "ParameterAliasSelectModel.h"
#include "DatabaseDirectory/ParametrAliases/ParametrAliasesDialogMapper.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

ParameterAliasSelectWidget::ParameterAliasSelectWidget(const QString &text, QWidget *parent) : QWidget(parent)
{
    // Соблюдай логику последовательности действий
    // Создал обьект, Настроил обьект, Связал обьект, Разместил объект

    // 1) Создали модель
    m_aliasModel = new ParameterAliasSelectModel(this);
    resetQuery(); // (headerData метод я убрал, это проще задавать методом setHeaderData)

    // 2) Создали прокси модель и скормили ей модельку
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_aliasModel);
    
    // 3) Создали представление
    // 4) Вызвали все настройки
    // 5) Передали представлению модель
    // 6) Сделали соединения
    m_aliasView = new QTableView(this);
    m_aliasView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_aliasView->setCurrentIndex(m_proxyModel->mapFromSource(m_aliasModel->findItem(text)));
    m_aliasView->setModel(m_proxyModel);

    m_aliasView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_aliasView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


    connect(m_aliasView, &QListView::clicked, this, &ParameterAliasSelectWidget::changeControlsBehavior);
    connect(m_aliasView, &QListView::doubleClicked, this, &ParameterAliasSelectWidget::selectItem);

    // 7) Создал все Кнопки и настроил соединения
    QSize iconSize = QSize(32, 32);
    m_okButton = new PushButtonPlus(QIcon(":/Images/call-start.png"), tr("Ок"), this);
    m_okButton->setIconSize(iconSize);
    m_addButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"), this);
    m_addButton->setIconSize(iconSize);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"), this);
    m_removeButton->setIconSize(iconSize);
    m_editButton = new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));
    m_editButton->setIconSize(iconSize);
    m_cancelButton = new PushButtonPlus(QIcon(":/Images/call-stop.png"), tr("Закрыть"), this);
    m_cancelButton->setIconSize(iconSize);

    connect(m_okButton, &PushButtonPlus::clicked, this, &ParameterAliasSelectWidget::selectItem);
    connect(m_addButton, &PushButtonPlus::clicked, this, &ParameterAliasSelectWidget::addItem);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &ParameterAliasSelectWidget::removeItem);
    connect(m_editButton, &PushButtonPlus::clicked, this, &ParameterAliasSelectWidget::editItem);
    connect(m_cancelButton, &PushButtonPlus::clicked, this, &ParameterAliasSelectWidget::close);

    // 8) это дерьмо мне не понятно, но по логике его надо вызывать после баттонов
    changeControlsBehavior();

    // 9) Тут группа объектов для строки поиска

    m_filterColumnCombo = new QComboBox(this);
    m_filterColumnCombo->addItem(tr("По псевдониму"));
    m_filterColumnCombo->addItem(tr("По описанию"));

    connect(m_filterColumnCombo,    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,                   &ParameterAliasSelectWidget::setupFilter);

    m_filterEdit = new QLineEdit();

    //ToDo: Если ставить фильтр по Enter, в слоте необходимо проверять наличие фокуса
    //Дабы исключить дополнительную фильтрацию при потере фокуса
    connect(m_filterEdit, &QLineEdit::textChanged, this, &ParameterAliasSelectWidget::setupFilter);


    m_searchIconLabel = new QLabel(this);
    m_searchIconLabel->setPixmap(QPixmap(":/Images/search.png").scaled(16,
                                                                       16,
                                                                       Qt::KeepAspectRatio,
                                                                       Qt::SmoothTransformation));

    // 10) Все размещение в конец, это очень удобно! Можно добавить отступы и прочиие настройки!
    // Сразу видно кто кому принадлежит и в какой последовательности добавляются
    // Даже ui не нужен, в головном форма рисуется сама

    m_vboxButtonLayout = new QVBoxLayout();
    m_vboxButtonLayout->addWidget(m_okButton);
    m_vboxButtonLayout->addWidget(m_addButton);
    m_vboxButtonLayout->addWidget(m_removeButton);
    m_vboxButtonLayout->addWidget(m_editButton);
    m_vboxButtonLayout->addWidget(m_cancelButton);
    m_vboxButtonLayout->addStretch();
    m_vboxButtonLayout->addWidget(m_filterColumnCombo);

    m_hboxFilterLayout = new QHBoxLayout();
    m_hboxFilterLayout->addWidget(new QLabel(tr("Фильтр: ")));
    m_hboxFilterLayout->addWidget(m_filterEdit);
    m_hboxFilterLayout->addWidget(m_searchIconLabel);




    m_vboxAliasListLayout = new QVBoxLayout();
    m_vboxAliasListLayout->addWidget(m_aliasView);
    m_vboxAliasListLayout->addSpacing(10);
    m_vboxAliasListLayout->addLayout(m_hboxFilterLayout);

    m_hboxMainLayout = new QHBoxLayout(this);
    m_hboxMainLayout->addLayout(m_vboxAliasListLayout);
    m_hboxMainLayout->addSpacing(5);
    m_hboxMainLayout->addLayout(m_vboxButtonLayout);

    setLayout(m_hboxMainLayout);
    resize(800, 600);
}

QTableView *ParameterAliasSelectWidget::getView()
{
    return m_aliasView;
}

void ParameterAliasSelectWidget::selectItem()
{
    if (m_aliasView->selectionModel()->selectedRows().count() > 0) {
        emit itemSelected(m_proxyModel->index(m_aliasView->currentIndex().row(), 0).data().toString());
        close();
    }
    else
        ErrorClassifier::instance()->showError(4);
}

void ParameterAliasSelectWidget::addItem()
{
    ParametrAliasesDialogMapper *dialog = new ParametrAliasesDialogMapper(m_aliasModel);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(dialog, &ParametrAliasesDialogMapper::dataChanged, this, &ParameterAliasSelectWidget::resetQuery);
    dialog->show();
}

void ParameterAliasSelectWidget::removeItem()
{
    if (!m_aliasView->currentIndex().isValid()) {
        ErrorClassifier::instance()->showError(4);
        return;
    }
    
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Удаление псевдонимов параметров (alias)"));
    msgBox.setText(tr("Вы действительно хотите удалить выбранные псевдонимы?") +
                   m_aliasView->currentIndex().data().toString() + "?");
    msgBox.setIcon(QMessageBox::Question);
    
    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == yesButton) {
        for (auto i : m_aliasView->selectionModel()->selectedIndexes()) {
            QSqlQuery(QString("DELETE FROM `ParametrAliases`"
                              "WHERE alias = '%1';")
                      .arg(i.data().toString()),
                      DBConfig::instance()->configDatabase()).exec();
        }
        resetQuery();
    }
}

void ParameterAliasSelectWidget::editItem()
{
    if (!m_aliasView->currentIndex().isValid()) {
        ErrorClassifier::instance()->showError(4);
        return;
    }
    
    ParametrAliasesDialogMapper *dialog = new ParametrAliasesDialogMapper(m_aliasModel);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(dialog, &ParametrAliasesDialogMapper::dataChanged, this, &ParameterAliasSelectWidget::resetQuery);
    dialog->setCurrentModelIndex(m_proxyModel->mapToSource(m_aliasView->currentIndex()));
    dialog->show();
}

void ParameterAliasSelectWidget::setupButtons(ParameterAliasSelectWidget::Buttons buttons)
{
    m_okButton->setVisible(buttons & OkButton);
    m_addButton->setVisible(buttons & AddButton);
    m_removeButton->setVisible(buttons & RemoveButton);
    m_editButton->setVisible(buttons & EditButton);
    m_cancelButton->setVisible(buttons &CancelButton);
}

void ParameterAliasSelectWidget::resetQuery()
{
    m_aliasModel->setQuery(QString("SELECT * FROM `ParametrAliases` ORDER BY `id` ASC"),
                           DBConfig::instance()->configDatabase());

    m_aliasModel->setHeaderData(0, Qt::Horizontal, tr("Псевдоним параметра"));
    m_aliasModel->setHeaderData(1, Qt::Horizontal, tr("Описание"));
}

void ParameterAliasSelectWidget::setupFilter()
{
    m_proxyModel->setFilterKeyColumn(m_filterColumnCombo->currentIndex());
    m_proxyModel->setFilterRegExp(QRegExp(m_filterEdit->text(), Qt::CaseInsensitive));
}

void ParameterAliasSelectWidget::changeControlsBehavior()
{
    QModelIndex index = m_aliasView->currentIndex();
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);

    bool editAndDeleteIsActive = false;
    if (sourceIndex.isValid()) {
        QSqlRecord &&record = m_aliasModel->record(sourceIndex.row());
        if (!record.isNull(QStringLiteral("flag"))) {
            editAndDeleteIsActive = !record.value(QStringLiteral("flag")).toBool();
        }
    }

    m_editButton->setEnabled(editAndDeleteIsActive);
    m_removeButton->setEnabled(editAndDeleteIsActive);
}
