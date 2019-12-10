#include "CommunicationProtocol.h"
#include "ReportManager/FaultReport.h"
#include "DBConfig/DBConfig.h"
#include "ProtocolDialogMapper.h"
#include "Common/PushButtonPlus.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "DatabaseDirectory/CommunicationProtocolClassifiers.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

//TODO: Если модбас, запрещать кнопку параметры

CommunicationProtocol::CommunicationProtocol(QWidget *parent) : QWidget(parent)
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

    QMap<int, QString> delegateMap;
    delegateMap[0] = "7.35 1B";
    delegateMap[1] = "Modbus";

    FKComboBoxDelegate *delegate = new FKComboBoxDelegate(delegateMap, this);

    m_view = new CustomizableTableView("CommunicationProtocol");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    m_view->setItemDelegateForColumn(1, delegate);
    //    m_view->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    //    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    connect(m_view, &QTableView::doubleClicked, this, &CommunicationProtocol::edit);
    //    connect(m_view, &QTableView::clicked, this, &CommunicationProtocol::checkEnableParametrsButton);

    m_addButton = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButton->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));
    m_paramsButton = new PushButtonPlus(QIcon(":/Images/document-config.png"), tr("Параметры"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButton);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(m_paramsButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButton, &PushButtonPlus::clicked, this, &CommunicationProtocol::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &CommunicationProtocol::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &CommunicationProtocol::edit);
    connect(m_paramsButton, &PushButtonPlus::clicked, this, &CommunicationProtocol::params);

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
    //    checkEnableParametrsButton();
}

void CommunicationProtocol::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `CommunicationProtocols` ORDER BY `code` ASC");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(0, Qt::Horizontal, tr("Код"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Протокол"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Название"));
}

void CommunicationProtocol::checkEnableParametrsButton()
{
    //    if (!m_view->currentIndex().isValid()) {
    //        m_paramsButton->setDisabled(true);
    //    }

    //    if (m_model->record(m_view->currentIndex().row()).value("protocolType") == 0) {
    //        m_paramsButton->setEnabled(true);
    //    } else {
    //        m_paramsButton->setDisabled(true);
    //    }
}

void CommunicationProtocol::edit()
{
    if (!m_view->currentIndex().isValid()) {
        return;
    }

    ProtocolDialogMapper *mapper = new ProtocolDialogMapper(m_model, true);
    connect(mapper, &ProtocolDialogMapper::dataChanged, this, &CommunicationProtocol::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void CommunicationProtocol::params()
{
    if (!m_view->currentIndex().isValid())
        return;

    int comProtocolCode = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("code").toInt();
    SapKsu::KSUProtocolType protocolType = static_cast<SapKsu::KSUProtocolType>(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("protocolType").toInt());
    CommunicationProtocolClassifiers *classifier = new CommunicationProtocolClassifiers(comProtocolCode, protocolType);
    classifier->setAttribute(Qt::WA_DeleteOnClose, true);
    classifier->show();
}

void CommunicationProtocol::add()
{
    ProtocolDialogMapper *mapper = new ProtocolDialogMapper(m_model);
    connect(mapper, &ProtocolDialogMapper::dataChanged, this, &CommunicationProtocol::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void CommunicationProtocol::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление записи");
    //    msgBox.setText(tr("Вы действительно хотите удалить запись?"));
    QSqlRecord &&record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());
    QString msgString = tr("Вы действительно хотите удалить протокол \"%1\" \r\nс кодом %2?")
                        .arg(record.value("name").toString())
                        .arg(record.value("code").toString());

    msgBox.setText(msgString);
    msgBox.setInformativeText(tr("Удаление протокола приведет к удалению всех связанных с ним справочников"));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {

        QSqlDatabase db = DBConfig::instance()->configDatabase();
        QSqlQuery query(db);


        QString sqlString = QString("DELETE FROM `CommunicationProtocols`"
                                    "WHERE code = \"%1\";")
                            .arg(m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row()).value("code").toString());

        if (!query.exec(sqlString)) {
            if (query.lastError().text().contains(QLatin1String("FOREIGN KEY"), Qt::CaseInsensitive)) {
                ErrorClassifier::instance()->showError(5);
                FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(5));
            } else {
                ErrorClassifier::instance()->showError(501);
                FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(501));
            }
        }
        selectData();
    }
}
