#include "ReturnFlags10DialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

ReturnFlags10DialogMapper::ReturnFlags10DialogMapper(int comProtocolCode, QAbstractItemModel *model, QWidget *parent)
    : QWidget(parent)
{
    m_comProtocolCode = comProtocolCode;

    m_code = new QLabel(tr("&Код"));
    m_name = new QLabel(tr("&Описание"));
    m_format = new QLabel(tr("&Тип"));
    m_alias = new QLabel(tr("&Псевдоним"));

    m_codeLineEdit = new QLineEdit();
    m_codeLineEdit->setValidator(new QIntValidator(0, 0x7FFFFFFF, m_codeLineEdit));
    m_codeLineEdit->setEnabled(false);

    m_nameLineEdit = new QLineEdit();

    m_formatComboBox = new QComboBox();
    m_formatComboBox->insertItem(0, "XXXX");
    m_formatComboBox->insertItem(1, "XXX.X");
    m_formatComboBox->insertItem(2, "XX.XX");
    m_formatComboBox->insertItem(3, "X.XXX");
    m_formatComboBox->insertItem(4, ".XXXX");
    m_formatComboBox->insertItem(5, "XXSS");

    m_aliasLineEdit = new QLineEdit();

    m_code->setBuddy(m_codeLineEdit);
    m_format->setBuddy(m_formatComboBox);
    m_name->setBuddy(m_nameLineEdit);
    m_alias->setBuddy(m_aliasLineEdit);

    QMap<int, QString> delegateMap;
    delegateMap[0] = "XXXX";
    delegateMap[1] = "XXX.X";
    delegateMap[2] = "XX.XX";
    delegateMap[3] = "X.XXX";
    delegateMap[4] = ".XXXX";
    delegateMap[5] = "XXSS";

    FKComboBoxDelegate *formatDelegate = new FKComboBoxDelegate(delegateMap, 3, this);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_codeLineEdit, 1);
    m_mapper->addMapping(m_nameLineEdit, 2);
    m_mapper->addMapping(m_formatComboBox, 3);
    m_mapper->addMapping(m_aliasLineEdit, 4);
    m_mapper->setItemDelegate(formatDelegate);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_code, m_codeLineEdit);
    formLayout->addRow(m_format, m_formatComboBox);
    formLayout->addRow(m_name, m_nameLineEdit);
    formLayout->addRow(m_alias, m_aliasLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &ReturnFlags10DialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &ReturnFlags10DialogMapper::close);

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

void ReturnFlags10DialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}

void ReturnFlags10DialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();

    if (m_mapper->currentIndex() == -1) {

        QString sqlString = QString("INSERT INTO `Return0x10FunctionParametrsFlags` \r\n"
                                    "(`comProtocolCode`, `code`, `parametrName`, `format`, `alias`) \r\n"
                                    "VALUES (%1, %2, '%3', %4, '%5');")
                            .arg(m_comProtocolCode)
                            .arg(m_codeLineEdit->text())
                            .arg(m_nameLineEdit->text())
                            .arg(m_formatComboBox->currentIndex())
                            .arg(m_aliasLineEdit->text());

        QSqlQuery query(db);

        if (query.exec(sqlString) == false)
        {
            ErrorClassifier::instance()->showError(14);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(14) + query.lastError().databaseText());
        } else {
            emit dataChanged();
            close();
        }
    } else {
        QString sqlString = QString("UPDATE `Return0x10FunctionParametrsFlags` \r\n"
                                    "SET parametrName = '%1', \r\n"
                                    "format = %2, \r\n"
                                    "alias = '%3' \r\n"
                                    "WHERE code = %4 AND comProtocolCode = %5;")
                            .arg(m_nameLineEdit->text())
                            .arg(m_formatComboBox->currentIndex())
                            .arg(m_aliasLineEdit->text())
                            .arg(static_cast<QSqlQueryModel *>(m_mapper->model())->record(m_mapper->currentIndex()).value("code").toString())
                            .arg(m_comProtocolCode);

        QSqlQuery query(db);

        if (query.exec(sqlString) == false)
        {
            ErrorClassifier::instance()->showError(14);
            FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(14) + query.lastError().databaseText());
        } else {
            emit dataChanged();
            close();
        }
    }
}
