#include "ModbusVariableForm.h"
#include "ModbusVariableEditBitsetValue.h"
#include "ReportManager/FaultReport.h"
#include "Common/ParametrAliases/ParameterAliasWidget.h"
#include "ErrorClassifier/ErrorClassifier.h"

static const QString variableNodeTagName        = "IVariable";
static const QString variableNodeElementId      = "ModbusVariable";

ModbusVariableForm::ModbusVariableForm(QDomNode node, QWidget *parent) :
    QWidget(parent)
{
    setupUi();
    fillDataTypes();
    setVariableStrategy(0);

    m_enumStrategyValuesModel = new QStandardItemModel(this);
    m_bitsetStrategyValuesModel = new QStandardItemModel(this);

    extendedParametrsTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    extendedParametrsTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    QDomElement element = node.toElement();
    if (!element.isNull() &&
        element.tagName() == variableNodeTagName &&
        element.attribute(QStringLiteral("id")) == variableNodeElementId) {
        m_variableElement = element;

        traverseXMLElement(m_variableElement);
    } else {
        m_parentNode = node;
        m_variableElement = node.ownerDocument().createElement(variableNodeTagName);
    }
}

void ModbusVariableForm::setVariableStrategy(int type)
{
    m_strategyType = type;

    linearParametrsWidget->setVisible(type == 1);
    registerParametrsWidget->setVisible(type == 2 || type == 3);
    enumeratedParametrsWidget->setVisible(type == 3 || type == 4);

    bool minMaxVisible = type != 3;
    minimumLabel->setVisible(minMaxVisible);
    minimumLineEdit->setVisible(minMaxVisible);
    maximumLabel->setVisible(minMaxVisible);
    maximumLineEdit->setVisible(minMaxVisible);
    defaultValueLabel->setVisible(minMaxVisible);
    defaultValueLineEdit->setVisible(minMaxVisible);

    bool dataTypesEnabled = type == 0 || type == 1;

    QStandardItemModel *deviceDataTypeComboBoxModel = qobject_cast<QStandardItemModel *>(deviceDataTypeComboBox->model());
    if (deviceDataTypeComboBoxModel) {
        for (int i : {0, 9, 10}) {
            QStandardItem *item = deviceDataTypeComboBoxModel->item(i);
            if (dataTypesEnabled) {
                item->setFlags(item->flags() | (Qt::ItemIsSelectable | Qt::ItemIsEnabled));
            } else {
                item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
            }

            if (deviceDataTypeComboBox->currentIndex() == i && !dataTypesEnabled) {
                deviceDataTypeComboBox->setCurrentIndex(4);
            }
        }
    }


    bool mbKsuDataTypesEnabled = type == 0;
    for (auto comboBox : {deviceDataTypeComboBox, serverDataTypeComboBox}) {
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(comboBox->model());
        if (!model)
            continue;

        for (int i : {11, 12}) {
            QStandardItem *item = model->item(i);
            if (mbKsuDataTypesEnabled && comboBox != serverDataTypeComboBox) {
                item->setFlags(item->flags() | (Qt::ItemIsSelectable | Qt::ItemIsEnabled));
            } else {
                item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
            }

            if (comboBox->currentIndex() == i && !mbKsuDataTypesEnabled) {
                comboBox->setCurrentIndex(4);
            }
        }
    }


    if (type == 3) {
        extendedParametrsTableView->setModel(m_enumStrategyValuesModel);
        extendedParametrsTableView->horizontalHeader()->setStretchLastSection(true);
        extendedParametrsTableView->verticalHeader()->setVisible(true);
    } if (type == 4) {
        extendedParametrsTableView->setModel(m_bitsetStrategyValuesModel);
        extendedParametrsTableView->horizontalHeader()->setStretchLastSection(true);
        extendedParametrsTableView->verticalHeader()->setVisible(false);
    }
}

void ModbusVariableForm::save()
{
    m_variableElement.setAttribute(QStringLiteral("id"), variableNodeElementId);

    m_variableElement.setAttribute(QStringLiteral("name"), nameLineEdit->text().trimmed());
    m_variableElement.setAttribute(QStringLiteral("description"), descriptionTextEdit->toPlainText().trimmed());
    m_variableElement.setAttribute(QStringLiteral("measureUnit"), measureUnitLineEdit->text().trimmed());
    m_variableElement.setAttribute(QStringLiteral("readable"), readAccessCheckBox->isChecked());
    m_variableElement.setAttribute(QStringLiteral("writable"), writeAccessCheckBox->isChecked());
    m_variableElement.setAttribute(QStringLiteral("enabled"), enabledCheckBox->isChecked());
    m_variableElement.setAttribute(QStringLiteral("permissibleVariation"), permissibleVariationLineEdit->text().trimmed());

    QString alias = aliasLineEdit->selectedAlias();
    if (alias.trimmed().isEmpty()) {
        m_variableElement.removeAttribute(QStringLiteral("alias"));
    } else {
        m_variableElement.setAttribute(QStringLiteral("alias"), alias);
    }

    if (periodEnableCheckBox->isChecked()) {
        m_variableElement.setAttribute(QStringLiteral("period"), periodSpinBox->value());
    } else {
        m_variableElement.removeAttribute(QStringLiteral("period"));
    }

    m_variableElement.setAttribute(QStringLiteral("address"), decAddressSpinBox->value());
    m_variableElement.setAttribute(QStringLiteral("byteOrder"), byteOrderComboBox->currentText());

    QString strategyTypeString;
    int strategyType = variableTypeGroup->checkedId();
    switch (strategyType) {
        case 1: // linear strategy
            strategyTypeString = "NumberStrategy";
            break;

        case 2: // register strategy
            strategyTypeString = "RegisterStrategy";
            break;

        case 3: // enum strategy
            strategyTypeString = "EnumStrategy";
            break;

        case 4: // bitset strategy
            strategyTypeString = "BitsetStrategy";
            break;

        case 0: // standart strategy
        default:
            strategyTypeString = "NoneStrategy";
            break;
    }

    m_variableElement.setAttribute(QStringLiteral("valueStrategyType"), strategyTypeString);
    m_variableElement.setAttribute(QStringLiteral("deviceDataType"), deviceDataTypeComboBox->currentText());
    m_variableElement.setAttribute(QStringLiteral("serverValueDataType"), serverDataTypeComboBox->currentText());


    bool shouldRemoveMinMax = strategyType == 3 ||
                              deviceDataTypeComboBox->currentIndex() == 11 ||
                              deviceDataTypeComboBox->currentIndex() == 12;

    if (serverDataTypeComboBox->currentIndex() == 9 ||
        serverDataTypeComboBox->currentIndex() == 10 ||
        deviceDataTypeComboBox->currentIndex() == 12) {

        if (deviceDataTypeComboBox->currentIndex() == 12) {
            m_variableElement.setAttribute(QStringLiteral("precision"), stringRegistersCountSpinBox->value());
        } else {
            m_variableElement.setAttribute(QStringLiteral("precision"), floatPrecisionSpinBox->value());
        }
    } else {
        m_variableElement.removeAttribute(QStringLiteral("precision"));
    }

    if (minimumLineEdit->text().simplified().isEmpty() || shouldRemoveMinMax) {
        m_variableElement.removeAttribute(QStringLiteral("minValue"));
    } else {
        m_variableElement.setAttribute(QStringLiteral("minValue"), minimumLineEdit->text().simplified());
    }

    if (maximumLineEdit->text().simplified().isEmpty() || shouldRemoveMinMax) {
        m_variableElement.removeAttribute(QStringLiteral("maxValue"));
    } else {
        m_variableElement.setAttribute(QStringLiteral("maxValue"), maximumLineEdit->text().simplified());
    }

    if (defaultValueLineEdit->text().simplified().isEmpty() || shouldRemoveMinMax) {
        m_variableElement.removeAttribute(QStringLiteral("defaultValue"));
    } else {
        m_variableElement.setAttribute(QStringLiteral("defaultValue"), defaultValueLineEdit->text().simplified());
    }

    m_variableElement.setAttribute(QStringLiteral("coefficientK"), coefficientKSpinBox->value());
    m_variableElement.setAttribute(QStringLiteral("coefficientB"), coefficientBSpinBox->value());

    m_variableElement.setAttribute(QStringLiteral("startBit"), startBitSpinBox->value());
    m_variableElement.setAttribute(QStringLiteral("endBit"), endBitSpinBox->value());

    saveEnumValues();
    saveBitsetValues();

    if (!m_parentNode.isNull()) {
        m_parentNode.appendChild(m_variableElement);
        emit elementAdded(m_variableElement);
    } else {
        elementSaved(m_variableElement);
    }

    close();
}

void ModbusVariableForm::appendClicked()
{
    if (m_strategyType != 3 && m_strategyType != 4)
        return;

    if (m_strategyType == 3) {
        ModbusVariableEditBitsetValue *appendEnumValueForm = new ModbusVariableEditBitsetValue();
        appendEnumValueForm->setAttribute(Qt::WA_DeleteOnClose, true);
        appendEnumValueForm->setWindowModality(Qt::ApplicationModal);
        appendEnumValueForm->setCodeLabelText(tr("Маска флага:"));
        appendEnumValueForm->setValueLabelText(tr("Значение флага:"));
        appendEnumValueForm->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, QSizePolicy::DefaultType));

        connect(appendEnumValueForm, &ModbusVariableEditBitsetValue::accepted, [=](){
            QString bitsetCodeString = appendEnumValueForm->code().trimmed();

            bool ok;
            quint64 enumCode = bitsetCodeString.toULongLong(&ok, 0);
            if (!ok || bitsetCodeString.isEmpty()) {
                ErrorClassifier::instance()->showError(7);
                return;
            }

            if (m_enumFlags.contains(enumCode)) {
                ErrorClassifier::instance()->showError(8);
                return;
            }

            QString enumValue = appendEnumValueForm->value().trimmed();
            if (enumValue.isEmpty()) {
                ErrorClassifier::instance()->showError(9);
                return;
            }
            appendEnumValueForm->close();

            m_enumFlags.insert(enumCode);

            QString keyText = stringHexCode(enumCode);
            QStandardItem *keyItem = new QStandardItem(keyText);
            keyItem->setEditable(false);

            QStandardItem *valueItem = new QStandardItem(enumValue);
            valueItem->setEditable(false);

            QList<QStandardItem *> items;
            items.append(keyItem);
            items.append(valueItem);
            m_enumStrategyValuesModel->appendRow(items);

            m_enumStrategyValuesModel->sort(0);
        });

        appendEnumValueForm->move(QApplication::desktop()->screenGeometry().center() - appendEnumValueForm->rect().center());
        appendEnumValueForm->show();


        //        QInputDialog appendEnumValueDialog;
        //        appendEnumValueDialog.setWindowTitle(tr("САП КСУ - Добавить параметр"));
        //        appendEnumValueDialog.setLabelText("Строка перечислимого типа:");
        //        appendEnumValueDialog.setOkButtonText(tr("Да"));
        //        appendEnumValueDialog.setCancelButtonText(tr("Отменить"));

        //        if (appendEnumValueDialog.exec() == QDialog::Accepted) {
        //            QString enumValue = appendEnumValueDialog.textValue().trimmed();

        //            if (!enumValue.isEmpty()) {
        //                QStandardItem *valueItem = new QStandardItem(enumValue);
        //                valueItem->setEditable(false);

        //                m_enumStrategyValuesModel->appendRow(valueItem);
        //            }
        //        }
    } else if (m_strategyType == 4) {
        ModbusVariableEditBitsetValue *appendBitsetValueForm = new ModbusVariableEditBitsetValue();
        appendBitsetValueForm->setAttribute(Qt::WA_DeleteOnClose, true);
        appendBitsetValueForm->setWindowModality(Qt::ApplicationModal);
        appendBitsetValueForm->setCodeLabelText(tr("Маска флага:"));
        appendBitsetValueForm->setValueLabelText(tr("Значение флага:"));
        appendBitsetValueForm->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, QSizePolicy::DefaultType));

        connect(appendBitsetValueForm, &ModbusVariableEditBitsetValue::accepted, [=](){
            QString bitsetCodeString = appendBitsetValueForm->code().trimmed();

            bool ok;
            quint64 bitsetCode = bitsetCodeString.toULongLong(&ok, 0);
            if (!ok || bitsetCodeString.isEmpty()) {
                ErrorClassifier::instance()->showError(7);
                return;
            }

            if (m_bitsetFlags.contains(bitsetCode)) {
                ErrorClassifier::instance()->showError(8);
                return;
            }

            QString bitsetValue = appendBitsetValueForm->value().trimmed();
            if (bitsetValue.isEmpty()) {
                ErrorClassifier::instance()->showError(9);
                return;
            }
            appendBitsetValueForm->close();

            m_bitsetFlags.insert(bitsetCode);

            QString keyText = stringHexCode(bitsetCode);

            QStandardItem *keyItem = new QStandardItem(keyText);
            keyItem->setEditable(false);

            QStandardItem *valueItem = new QStandardItem(bitsetValue);
            valueItem->setEditable(false);

            QList<QStandardItem *> items;
            items.append(keyItem);
            items.append(valueItem);
            m_bitsetStrategyValuesModel->appendRow(items);

            m_bitsetStrategyValuesModel->sort(0);
        });

        appendBitsetValueForm->show();
    }
}

void ModbusVariableForm::removeClicked()
{
    if (m_strategyType != 3 && m_strategyType != 4)
        return;

    QModelIndex currentIndex = extendedParametrsTableView->currentIndex();
    if (!currentIndex.isValid())
        return;

    if (m_strategyType == 3) {
        QVariant enumCodeVariant = m_enumStrategyValuesModel->item(currentIndex.row(), 0)->data(Qt::DisplayRole);
        QString enumCodeString = enumCodeVariant.toString();

        bool ok;
        quint64 enumCode = enumCodeString.toULongLong(&ok, 0);
        if (ok && m_enumFlags.contains(enumCode))
            m_enumFlags.remove(enumCode);

        m_enumStrategyValuesModel->removeRow(currentIndex.row());
    } else if (m_strategyType == 4) {
        QVariant bitsetCodeVariant = m_bitsetStrategyValuesModel->item(currentIndex.row(), 0)->data(Qt::DisplayRole);
        QString bitsetCodeString = bitsetCodeVariant.toString();

        bool ok;
        quint64 bitsetCode = bitsetCodeString.toULongLong(&ok, 0);
        if (ok && m_bitsetFlags.contains(bitsetCode))
            m_bitsetFlags.remove(bitsetCode);

        m_bitsetStrategyValuesModel->removeRow(currentIndex.row());
    }
}

void ModbusVariableForm::deviceDataTypeComboBoxCurrentIndexChanged(int index)
{
    bool disableMinMaxDefault = false;

    if (index == 11 || index == 12) {
        minimumLineEdit->clear();
        maximumLineEdit->clear();
        defaultValueLineEdit->clear();

        serverDataTypeComboBox->setCurrentIndex(index);

        disableMinMaxDefault = true;
    } else {
        int serverIndex = serverDataTypeComboBox->currentIndex();
        if (serverIndex == 11 || serverIndex == 12) {
            serverDataTypeComboBox->setCurrentIndex(index);
        }

        parseAndFillMinMaxDefaultValues();
    }

    stringRegistersCountLabel->setVisible(index == 12);
    stringRegistersCountSpinBox->setVisible(index == 12);

    minimumLineEdit->setDisabled(disableMinMaxDefault);
    maximumLineEdit->setDisabled(disableMinMaxDefault);
    defaultValueLineEdit->setDisabled(disableMinMaxDefault);

    serverDataTypeComboBox->setDisabled(disableMinMaxDefault);
}

void ModbusVariableForm::serverDataTypeComboBoxCurrentIndexChanged(int index)
{
    switch (index) {
        case 0:
            Sap::setEditorValidator<bool>(minimumLineEdit);
            Sap::setEditorValidator<bool>(maximumLineEdit);
            Sap::setEditorValidator<bool>(defaultValueLineEdit);
            break;

        case 1:
            Sap::setEditorValidator<qint8>(minimumLineEdit);
            Sap::setEditorValidator<qint8>(maximumLineEdit);
            Sap::setEditorValidator<qint8>(defaultValueLineEdit);
            break;

        case 2:
            Sap::setEditorValidator<quint8>(minimumLineEdit);
            Sap::setEditorValidator<quint8>(maximumLineEdit);
            Sap::setEditorValidator<quint8>(defaultValueLineEdit);
            break;

        case 3:
            Sap::setEditorValidator<qint16>(minimumLineEdit);
            Sap::setEditorValidator<qint16>(maximumLineEdit);
            Sap::setEditorValidator<qint16>(defaultValueLineEdit);
            break;

        case 4:
            Sap::setEditorValidator<quint16>(minimumLineEdit);
            Sap::setEditorValidator<quint16>(maximumLineEdit);
            Sap::setEditorValidator<quint16>(defaultValueLineEdit);
            break;

        case 5:
            Sap::setEditorValidator<qint32>(minimumLineEdit);
            Sap::setEditorValidator<qint32>(maximumLineEdit);
            Sap::setEditorValidator<qint32>(defaultValueLineEdit);
            break;

        case 6:
            Sap::setEditorValidator<quint32>(minimumLineEdit);
            Sap::setEditorValidator<quint32>(maximumLineEdit);
            Sap::setEditorValidator<quint32>(defaultValueLineEdit);
            break;

        case 7:
            Sap::setEditorValidator<qint64>(minimumLineEdit);
            Sap::setEditorValidator<qint64>(maximumLineEdit);
            Sap::setEditorValidator<qint64>(defaultValueLineEdit);
            break;

        case 8:
            Sap::setEditorValidator<quint64>(minimumLineEdit);
            Sap::setEditorValidator<quint64>(maximumLineEdit);
            Sap::setEditorValidator<quint64>(defaultValueLineEdit);
            break;

        case 9:
            Sap::setFloatEditorValidator(minimumLineEdit);
            Sap::setFloatEditorValidator(maximumLineEdit);
            Sap::setFloatEditorValidator(defaultValueLineEdit);
            break;

        case 10:
            Sap::setDoubleEditorValidator(minimumLineEdit);
            Sap::setDoubleEditorValidator(maximumLineEdit);
            Sap::setDoubleEditorValidator(defaultValueLineEdit);
            break;
    }

    bool floatPrecisionVisible = index == 9 || index == 10;
    floatPrecisionLabel->setVisible(floatPrecisionVisible);
    floatPrecisionSpinBox->setVisible(floatPrecisionVisible);
}

void ModbusVariableForm::setupUi()
{
    setupLinearParametrsWidget();
    setupRegisterParametrsWidget();
    setupEnumeratedParametrsWidget();

    setupCommonGroupBox();
    setupFeaturesGroubBox();
    setupParametrsGroupBox();
    setupButtonBox();

    mainLayout = new QVBoxLayout();
    mainLayout->addWidget(commonGroupBox);
    mainLayout->addWidget(featuresGroubBox);
    mainLayout->addWidget(parametrsGroupBox);
    mainLayout->addStretch(255);
    mainLayout->addWidget(applyButtonBox);

    setLayout(mainLayout);

    connect(deviceDataTypeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,                   &ModbusVariableForm::deviceDataTypeComboBoxCurrentIndexChanged);
    connect(serverDataTypeComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,                   &ModbusVariableForm::serverDataTypeComboBoxCurrentIndexChanged);

    changeLinearCoefficientsButton->hide();
}

void ModbusVariableForm::setupCommonGroupBox()
{
    commonGroupBox = new QGroupBox();
    commonGroupBox->setObjectName(QStringLiteral("commonGroupBox"));

    nameLineEdit = new QLineEdit();
    nameLineEdit->setObjectName(QStringLiteral("nameLineEdit"));

    descriptionTextEdit = new QTextEdit();
    descriptionTextEdit->setMaximumHeight(80);
    descriptionTextEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum, QSizePolicy::LineEdit));
    descriptionTextEdit->setObjectName(QStringLiteral("descriptionTextEdit"));

    aliasLineEdit = new ParameterAliasWidget();
    aliasLineEdit->setObjectName(QStringLiteral("aliasLineEdit"));

    measureUnitLineEdit = new QLineEdit();
    measureUnitLineEdit->setObjectName(QStringLiteral("measureUnitLineEdit"));

    readAccessCheckBox = new QCheckBox();
    readAccessCheckBox->setObjectName(QStringLiteral("readAccessCheckBox"));
    readAccessCheckBox->setText(tr("Чтение"));
    readAccessCheckBox->setChecked(true);

    writeAccessCheckBox = new QCheckBox();
    writeAccessCheckBox->setObjectName(QStringLiteral("writeAccessCheckBox"));
    writeAccessCheckBox->setText(tr("Запись"));

    accessLayout = new QHBoxLayout();
    accessLayout->addWidget(readAccessCheckBox);
    accessLayout->addWidget(writeAccessCheckBox);

    enabledCheckBox = new QCheckBox();
    Q_SET_OBJECT_NAME(enabledCheckBox);

    periodEnableCheckBox = new QCheckBox();
    periodEnableCheckBox->setObjectName(QStringLiteral("periodEnableCheckBox"));
    periodEnableCheckBox->setEnabled(false);

    periodSpinBox = new QSpinBox();
    periodSpinBox->setObjectName(QStringLiteral("periodSpinBox"));
    periodSpinBox->setMinimum(100);
    periodSpinBox->setMaximum(std::numeric_limits<int>::max());
    periodSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum, QSizePolicy::SpinBox));
    periodSpinBox->setEnabled(periodEnableCheckBox->isChecked());

    connect(periodEnableCheckBox, &QCheckBox::toggled, periodSpinBox, &QSpinBox::setEnabled);

    periodUnitLabel = new QLabel();
    periodUnitLabel->setObjectName(QStringLiteral("periodUnitLabel"));
    periodUnitLabel->setText(tr("мс"));

    periodLayout = new QHBoxLayout();
    periodLayout->addWidget(periodEnableCheckBox);
    periodLayout->addWidget(periodSpinBox);
    periodLayout->addWidget(periodUnitLabel);

    commonLayout = new QFormLayout();
    commonLayout->addRow(tr("Название"), nameLineEdit);
    commonLayout->addRow(tr("Описание"), descriptionTextEdit);
    commonLayout->addRow(tr("Алиас"), aliasLineEdit);
    commonLayout->addRow(tr("Единица измерения"), measureUnitLineEdit);
    commonLayout->addRow(tr("Права доступа"), accessLayout);
    commonLayout->addRow(tr("Параметр включен"), enabledCheckBox);
    commonLayout->addRow(tr("Период опроса"), periodLayout);
    commonLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(125);

    commonGroupBox->setLayout(commonLayout);
}

void ModbusVariableForm::setupFeaturesGroubBox()
{
    featuresGroubBox = new QGroupBox();
    featuresGroubBox->setObjectName(QStringLiteral("featuresGroubBox"));
    featuresGroubBox->setTitle(tr("Тип переменной"));

    hexAddressSpinBox = new QSpinBox();
    hexAddressSpinBox->setObjectName(QStringLiteral("hexAddressSpinBox"));
    hexAddressSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum, QSizePolicy::SpinBox));
    hexAddressSpinBox->setMaximum(std::numeric_limits<quint16>::max());
    hexAddressSpinBox->setDisplayIntegerBase(16);
    hexAddressSpinBox->setWrapping(true);

    hexAddressLabel = new QLabel();
    hexAddressLabel->setObjectName(QStringLiteral("hexAddressLabel"));
    hexAddressLabel->setText(tr("HEX"));

    hexAddressLayout = new QHBoxLayout();
    hexAddressLayout->addWidget(hexAddressSpinBox);
    hexAddressLayout->addWidget(hexAddressLabel);

    decAddressSpinBox = new QSpinBox();
    decAddressSpinBox->setObjectName(QStringLiteral("decAddressSpinBox"));
    decAddressSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum, QSizePolicy::SpinBox));
    decAddressSpinBox->setMaximum(hexAddressSpinBox->maximum());
    decAddressSpinBox->setWrapping(true);

    decAddressLabel = new QLabel();
    decAddressLabel->setObjectName(QStringLiteral("decAddressLabel"));
    decAddressLabel->setText(tr("DEC"));

    decAddressLayout = new QHBoxLayout();
    decAddressLayout->addWidget(decAddressSpinBox);
    decAddressLayout->addWidget(decAddressLabel);

    connect(hexAddressSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), decAddressSpinBox, &QSpinBox::setValue);
    connect(decAddressSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), hexAddressSpinBox, &QSpinBox::setValue);

    byteOrderComboBox = new QComboBox();
    byteOrderComboBox->addItem(tr("LittleEndian"));
    byteOrderComboBox->addItem(tr("BigEndian"));
    byteOrderComboBox->addItem(tr("PdpEndian"));

    standartTypeRadioButton = new QRadioButton();
    standartTypeRadioButton->setObjectName(QStringLiteral("standartTypeRadioButton"));
    standartTypeRadioButton->setText(tr("Стандартная"));
    standartTypeRadioButton->setChecked(true);

    linearTypeRadioButton = new QRadioButton();
    linearTypeRadioButton->setObjectName(QStringLiteral("linearTypeRadioButton"));
    linearTypeRadioButton->setText(tr("Линейная"));

    registerTypeRadioButton = new QRadioButton();
    registerTypeRadioButton->setObjectName(QStringLiteral("registerTypeRadioButton"));
    registerTypeRadioButton->setText(tr("Регистр"));

    enumTypeRadioButton = new QRadioButton();
    enumTypeRadioButton->setObjectName(QStringLiteral("enumTypeRadioButton"));
    enumTypeRadioButton->setText(tr("Перечисление"));

    bitsetTypeRadioButton = new QRadioButton();
    bitsetTypeRadioButton->setObjectName(QStringLiteral("bitsetTypeRadioButton"));
    bitsetTypeRadioButton->setText(tr("Список бит"));

    variableTypeGroup = new QButtonGroup(this);
    variableTypeGroup->addButton(standartTypeRadioButton, 0);
    variableTypeGroup->addButton(linearTypeRadioButton, 1);
    variableTypeGroup->addButton(registerTypeRadioButton, 2);
    variableTypeGroup->addButton(enumTypeRadioButton, 3);
    variableTypeGroup->addButton(bitsetTypeRadioButton, 4);

    connect(variableTypeGroup,  static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this,               &ModbusVariableForm::setVariableStrategy);

    typeLayout = new QGridLayout();
    typeLayout->addWidget(standartTypeRadioButton, 0, 0);
    typeLayout->addWidget(linearTypeRadioButton, 0, 1);
    typeLayout->addWidget(registerTypeRadioButton, 0, 2);
    typeLayout->addWidget(enumTypeRadioButton, 1, 0);
    typeLayout->addWidget(bitsetTypeRadioButton, 1, 1);

    featuresLayout = new QFormLayout();
    featuresLayout->addRow(tr("Адрес переменной"), hexAddressLayout);
    featuresLayout->setLayout(1, QFormLayout::FieldRole, decAddressLayout);
    featuresLayout->addRow(tr("Порядок байт"), byteOrderComboBox);
    featuresLayout->addRow(typeLayout);
    featuresLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(125);

    featuresGroubBox->setLayout(featuresLayout);
}

void ModbusVariableForm::setupParametrsGroupBox()
{
    parametrsGroupBox = new QGroupBox();
    parametrsGroupBox->setObjectName(QStringLiteral("parametrsGroupBox"));
    parametrsGroupBox->setTitle(tr("Параметры переменной"));

    deviceDataTypeComboBox = new QComboBox();
    deviceDataTypeComboBox->setObjectName(QStringLiteral("deviceDataTypeComboBox"));

    serverDataTypeComboBox = new QComboBox();
    serverDataTypeComboBox->setObjectName(QStringLiteral("serverDataTypeComboBox"));

    minimumLabel = new QLabel();
    minimumLabel->setObjectName(QStringLiteral("minimumLabel"));
    minimumLabel->setText(tr("Минимум"));

    minimumLineEdit = new QLineEdit();
    minimumLineEdit->setObjectName(QStringLiteral("minimumLineEdit"));

    maximumLabel = new QLabel();
    maximumLabel->setObjectName(QStringLiteral("maximumLabel"));
    maximumLabel->setText(tr("Максимум"));

    maximumLineEdit = new QLineEdit();
    maximumLineEdit->setObjectName(QStringLiteral("maximumLineEdit"));

    defaultValueLabel = new QLabel;
    defaultValueLabel->setObjectName(QStringLiteral("defaultValueLabel"));
    defaultValueLabel->setText(tr("Значение по умолчанию"));

    defaultValueLineEdit = new QLineEdit();
    defaultValueLineEdit->setObjectName(QStringLiteral("defaultValueLineEdit"));

    permissibleVariationLabel = new QLabel;
    permissibleVariationLabel->setObjectName("permissibleVariationLabel");
    permissibleVariationLabel->setText(tr("Допустимое отклонение"));

    permissibleVariationLineEdit = new QLineEdit;
    permissibleVariationLineEdit->setObjectName("permissibleVariationLineEdit");

    extendedParametrsLayout = new QVBoxLayout();
    extendedParametrsLayout->setContentsMargins(QMargins());
    extendedParametrsLayout->addWidget(linearParametrsWidget);
    extendedParametrsLayout->addWidget(registerParametrsWidget);
    extendedParametrsLayout->addWidget(enumeratedParametrsWidget);

    extendedParametrsWidget = new QWidget();
    extendedParametrsWidget->setObjectName(QStringLiteral("extendedParametrsWidget"));
    extendedParametrsWidget->setLayout(extendedParametrsLayout);


    stringRegistersCountLabel = new QLabel;
    stringRegistersCountLabel->setObjectName("stringRegistersCountLabel");
    stringRegistersCountLabel->setText(tr("Количество регистров"));
    stringRegistersCountLabel->setHidden(true);

    stringRegistersCountSpinBox = new QSpinBox;
    stringRegistersCountSpinBox->setObjectName("stringRegistersCountSpinBox");
    stringRegistersCountSpinBox->setMinimum(1);
    stringRegistersCountSpinBox->setMaximum(100);
    stringRegistersCountSpinBox->setWrapping(true);
    stringRegistersCountSpinBox->setHidden(true);

    floatPrecisionLabel = new QLabel;
    floatPrecisionLabel->setObjectName("floatPrecisionLabel");
    floatPrecisionLabel->setText(tr("Точность"));
    floatPrecisionLabel->setHidden(true);

    floatPrecisionSpinBox = new QSpinBox;
    floatPrecisionSpinBox->setObjectName("floatPrecisionSpinBox");
    floatPrecisionSpinBox->setMinimum(-1);
    floatPrecisionSpinBox->setMaximum(1000);
    floatPrecisionSpinBox->setWrapping(true);
    floatPrecisionSpinBox->setHidden(true);


    parametrsLayout = new QFormLayout();
    parametrsLayout->addRow(tr("Тип данных устройства"), deviceDataTypeComboBox);
    parametrsLayout->addRow(stringRegistersCountLabel, stringRegistersCountSpinBox);
    parametrsLayout->addRow(tr("Тип данных сервера"), serverDataTypeComboBox);
    parametrsLayout->addRow(floatPrecisionLabel, floatPrecisionSpinBox);
    parametrsLayout->addRow(extendedParametrsWidget);
    parametrsLayout->addRow(minimumLabel, minimumLineEdit);
    parametrsLayout->addRow(maximumLabel, maximumLineEdit);
    parametrsLayout->addRow(defaultValueLabel, defaultValueLineEdit);
    parametrsLayout->addRow(permissibleVariationLabel, permissibleVariationLineEdit);

    parametrsLayout->labelForField(deviceDataTypeComboBox)->setMinimumWidth(125);

    parametrsGroupBox->setLayout(parametrsLayout);
}

void ModbusVariableForm::setupButtonBox()
{
    applyButtonBox = new QDialogButtonBox(Qt::Horizontal);
    applyButtonBox->setObjectName(QStringLiteral("buttonBox"));
    applyButtonBox->addButton(tr("Сохранить"), QDialogButtonBox::AcceptRole);
    applyButtonBox->addButton(tr("Отменить"), QDialogButtonBox::RejectRole);

    connect(applyButtonBox, &QDialogButtonBox::rejected, this, &ModbusVariableForm::close);
    connect(applyButtonBox, &QDialogButtonBox::accepted, this, &ModbusVariableForm::save);
}

void ModbusVariableForm::setupLinearParametrsWidget()
{
    linearParametrsWidget = new QWidget();
    linearParametrsWidget->setObjectName(QStringLiteral("linearParametrsWidget"));

    coefficientKSpinBox = new QDoubleSpinBox();
    coefficientKSpinBox->setObjectName(QStringLiteral("coefficientKSpinBox"));
    coefficientKSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    coefficientKSpinBox->setMaximum(std::numeric_limits<double>::max());
    coefficientKSpinBox->setDecimals(6);
    coefficientKSpinBox->setValue(1.0);

    coefficientBSpinBox = new QDoubleSpinBox();
    coefficientBSpinBox->setObjectName(QStringLiteral("coefficientBSpinBox"));
    coefficientBSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    coefficientBSpinBox->setMaximum(std::numeric_limits<double>::max());
    coefficientBSpinBox->setDecimals(6);
    coefficientBSpinBox->setValue(0.0);

    changeLinearCoefficientsButton = new QPushButton();
    changeLinearCoefficientsButton->setText(tr("Пересчитать по шкалам"));

    linearParametrsLayout = new QFormLayout();
    linearParametrsLayout->setContentsMargins(QMargins());
    linearParametrsLayout->addRow(tr("Коэффициент K"), coefficientKSpinBox);
    linearParametrsLayout->addRow(tr("Коэффициент B"), coefficientBSpinBox);
    linearParametrsLayout->addRow(changeLinearCoefficientsButton);
    linearParametrsLayout->itemAt(0, QFormLayout::LabelRole)->widget()->setMinimumWidth(125);

    linearParametrsWidget->setLayout(linearParametrsLayout);
}

void ModbusVariableForm::setupRegisterParametrsWidget()
{
    registerParametrsWidget = new QWidget();
    registerParametrsWidget->setObjectName(QStringLiteral("registerParametrsWidget"));

    significantBitsLabel = new QLabel();
    significantBitsLabel->setObjectName(QStringLiteral("significantBitsLabel"));
    significantBitsLabel->setText(tr("Значимые биты"));
    significantBitsLabel->setMinimumWidth(125);

    startBitLabel = new QLabel();
    startBitLabel->setObjectName(QStringLiteral("startBitLabel"));
    startBitLabel->setText(tr("с"));

    startBitSpinBox = new QSpinBox();
    startBitSpinBox->setObjectName(QStringLiteral("startBitSpinBox"));
    startBitSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum, QSizePolicy::SpinBox));

    endBitLabel = new QLabel();
    endBitLabel->setObjectName(QStringLiteral("endBitLabel"));
    endBitLabel->setText(tr("по"));

    endBitSpinBox = new QSpinBox();
    endBitSpinBox->setObjectName(QStringLiteral("endBitSpinBox"));
    endBitSpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum, QSizePolicy::SpinBox));

    connect(startBitSpinBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), endBitSpinBox, &QSpinBox::setMinimum);

    significantBitsNotationLabel = new QLabel();
    significantBitsNotationLabel->setObjectName(QStringLiteral("significantBitsNotationLabel"));
    significantBitsNotationLabel->setText(tr("DEC"));

    significantBitsLayout = new QHBoxLayout();
    significantBitsLayout->setContentsMargins(QMargins());
    significantBitsLayout->addWidget(significantBitsLabel);
    significantBitsLayout->addWidget(startBitLabel);
    significantBitsLayout->addWidget(startBitSpinBox);
    significantBitsLayout->addWidget(endBitLabel);
    significantBitsLayout->addWidget(endBitSpinBox);
    significantBitsLayout->addWidget(significantBitsNotationLabel);

    registerParametrsWidget->setLayout(significantBitsLayout);
}

void ModbusVariableForm::setupEnumeratedParametrsWidget()
{
    enumeratedParametrsWidget = new QWidget();
    enumeratedParametrsWidget->setObjectName(QStringLiteral("enumeratedParametrsWidget"));

    extendedParametrsTableView = new QTableView();
    extendedParametrsTableView->setObjectName(QStringLiteral("extendedParametrsTableView"));
    extendedParametrsTableView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,
                                                          QSizePolicy::Expanding,
                                                          QSizePolicy::DefaultType));

    appendButton = new QPushButton();
    appendButton->setObjectName(QStringLiteral("appendButton"));
    appendButton->setText(tr("Добавить"));

    removeButton = new QPushButton();
    removeButton->setObjectName(QStringLiteral("removeButton"));
    removeButton->setText(tr("Удалить"));

    enumeratedParametrsLayout = new QGridLayout();
    enumeratedParametrsLayout->setContentsMargins(QMargins());
    enumeratedParametrsLayout->addWidget(extendedParametrsTableView, 0, 0, 5, 5);
    enumeratedParametrsLayout->addWidget(appendButton, 0, 6);
    enumeratedParametrsLayout->addWidget(removeButton, 1, 6);

    enumeratedParametrsWidget->setLayout(enumeratedParametrsLayout);

    connect(appendButton, &QPushButton::clicked, this, &ModbusVariableForm::appendClicked);
    connect(removeButton, &QPushButton::clicked, this, &ModbusVariableForm::removeClicked);
}

void ModbusVariableForm::fillDataTypes()
{
    QStringList commonDataTypes;
    commonDataTypes << "Boolean"
                    << "Int8"     << "UInt8"
                    << "Int16"    << "UInt16"
                    << "Int32"    << "UInt32"
                    << "Int64"    << "UInt64"
                    << "Float32"  << "Float64";

    QStringList serverDataTypes;
    serverDataTypes << commonDataTypes
                    << "DateTime" << "String";

    serverDataTypeComboBox->clear();
    serverDataTypeComboBox->addItems(serverDataTypes);
    serverDataTypeComboBox->setCurrentIndex(4);

    QStringList deviceDataTypes;
    deviceDataTypes << commonDataTypes
                    << "ModbusKsuDate" << "ModbusKsuString";

    deviceDataTypeComboBox->clear();
    deviceDataTypeComboBox->addItems(deviceDataTypes);
    deviceDataTypeComboBox->setCurrentIndex(4);

}

void ModbusVariableForm::traverseXMLElement(const QDomElement &element)
{
    nameLineEdit->setText(element.attribute(QStringLiteral("name")));
    descriptionTextEdit->setPlainText(element.attribute(QStringLiteral("description")));
    aliasLineEdit->setupAlias(element.attribute(QStringLiteral("alias")));
    measureUnitLineEdit->setText(element.attribute(QStringLiteral("measureUnit")));

    readAccessCheckBox->setChecked(element.attribute(QStringLiteral("readable")).toInt());
    writeAccessCheckBox->setChecked(element.attribute(QStringLiteral("writable")).toInt());

    enabledCheckBox->setChecked(element.attribute(QStringLiteral("enabled")).toInt());

    int period = element.attribute(QStringLiteral("period")).toInt();
    periodSpinBox->setValue(period);
    periodEnableCheckBox->setChecked(period);

    decAddressSpinBox->setValue(element.attribute(QStringLiteral("address")).toInt());
    byteOrderComboBox->setCurrentText(element.attribute(QStringLiteral("byteOrder")));

    int strategyId = 0;
    QString strategyTypeString = element.attribute(QStringLiteral("valueStrategyType"));
    if (strategyTypeString == "NumberStrategy")     strategyId = 1;
    if (strategyTypeString == "RegisterStrategy")   strategyId = 2;
    if (strategyTypeString == "EnumStrategy")       strategyId = 3;
    if (strategyTypeString == "BitsetStrategy")     strategyId = 4;

    QAbstractButton *strategyButton = variableTypeGroup->button(strategyId);
    if (strategyButton) strategyButton->setChecked(true);
    setVariableStrategy(strategyId);

    deviceDataTypeComboBox->setCurrentText(element.attribute(QStringLiteral("deviceDataType")));
    serverDataTypeComboBox->setCurrentText(element.attribute(QStringLiteral("serverValueDataType")));

    parseAndFillMinMaxDefaultValues();

    bool ok = false;

    double coefficientK = element.attribute(QStringLiteral("coefficientK")).toDouble(&ok);
    if (ok) coefficientKSpinBox->setValue(coefficientK);

    double coefficientB = element.attribute(QStringLiteral("coefficientB")).toDouble(&ok);
    if (ok) coefficientBSpinBox->setValue(coefficientB);


    int startBit = element.attribute(QStringLiteral("startBit")).toInt(&ok);
    if (ok) startBitSpinBox->setValue(startBit);

    int endBit = element.attribute(QStringLiteral("endBit")).toInt(&ok);
    if (ok) endBitSpinBox->setValue(endBit);

    QString permissibleVariation = element.attribute(QStringLiteral("permissibleVariation"));
    permissibleVariationLineEdit->setText(permissibleVariation);

    QMap<quint64, QString> bitsValues;
    QStringList mapStringList = element.attribute("bitsetValues").split(";/", QString::SkipEmptyParts);
    for (const QString &oneValue : mapStringList) {
        QStringList mapElementStringList = oneValue.split("::");
        if (mapElementStringList.length() == 2 && !mapElementStringList.at(1).isEmpty()) {
            bool ok = false;
            QString keyString = mapElementStringList.at(0);
            quint64 key = keyString.toULongLong(&ok, 0);
            if (!ok) continue;
            bitsValues.insert(key, mapElementStringList.at(1));
        }
    }

    parseEnumStrategyValues();
    parseBitsetStrategyValues();


    int precision = element.attribute(QStringLiteral("precision")).toInt(&ok);
    if (ok) {
        if (serverDataTypeComboBox->currentIndex() == 9 ||
            serverDataTypeComboBox->currentIndex() == 10) {
            floatPrecisionSpinBox->setValue(precision);
        } else if (deviceDataTypeComboBox->currentIndex() == 12) {
            stringRegistersCountSpinBox->setValue(precision);
        }
    }
}

void ModbusVariableForm::parseAndFillMinMaxDefaultValues()
{
    minimumLineEdit->setText(m_variableElement.attribute(QStringLiteral("minValue")));
    maximumLineEdit->setText(m_variableElement.attribute(QStringLiteral("maxValue")));
    defaultValueLineEdit->setText(m_variableElement.attribute(QStringLiteral("defaultValue")));
}

void ModbusVariableForm::parseEnumStrategyValues()
{
    m_enumStrategyValuesModel->clear();
    m_enumStrategyValuesModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Маска флага")));
    m_enumStrategyValuesModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Значение флага")));
    m_enumFlags.clear();

    QList<QStandardItem *> items;
    QStringList mapStringList = m_variableElement.attribute(QStringLiteral("enumValues")).split(";/", QString::SkipEmptyParts);
    for (const QString &oneValue : mapStringList) {
        QStringList mapElementStringList = oneValue.split(QStringLiteral("::"));
        if (mapElementStringList.length() == 2 && !mapElementStringList.at(1).isEmpty()) {
            bool ok = false;
            QString keyString = mapElementStringList.at(0);
            quint64 key = keyString.toULongLong(&ok, 0);
            if (!ok || m_enumFlags.contains(key)) continue;

            m_enumFlags.insert(key);

            QString keyText = stringHexCode(key);
            QStandardItem *keyItem = new QStandardItem(keyText);
            keyItem->setEditable(false);

            QStandardItem *valueItem = new QStandardItem(mapElementStringList.at(1));
            valueItem->setEditable(false);

            items.clear();
            items.append(keyItem);
            items.append(valueItem);

            m_enumStrategyValuesModel->appendRow(items);
        }
    }

    m_enumStrategyValuesModel->sort(0);
}

void ModbusVariableForm::parseBitsetStrategyValues()
{
    m_bitsetStrategyValuesModel->clear();
    m_bitsetStrategyValuesModel->setHorizontalHeaderItem(0, new QStandardItem(tr("Маска флага")));
    m_bitsetStrategyValuesModel->setHorizontalHeaderItem(1, new QStandardItem(tr("Значение флага")));
    m_bitsetFlags.clear();

    QList<QStandardItem *> items;
    QStringList mapStringList = m_variableElement.attribute(QStringLiteral("bitsetValues")).split(";/", QString::SkipEmptyParts);
    for (const QString &oneValue : mapStringList) {
        QStringList mapElementStringList = oneValue.split(QStringLiteral("::"));
        if (mapElementStringList.length() == 2 && !mapElementStringList.at(1).isEmpty()) {
            bool ok = false;
            QString keyString = mapElementStringList.at(0);
            quint64 key = keyString.toULongLong(&ok, 0);
            if (!ok || m_bitsetFlags.contains(key)) continue;

            m_bitsetFlags.insert(key);

            QString keyText = stringHexCode(key);
            QStandardItem *keyItem = new QStandardItem(keyText);
            keyItem->setEditable(false);

            QStandardItem *valueItem = new QStandardItem(mapElementStringList.at(1));
            valueItem->setEditable(false);

            items.clear();
            items.append(keyItem);
            items.append(valueItem);

            m_bitsetStrategyValuesModel->appendRow(items);
        }
    }

    m_bitsetStrategyValuesModel->sort(0);
}

void ModbusVariableForm::saveEnumValues()
{
    QStringList enumElement;
    QStringList enumValues;
    for (int i = 0; i < m_enumStrategyValuesModel->rowCount(); i++) {
        QString codeString = m_enumStrategyValuesModel->data(m_enumStrategyValuesModel->index(i, 0)).toString();
        QString value = m_enumStrategyValuesModel->data(m_enumStrategyValuesModel->index(i, 1)).toString().trimmed();

        bool ok;
        quint64 code = codeString.toULongLong(&ok, 0);

        if (!ok || value.isEmpty() || !m_enumFlags.contains(code))
            continue;

        enumElement.clear();
        enumElement << QString::number(code) << value;
        enumValues.append(enumElement.join(QStringLiteral("::")));
    }

    if (enumValues.isEmpty()) {
        m_variableElement.removeAttribute(QStringLiteral("enumValues"));
    } else {
        QString enumValuesString = enumValues.join(QStringLiteral(";/"));
        m_variableElement.setAttribute(QStringLiteral("enumValues"), enumValuesString);
    }

    //    QStringList enumValuesList;
    //    for (int i = 0; i < m_enumStrategyValuesModel->rowCount(); i++) {
    //        QString enumStringValue = m_enumStrategyValuesModel->data(m_enumStrategyValuesModel->index(i, 0)).toString().trimmed();
    //        if (!enumStringValue.isEmpty())
    //            enumValuesList.append(enumStringValue);
    //    }
    //    if (enumValuesList.isEmpty()) {
    //        m_variableElement.removeAttribute(QStringLiteral("enumValues"));
    //    } else {
    //        QString enumValuesString = enumValuesList.join(QStringLiteral(";/"));
    //        m_variableElement.setAttribute(QStringLiteral("enumValues"), enumValuesString);
    //    }
}

void ModbusVariableForm::saveBitsetValues()
{
    QStringList bitsetElement;
    QStringList bitsetValues;
    for (int i = 0; i < m_bitsetStrategyValuesModel->rowCount(); i++) {
        QString codeString = m_bitsetStrategyValuesModel->data(m_bitsetStrategyValuesModel->index(i, 0)).toString();
        QString value = m_bitsetStrategyValuesModel->data(m_bitsetStrategyValuesModel->index(i, 1)).toString().trimmed();

        bool ok;
        quint64 code = codeString.toULongLong(&ok, 0);

        if (!ok || value.isEmpty() || !m_bitsetFlags.contains(code))
            continue;

        bitsetElement.clear();
        bitsetElement << QString::number(code) << value;
        bitsetValues.append(bitsetElement.join(QStringLiteral("::")));
    }

    if (bitsetValues.isEmpty()) {
        m_variableElement.removeAttribute(QStringLiteral("bitsetValues"));
    } else {
        QString bitsetValuesString = bitsetValues.join(QStringLiteral(";/"));
        m_variableElement.setAttribute(QStringLiteral("bitsetValues"), bitsetValuesString);
    }
}

QString ModbusVariableForm::stringHexCode(quint64 value) const
{
    QString text;
    QTextStream stream(&text);
    stream.setNumberFlags(QTextStream::UppercaseDigits);

    stream << left << "0x";

    stream.setFieldWidth(8);
    stream.setPadChar('0');

    stream << right << hex << value;
    stream.flush();

    return text;
}

void ModbusVariableForm::closeEvent(QCloseEvent *event)
{
    emit closed();
    event->accept();
}
