#include "KsuTypes.h"

#include "DBConfig/DBConfig.h"
#include "KsuTypesDialogMapper.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

KsuTypes::KsuTypes(QWidget *parent) : QWidget(parent)
{
    m_model = new QSqlQueryModel;
    selectData();

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("KsuTypes");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    QHeaderView *header = m_view->horizontalHeader();
    header->hideSection(0);
    header->setSectionHidden(4, true);

    connect(m_view, &QTableView::doubleClicked, this, &KsuTypes::edit);

    m_addButon = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButon->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButon);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButon, &PushButtonPlus::clicked, this, &KsuTypes::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &KsuTypes::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &KsuTypes::edit);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_view);
    mainLayout->addLayout(buttonBox);
    setLayout(mainLayout);
    resize(1024, 768);
    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowTitle(tr("Справочник протоколов"));
}

void KsuTypes::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT t.*, p.name "
                                "   FROM `KsuTypes` as t "
                                "   INNER JOIN SAPParametrs as p "
                                "       ON t.sapParamId = p.id "
                                "ORDER BY `code` ASC");

    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(1, Qt::Horizontal, tr("Код"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Тип КСУ"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Завод-изготовитель"));
    m_model->setHeaderData(5, Qt::Horizontal, tr("Параметры САП"));
}


void KsuTypes::edit()
{
    if (!m_view->currentIndex().isValid())
        return;

    bool ok;
    int ksuTypeId = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value(QStringLiteral("id")).toInt(&ok);
    if (!ok)
        return;

    KsuTypesDialogMapper *mapper = new KsuTypesDialogMapper(m_model);
    connect(mapper, &KsuTypesDialogMapper::dataChanged, this, &KsuTypes::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setKsuTypeId(ksuTypeId);
    mapper->show();
}

void KsuTypes::add()
{
    KsuTypesDialogMapper *mapper = new KsuTypesDialogMapper(m_model);
    connect(mapper, &KsuTypesDialogMapper::dataChanged, this, &KsuTypes::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->show();
}

void KsuTypes::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());

    QString sqlString = QString("SELECT kt.ksuName as ksuName, ksv.softwareVersion as version "
                                "FROM `KsuTypes` as kt "
                                "INNER JOIN KsuSoftwareVersions as ksv ON kt.id = ksv.ksuTypeId "
                                "WHERE kt.id = %1;")
            .arg(record.value("id").toInt());

    QString relatedSoftwareVersions = QString();

    bool hasSoftwareVersions = false;
    if (query.exec(sqlString)) {
        QStringList softwareVersionsList;

        while (query.next()) {
            hasSoftwareVersions = true;
            softwareVersionsList.append(query.value("ksuName").toString() + " \"" + query.value("version").toString() + "\"");
            //            QString relatedKsu = relatedKsu + query.value(1).toString();
        }

        relatedSoftwareVersions = softwareVersionsList.join(QStringLiteral("\r\n"));
    }

    if (hasSoftwareVersions) {

        ErrorClassifier::instance()->setErrorDescription(22, QString("Удаление типа КСУ невозможно, "
                               "т.к. к нему привязаны следующие версии ПО КСУ: \r\n%1").arg(relatedSoftwareVersions));
        ErrorClassifier::instance()->showError(22);
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QString msgString = tr("Вы действительно хотите удалить КСУ \"%1\" \r\nс кодом %2?")
            .arg(record.value("ksuName").toString())
            .arg(record.value("code").toString());

    msgBox.setText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `KsuTypes`"
                                    "WHERE id = %1;")
                .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("id").toString());

        query.exec(sqlString);
        selectData();
    }
}
