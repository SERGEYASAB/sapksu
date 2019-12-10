#include "InputDialogMapperInterface.h"

InputDialogMapperInterface::InputDialogMapperInterface(QSqlQueryModel *model, QWidget *parent) :
    QWidget(parent), m_model(model)
{
    m_code = new QLabel(tr("&Код"));
    m_description = new QLabel(tr("&Описание"));

    m_codeLineEdit = new QLineEdit;
    m_codeLineEdit->setValidator(new QIntValidator(0, 0x7FFFFFFF, m_codeLineEdit));
    m_descriptionLineEdit = new QLineEdit;

    m_code->setBuddy(m_codeLineEdit);
    m_description->setBuddy(m_descriptionLineEdit);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_codeLineEdit, 1);
    m_mapper->addMapping(m_descriptionLineEdit, 2);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_code, m_codeLineEdit);
    formLayout->addRow(m_description, m_descriptionLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &InputDialogMapperInterface::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &InputDialogMapperInterface::close);

    vlayout->addLayout(formLayout);
    vlayout->addWidget(buttonBox);

    setLayout(vlayout);

    resize(400, 150);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Параметры"));
}

InputDialogMapperInterface::~InputDialogMapperInterface()
{

}

void InputDialogMapperInterface::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
}
