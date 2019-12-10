#include "SAPParametrs.h"
#include "SAPParametrsDialogMapper.h"
#include "DBConfig/DBConfig.h"
#include "Common/QSyntaxHighlighter/XMLSyntaxHighlighter/XMLSyntaxHighlighter.h"
#include "Common/CustomizableTableView/CustomizableTableView.h"
#include "Common/SearchProxyModelWidget/SearchProxyModelWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

SAPParametrs::SAPParametrs(QWidget *parent)
{
    m_model = new QSqlQueryModel(this);
    selectData();

    SearchProxyModelWidget *searchWidget = new SearchProxyModelWidget();
    searchWidget->setSourceModel(m_model);
    searchWidget->setDefaultParams(QRegExp::Wildcard, 2);
    m_proxyModel = searchWidget->getProxyModel();

    QGroupBox *searchBox = new QGroupBox;
    searchBox->setTitle(tr("Поиск и фильтрация"));
    searchBox->setLayout(searchWidget->layout());

    m_view = new CustomizableTableView("SAPParametrs");
    m_view->setModel(m_proxyModel);
    m_view->setSortingEnabled(true);
    //    m_view->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    //    m_view->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    //    m_view->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    //    m_view->horizontalHeader()->setStretchLastSection(true);
    m_view->horizontalHeader()->hideSection(0);
    m_view->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_view->selectRow(0);

    connect(m_view, &QTableView::doubleClicked, this, &SAPParametrs::edit);

    m_addButon = new PushButtonPlus(QIcon(":/Images/list-add.png"), tr("Добавить"));
    m_addButon->setDefault(true);
    m_removeButton = new PushButtonPlus(QIcon(":/Images/list-remove.png"), tr("Удалить"));
    m_editButton= new PushButtonPlus(QIcon(":/Images/edit-all.png"), tr("Редактировать"));
    m_paramsButton = new PushButtonPlus(QIcon(":/Images/document-config.png"), tr("Параметры"));

    buttonBox = new QVBoxLayout;
    buttonBox->addWidget(m_addButon);
    buttonBox->addWidget(m_removeButton);
    buttonBox->addWidget(m_editButton);
    buttonBox->addWidget(m_paramsButton);
    buttonBox->addWidget(searchBox);
    buttonBox->addStretch();

    connect(m_addButon, &PushButtonPlus::clicked, this, &SAPParametrs::add);
    connect(m_removeButton, &PushButtonPlus::clicked, this, &SAPParametrs::remove);
    connect(m_editButton, &PushButtonPlus::clicked, this, &SAPParametrs::edit);
    connect(m_paramsButton, &PushButtonPlus::clicked, this, &SAPParametrs::params);

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

void SAPParametrs::selectData()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString = QString("SELECT * FROM `SAPParametrs` ORDER BY `code` ASC");
    QSqlQuery query(sqlString, db);

    m_model->setQuery(query);
    m_model->setHeaderData(0, Qt::Horizontal, tr("id"));
    m_model->setHeaderData(1, Qt::Horizontal, tr("Код"));
    m_model->setHeaderData(2, Qt::Horizontal, tr("Название"));
    m_model->setHeaderData(3, Qt::Horizontal, tr("Описание"));
    m_model->setHeaderData(4, Qt::Horizontal, tr("Путь конфигурации"));
}

void SAPParametrs::params()
{
    if (!m_view->currentIndex().isValid())
        return;

    QSqlRecord record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());

    if (record.isEmpty())
        return;

    QString parametrsXMLFileName = record.value("parametrsConfigPath").toString();
    if (parametrsXMLFileName.isEmpty())
        return;

    QString parametrsXMLFilePath = DBConfig::sapParametrsFolderPath() + QDir::separator() +
                                   parametrsXMLFileName;

    QFile file(parametrsXMLFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QByteArray fileContent = file.readAll();

    file.close();

    QTextEdit *parametrsTextEditor =  new QTextEdit;
    XMLSyntaxHighlighter * highlighter = new XMLSyntaxHighlighter(parametrsTextEditor);
    Q_UNUSED(highlighter);
    parametrsTextEditor->setPlainText(fileContent);
    parametrsTextEditor->setReadOnly(true);
    parametrsTextEditor->setAttribute(Qt::WA_DeleteOnClose, true);
    parametrsTextEditor->setWindowModality(Qt::ApplicationModal);
    parametrsTextEditor->resize(1024, 512);
    parametrsTextEditor->setWindowIcon(QIcon(":/Images/xml-green.png"));

    QRect screen  = QApplication::desktop()->screenGeometry();
    parametrsTextEditor->move(screen.center() - rect().center());
    parametrsTextEditor->show();
}

void SAPParametrs::add()
{
    SAPParametrsDialogMapper *mapper = new SAPParametrsDialogMapper(m_model);
    connect(mapper, &SAPParametrsDialogMapper::dataChanged, this, &SAPParametrs::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(QModelIndex());
    mapper->show();
}

void SAPParametrs::edit()
{
    SAPParametrsDialogMapper *mapper = new SAPParametrsDialogMapper(m_model, true);
    connect(mapper, &SAPParametrsDialogMapper::dataChanged, this, &SAPParametrs::selectData);
    mapper->setAttribute(Qt::WA_DeleteOnClose, true);
    mapper->setCurrentModelIndex(m_proxyModel->mapToSource(m_view->currentIndex()));
    mapper->show();
}

void SAPParametrs::remove()
{
    if (!m_view->currentIndex().isValid())
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QSqlRecord record = m_model->record(m_proxyModel->mapToSource(m_view->currentIndex()).row());

    QString sqlString = QString("SELECT t.ksuName "
                                "FROM `KsuTypes` as t "
                                "INNER JOIN SAPParametrs as p ON t.sapParamId = p.id "
                                "WHERE p.id = %1;")
                        .arg(record.value("id").toInt());

    QString relatedKsu = QString();

    bool hasKsuTypes = false;
    if (query.exec(sqlString)) {
        QStringList ksuNames;

        while (query.next()) {
            hasKsuTypes = true;
            ksuNames.append(query.value(0).toString());
            //            QString relatedKsu = relatedKsu + query.value(1).toString();
        }

        relatedKsu = ksuNames.join(QStringLiteral("\r\n"));
    }

    if (hasKsuTypes) {
        ErrorClassifier::instance()->setErrorDescription(27, QString("Удаление шаблона САП параметров невозможно, "
                                                                     "т.к. к нему привязаны следующие типы КСУ: \r\n%1").arg(relatedKsu));
        ErrorClassifier::instance()->showError(27);
        return;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Удаление шаблона САП параметров");

    QString msgString = tr("Вы действительно хотите удалить шаблон \"%1\" \r\nс кодом %2?")
                        .arg(record.value("name").toString())
                        .arg(record.value("code").toString());

    msgBox.setText(msgString);
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(tr("&Отмена"), QMessageBox::NoRole);
    msgBox.setDefaultButton(noButton);

    msgBox.exec();


    if (msgBox.clickedButton() == yesButton) {

        QFile file(DBConfig::sapParametrsFolderPath() + QDir::separator() +
                   record.value("parametrsConfigPath").toString());

        QString sqlString = QString("DELETE FROM `SAPParametrs`"
                                    "WHERE code = \"%1\";")
                            .arg(record.value("code").toString());

        if (query.exec(sqlString)) {
            file.remove();
        }
        selectData();
    }
}
