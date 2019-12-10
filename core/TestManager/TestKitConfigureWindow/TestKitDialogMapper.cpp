#include "TestKitDialogMapper.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "Common/FKProxyDelegate/FKProxyDelegate.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

TestKitDialogMapper::TestKitDialogMapper(QSqlQueryModel *model, QWidget *parent) : QWidget(parent)
{
    m_id = new QLabel(tr("&id"));
    m_name = new QLabel(tr("&Название"));
    m_description = new QLabel(tr("&Описание"));
    m_buttonType = new QLabel(tr("&Наличие кнопки"));
    m_protocolType = new QLabel(tr("&Протокол обмена"));

    m_nameLineEdit = new QLineEdit();
    m_descriptionLineEdit = new QLineEdit();

    m_buttonTypeComboBox = new QComboBox();
    m_buttonTypeComboBox->insertItem(0, "Без кнопки");
    m_buttonTypeComboBox->insertItem(1, "С кнопкой");
    m_buttonTypeComboBox->insertItem(2, "Универсальный");

    m_protocolTypeComboBox = new QComboBox();
    m_protocolTypeComboBox->insertItem(0, "KSU 7.35");
    m_protocolTypeComboBox->insertItem(1, "Modbus");
    m_protocolTypeComboBox->insertItem(2, "Универсальный");

    m_name->setBuddy(m_nameLineEdit);
    m_description->setBuddy(m_descriptionLineEdit);
    m_buttonType->setBuddy(m_buttonTypeComboBox);
    m_protocolType->setBuddy(m_protocolTypeComboBox);


    QMap<int, QString> buttonDelegateMap;
    buttonDelegateMap[0] = "Без кнопки";
    buttonDelegateMap[1] = "С кнопкой";
    buttonDelegateMap[2] = "Универсальный";

    FKComboBoxDelegate *buttonDelegate = new FKComboBoxDelegate(buttonDelegateMap, this);

    QMap<int, QString> protocolDelegateMap;
    protocolDelegateMap[0] = "KSU 7.35";
    protocolDelegateMap[1] = "Modbus";
    protocolDelegateMap[2] = "Универсальный";

    FKComboBoxDelegate *protocolDelegate = new FKComboBoxDelegate(protocolDelegateMap, this);

    FKProxyDelegate *proxyDelegate = new FKProxyDelegate(this);
    proxyDelegate->setDelegateForColumn(3, buttonDelegate);
    proxyDelegate->setDelegateForColumn(4, protocolDelegate);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_nameLineEdit, 1);
    m_mapper->addMapping(m_descriptionLineEdit, 2);
    m_mapper->addMapping(m_buttonTypeComboBox, 3);
    m_mapper->addMapping(m_protocolTypeComboBox, 4);
    m_mapper->setItemDelegate(proxyDelegate);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_name, m_nameLineEdit);
    formLayout->addRow(m_description, m_descriptionLineEdit);
    formLayout->addRow(m_buttonType, m_buttonTypeComboBox);
    formLayout->addRow(m_protocolType, m_protocolTypeComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &TestKitDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &TestKitDialogMapper::close);

    vlayout->addLayout(formLayout);
    vlayout->addWidget(buttonBox);

    setLayout(vlayout);

    resize(400, 150);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Редактор параметров"));
}

void TestKitDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}

void TestKitDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString;

    if (m_mapper->currentIndex() == -1) {
        sqlString = QString("INSERT INTO `TestKit` (`name`, `description`, `buttonType`, `protocolType`)"
                            "VALUES ('%1', '%2', '%3', %4);")
                    .arg(m_nameLineEdit->text())
                    .arg(m_descriptionLineEdit->text())
                    .arg(m_buttonTypeComboBox->currentIndex())
                    .arg(m_protocolTypeComboBox->currentIndex());
    } else {
        sqlString = QString("UPDATE `TestKit`"
                            "SET name = '%1', description = '%2', buttonType = %3, protocolType = %4 "
                            "WHERE name = '%5';")
                    .arg(m_nameLineEdit->text())
                    .arg(m_descriptionLineEdit->text())
                    .arg(m_buttonTypeComboBox->currentIndex())
                    .arg(m_protocolTypeComboBox->currentIndex())
                    .arg(static_cast<QSqlQueryModel *>(m_mapper->model())->record(m_mapper->currentIndex()).value("name").toString());
    }

    QSqlQuery query(db);

    if (query.exec(sqlString) == false)
    {
        ErrorClassifier::instance()->showError(43);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(43) + query.lastError().databaseText());
    } else {

        emit dataChanged();

        close();
    } 
}
