#include "TestsDialogMapper.h"

#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "Common/FKProxyDelegate/FKProxyDelegate.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"
#include "ReportManager/FaultReport.h"

TestsDialogMapper::TestsDialogMapper(QSqlQueryModel *model, QWidget *parent) : QWidget(parent)
{
    m_id = new QLabel(tr("&id"));
    m_name = new QLabel(tr("&Название"));
    m_description = new QLabel(tr("&Описание"));
    m_path = new QLabel(tr("&Путь к конфигурации"));
    m_buttonType = new QLabel(tr("&Наличие кнопки"));
    m_protocolType = new QLabel(tr("&Протокол обмена"));

    m_nameLineEdit = new QLineEdit();
    m_descriptionLineEdit = new QLineEdit();
    m_pathLineEdit = new QLineEdit();
    m_pathLineEdit->setDisabled(true);

    m_buttonTypeComboBox = new QComboBox();
    m_buttonTypeComboBox->insertItem(0, "Без кнопки");
    m_buttonTypeComboBox->insertItem(1, "С кнопкой");
    m_buttonTypeComboBox->insertItem(2, "Универсальный");

    m_protocolTypeComboBox = new QComboBox();
    m_protocolTypeComboBox->insertItem(0, "KSU 7.35");
    m_protocolTypeComboBox->insertItem(1, "Modbus");
    m_protocolTypeComboBox->insertItem(2, "Универсальный");

    m_name->setBuddy(m_nameLineEdit);
    m_path->setBuddy(m_pathLineEdit);
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
    proxyDelegate->setDelegateForColumn(4, buttonDelegate);
    proxyDelegate->setDelegateForColumn(5, protocolDelegate);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_nameLineEdit, 1);
    m_mapper->addMapping(m_descriptionLineEdit, 2);
    m_mapper->addMapping(m_pathLineEdit, 3);
    m_mapper->addMapping(m_buttonTypeComboBox, 4);
    m_mapper->addMapping(m_protocolTypeComboBox, 5);
    m_mapper->setItemDelegate(proxyDelegate);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_name, m_nameLineEdit);
    formLayout->addRow(m_description, m_descriptionLineEdit);
#ifndef QT_NO_DEBUG
    formLayout->addRow(m_path, m_pathLineEdit);
#endif
    formLayout->addRow(m_buttonType, m_buttonTypeComboBox);
    formLayout->addRow(m_protocolType, m_protocolTypeComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &TestsDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &TestsDialogMapper::close);

    vlayout->addLayout(formLayout);
    vlayout->addWidget(buttonBox);

    setLayout(vlayout);

    resize(450, 150);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Редактор параметров"));
}

void TestsDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}

void TestsDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    QString sqlString;

    if (m_mapper->currentIndex() == -1) {
        sqlString = QString("INSERT INTO `Tests` (`name`, `description`, `path`, `buttonType`, `protocolType`)"
                            "VALUES ('%1', '%2', '%3', %4, %5);")
                    .arg(m_nameLineEdit->text())
                    .arg(m_descriptionLineEdit->text())
                    .arg(QString(QCryptographicHash::hash((m_nameLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex()))
                    .arg(m_buttonTypeComboBox->currentIndex())
                    .arg(m_protocolTypeComboBox->currentIndex());

        copyTemplate();

    } else {

        qDebug() << "// Переименование старого файла";

        QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "DBConfig" + QDir::separator() + "Tests");

        QString oldName = dir.absoluteFilePath(static_cast<QSqlQueryModel *>
                                               (m_mapper->model())->record(m_mapper->currentIndex()).value("path").toString());
        QString newName = dir.absoluteFilePath
                          (QString(QCryptographicHash::hash
                                   ((m_nameLineEdit->text().toLocal8Bit().constData()),
                                    QCryptographicHash::Md5).toHex()));
        QFile::rename(oldName, newName);


        sqlString = QString("UPDATE `Tests`"
                            "SET name = '%1', description = '%2', path = '%3', buttonType = %4, protocolType = %5 "
                            "WHERE name = '%6';")
                    .arg(m_nameLineEdit->text())
                    .arg(m_descriptionLineEdit->text())
                    .arg(QString(QCryptographicHash::hash((m_nameLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex()))
                    .arg(m_buttonTypeComboBox->currentIndex())
                    .arg(m_protocolTypeComboBox->currentIndex())
                    .arg(static_cast<QSqlQueryModel *>(m_mapper->model())->record(m_mapper->currentIndex()).value("name").toString());
    }

    QSqlQuery query(db);

    if (query.exec(sqlString) == false)
    {
        ErrorClassifier::instance()->showError(44);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(44) + query.lastError().databaseText());
    } else {

        emit dataChanged();

        close();
    }
}

void TestsDialogMapper::copyTemplate()
{
    QDir dir(QCoreApplication::applicationDirPath() + QDir::separator() + "DBConfig" + QDir::separator() + "Tests");
    dir.mkpath(dir.path());
    QString path = dir.absoluteFilePath(QString(QCryptographicHash::hash((m_nameLineEdit->text().toLocal8Bit().constData()), QCryptographicHash::Md5).toHex()));

    QFile::copy(":/DBConfig/TestExample.xml", path);

    QFile::setPermissions(path,
                        QFile::ReadOwner |
                        QFile::WriteOwner |
                        QFileDevice::WriteOther |
                        QFileDevice::ReadOther);
}
