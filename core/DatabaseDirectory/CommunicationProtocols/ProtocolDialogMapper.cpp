#include "ProtocolDialogMapper.h"
#include "ReportManager/FaultReport.h"
#include "Common/FKComboBoxDelegate/FKComboBoxDelegate.h"
#include "DBConfig/DBConfig.h"
#include "ErrorClassifier/ErrorClassifier.h"

ProtocolDialogMapper::ProtocolDialogMapper(QAbstractItemModel *model, bool disableType, QWidget *parent)
    : QWidget(parent)
{
    m_code = new QLabel(tr("&Код"));
    m_protocolType = new QLabel(tr("&Протокол"));
    m_name = new QLabel(tr("&Название"));

    m_codeSpinBox = new QSpinBox();
    m_codeSpinBox->setRange(1, std::numeric_limits<int>::max());

    QSqlQueryModel *queryModel = qobject_cast<QSqlQueryModel *>(model);
    if (queryModel) {
        auto record = queryModel->record(queryModel->rowCount() - 1);
        auto lastCodeVariant = record.value("code");
        bool ok = false;
        int lastIntCode = lastCodeVariant.toInt(&ok);
        if (ok) {
            m_codeSpinBox->setValue(lastIntCode + 1);
        }
    }

    const QIcon m_protocol735Icon = QIcon(":/Images/crack-attack.png");
    const QIcon m_protocolModbusIcon = QIcon(":/Images/emblem-art.png");

    m_protocolTypeComboBox = new QComboBox();
    m_protocolTypeComboBox->insertItem(0, m_protocol735Icon, "7.35 1B");
    m_protocolTypeComboBox->insertItem(1, m_protocolModbusIcon, "Modbus");
    m_protocolTypeComboBox->setDisabled(disableType);

    m_nameLineEdit = new QLineEdit();

    m_code->setBuddy(m_codeSpinBox);
    m_protocolType->setBuddy(m_protocolTypeComboBox);
    m_name->setBuddy(m_nameLineEdit);

    QMap<int, QString> delegateMap;
    delegateMap[0] = "7.35 1B";
    delegateMap[1] = "Modbus";

    FKComboBoxDelegate *delegate = new FKComboBoxDelegate(delegateMap, this);

    m_mapper = new QDataWidgetMapper(this);
    m_mapper->setModel(model);
    m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    m_mapper->addMapping(m_codeSpinBox, 0);
    m_mapper->addMapping(m_protocolTypeComboBox, 1);
    m_mapper->addMapping(m_nameLineEdit, 2);
    m_mapper->setItemDelegate(delegate);

    QVBoxLayout *vlayout = new QVBoxLayout;

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(m_code, m_codeSpinBox);
    formLayout->addRow(m_protocolType, m_protocolTypeComboBox);
    formLayout->addRow(m_name, m_nameLineEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(Qt::Horizontal);

    m_saveButton = new QPushButton(tr("&Сохранить"));
    m_cancelButton = new QPushButton(tr("&Отменить"));

    buttonBox->addButton(m_saveButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_cancelButton, QDialogButtonBox::RejectRole);

    connect(m_saveButton, &QPushButton::clicked, this, &ProtocolDialogMapper::save);
    connect(m_cancelButton, &QPushButton::clicked, this, &ProtocolDialogMapper::close);

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

void ProtocolDialogMapper::setCurrentModelIndex(const QModelIndex &index)
{
    m_mapper->setCurrentModelIndex(index);
    m_codeSpinBox->setEnabled(!index.isValid());
}

void ProtocolDialogMapper::save()
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QString sqlString;
    SapKsu::KSUProtocolType protocolType = static_cast<SapKsu::KSUProtocolType>(m_protocolTypeComboBox->currentIndex());

    if (m_mapper->currentIndex() == -1) {
        sqlString = QString("INSERT INTO `CommunicationProtocols` (`code`, `protocolType`, `name`)"
                            "VALUES (%1, %2, '%3');")
                    .arg(m_codeSpinBox->value())
                    .arg(m_protocolTypeComboBox->currentIndex())
                    .arg(m_nameLineEdit->text());
    } else {
        sqlString = QString("UPDATE `CommunicationProtocols`"
                            "SET code = \"%1\", protocolType = \"%2\", name = \"%3\" "
                            "WHERE code = \"%4\";")
                    .arg(m_codeSpinBox->value())
                    .arg(m_protocolTypeComboBox->currentIndex())
                    .arg(m_nameLineEdit->text())
                    .arg(static_cast<QSqlQueryModel *>(m_mapper->model())->record(m_mapper->currentIndex()).value("code").toString());
    }

    QSqlQuery query(db);

    if (query.exec(sqlString) == false)
    {
        ErrorClassifier::instance()->showError(6);
        FaultReport::instance()->write(ErrorClassifier::instance()->errorFullText(6) + query.lastError().databaseText());
    } else {
        if (m_mapper->currentIndex() == -1) {
            bool ok;
            int protocolCode = query.lastInsertId().toInt(&ok);
            if (ok)
                fillDependentTables(protocolType, protocolCode);
        }

        emit dataChanged();
        close();
    }
}

void ProtocolDialogMapper::fillDependentTables(SapKsu::KSUProtocolType protocolType, int protocolCode)
{
    QProgressDialog progress;
    progress.setLabelText("Создание параметров запроса 0x10");
    progress.setCancelButton(nullptr);

    QRect screen  = QApplication::desktop()->screenGeometry();
    progress.move(screen.center() - rect().center());
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    if (protocolType == SapKsu::Ksu735) {
        progress.setMinimum(0);
        progress.setMaximum(11);
        progress.setMinimumDuration(0);

        fillKsu735WorkModes(protocolCode);
        progress.setValue(1);

        fillKsu735KindOfLaunch(protocolCode);
        progress.setValue(2);

        fillKsu735StopReasonCodes(protocolCode);
        progress.setValue(6);

        fillKsu735StatusWordInterpretationCodes(protocolCode);
        progress.setValue(8);

        fillKsu735ErrorCodes(protocolCode);
        progress.setValue(10);

        fillKsu735ReturnFlags10(protocolCode);
        progress.setValue(11);
    } else if (protocolType == SapKsu::Modbus) {
        progress.setMinimum(0);
        progress.setMaximum(13);
        progress.setMinimumDuration(0);

        fillModbusKsuKindOfLaunch(protocolCode);
        progress.setValue(1);

        fillModbusKsuStopReasonCodes(protocolCode);
        progress.setValue(8);

        fillModbusKsuArchiveEventClasses(protocolCode);
        progress.setValue(9);

        fillModbusKsuArchiveEventCodes(protocolCode);
        progress.setValue(13);
    }

    progress.close();
}

void ProtocolDialogMapper::fillKsu735WorkModes(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO WorkMode VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    codes << 0 << 1 << 2 << 3;
    query.addBindValue(codes);

    QVariantList names;
    names << "Останов" << "Ручной" << "Автоматический" << "По программе";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillKsu735KindOfLaunch(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO KindOfLaunch VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    codes << 1 << 2 << 3;
    query.addBindValue(codes);

    QVariantList names;
    names << "Оператор" << "Автоматический" << "Телеуправление";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillKsu735StopReasonCodes(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO StopReasonCodes VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    for (int i = 1; i < 100; i++)
        codes << i;
    query.addBindValue(codes);

    QVariantList names;
    names << "Останов ручной"
          << "Останов по сопротивлению изоляции"
          << "Останов по недогрузке"
          << "Останов по перегрузке"
          << "Останов по минимальному напряжению питания ПЭД"
          << "Останов по максимальному напряжению питания ПЭД"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Взлом станции управления"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Останов по контактному манометру"
          << "Останов по минимальному давлению на приеме насоса"
          << "Останов по максимальной температуре обмотки ПЭД"
          << "Зарезервировано"
          << "Останов по превышению вибрации"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Останов по превышению дисбаланса токов"
          << "Останов по превышению дисбаланса напряжений"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Останов по программе"
          << "Зарезервировано"
          << "Останов по максимальному сигналу аналогового входа 1"
          << "Останов по максимальному сигналу аналогового входа 2"
          << "Останов по телемеханике"
          << "Останов по короткому замыканию МТЗ"
          << "Останов по отключению питания"
          << "Останов по обрыву фазы A"
          << "Останов по обрыву фазы B"
          << "Останов по обрыву фазы C"
          << "Останов по обрыву всех фаз"
          << "Останов по перегрузке при отключенной защите по минимальному напряжению питания"
          << "Останов по недогрузке при отключенной защите по минимальному напряжению питания"
          << "Останов по перегрузке при отключенной защите по максимальному напряжению питания"
          << "Останов по недогрузке при отключенной защите по максимальному напряжению питания"
          << "Останов по дисбалансу токов при дисбалансе напряжений"
          << "Останов по отключению провода 0-ТМПН"
          << "Останов по минимальному сигналу аналогового входа 1"
          << "Останов по минимальному сигналу аналогового входа 2"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Останов по защите от ухода частоты сети электропитания"
          << "Зарезервировано"
          << "Сбой программы КСУ"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Останов по ЗП при напряжении питания \"Вне зоны U\""
          << "Останов по ЗCП при напряжении питания \"Вне зоны U\""
          << "Останов по дисбалансу токов при напряжении питания \"Вне зоны U\""
          << "Останов по дисбалансу напряжений при напряжении питания \"Вне зоны U\""
          << "Зарезервировано"
          << "Зарезервировано"
          << "Заклинивание"
          << "Останов по превышению напряжения на батарее конденсаторов"
          << "Останов по снижению напряжения на батарее конденсаторов"
          << "Останов по сигналу с цифрового входа 1"
          << "Останов по сигналу с цифрового входа 2"
          << "Останов по сигналу с цифрового входа 3"
          << "Останов по сигналу с цифрового входа 4"
          << "Останов по сигналу с цифрового входа 5"
          << "Останов по сигналу с цифрового входа 6"
          << "Авария IGBT1 модуля"
          << "Авария IGBT2 модуля"
          << "Авария IGBT3 модуля"
          << "Авария IGBT4 модуля"
          << "Авария IGBT5 модуля"
          << "Авария IGBT6 модуля"
          << "Останов по перегрузке IGBT модулей"
          << "Авария IGBT модулей"
          << "Неисправность датчиков температуры на охладителе силовых модулей"
          << "Неисправность датчиков температуры контроллера"
          << "Останов по недопустимо низкой температуре в контроллере"
          << "Останов по превышению температуры охладителя силовых модулей"
          << "Запрещение включения двигателя при низкой температуре охладителя силовых модулей"
          << "Сбой синхросигнала от сети: отсутствие одной из фаз или сильные помехи"
          << "Останов по превышению напряжения на батарее конденсаторов"
          << "Останов по снижению напряжения на батарее конденсаторов"
          << "Останов по превышению мгновенного значения выходного тока ПЧ Iu"
          << "Останов по превышению мгновенного значения выходного тока ПЧ Iv"
          << "Останов по превышению тока ID в звене постоянного напряжения"
          << "Останов по превышению напряжения Ud на батарее конденсаторов"
          << "Останов по недопустимому снижению напряжения питания контроллера"
          << "Останов по минимальному значению частоты ПЧ"
          << "Останов по максимальному значению частоты ПЧ"
          << "Батарея конденсаторов не заряжается или напряжения на ней меньше минимума"
          << "Нет связи между КСУ и КПЧ"
          << "Программный сбой КПЧ"
          << "Отказ СУ"
          << "Перегрев радиаторов СУ"
          << "Зарезервировано"
          << "Защита от нестабильной работы"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillKsu735StatusWordInterpretationCodes(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO StatusWordInterpretationCodes VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    for (int i = 0; i < 16; i++)
        codes << (1 << i);
    query.addBindValue(codes);

    QVariantList names;
    names << "Перегрузка"
          << "Недогрузка"
          << "Сопротивление изоляции ниже допустимого"
          << "Турбинное вращение"
          << "Чередование фаз"
          << "Вне зоны U"
          << "Срабатывание контактного манометра"
          << "Поломка контактора"
          << "Взлом станции управления"
          << "Состояние \"Авария\""
          << "Местный режим (работает оператор)"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Превышение дисбаланса токов"
          << "Превышение дисбаланса напряжений"
          << "ПЭД включен";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillKsu735ErrorCodes(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO KSU753ErrorCodes VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    for (int i = 0; i < 16; i++)
        codes << i;
    query.addBindValue(codes);

    QVariantList names;
    names << "Нормальное завершение"
          << "Ошибка CRC"
          << "Недопустимый код запроса"
          << "Недопустимое значение параметра"
          << "Выполнение команды блокировано"
          << "Попытка записи в ячейку чтения"
          << "Отказ связи \"адаптер - КСУ\""
          << "Недопустимая длина блока параметров (превышение)"
          << "Местный режим (работа обслуживающего персонала)"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Зарезервировано";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillKsu735ReturnFlags10(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO Return0x10FunctionParametrsFlags VALUES (%1, ?, ?, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    for (int i = 0; i < 24; i++) {
        codes << (1 << i);
    }
    query.addBindValue(codes);

    QVariantList names;
    names << "Дисбаланс напряжений"
          << "Дисбаланс токов"
          << "Загрузка ПЭД"
          << "Режим работы(XX) + Вид последнего запуска(SS)"
          << "Расход жидкости на выкиде насоса"
          << "Зарезервировано"
          << "Зарезервировано"
          << "Давление на выкиде насоса "
          << "Вибрация по оси Z"
          << "Зарезервировано"
          << "Вибрация по оси X+Y"
          << "Температура обмотки ПЭД"
          << "Температура жидкости на приеме насоса"
          << "Температура жидкости на выкиде насоса"
          << "Давление на приеме насоса"
          << "Напряжение \"Uc\""
          << "Напряжение \"Ub\""
          << "Напряжение \"Ua\""
          << "Коэффициент мощности (COS F)"
          << "Сопротивление изоляции"
          << "Ток потребления \"Ic\""
          << "Ток потребления \"Ib\""
          << "Ток потребления \"Ia\""
          << "Активная мощность";
    query.addBindValue(names);

    QVariantList types;
    types << 1 << 1 << 0 << 5 << 0 << 0 << 0 << 2
          << 1 << 1 << 1 << 0 << 0 << 0 << 2 << 0
          << 0 << 0 << 3 << 0 << 1 << 1 << 1 << 0;
    query.addBindValue(types);

    QVariantList aliases;
    aliases << "KSU735x10_U_disbalance"
            << "KSU735x10_I_disbalance"
            << "KSU735x10_loading_ped"
            << "KSU735x10_work_mode"
            << "KSU735x10_flow_pumpinput"
            << QVariant(QVariant::String)
            << QVariant(QVariant::String)
            << "KSU735x10_P_pumpoutput"
            << "KSU735x10_Z_Vibration"
            << QVariant(QVariant::String)
            << "KSU735x10_XY_Vibration"
            << "KSU735x10_T_PED"
            << "KSU735x10_T_pumpinput"
            << "KSU735x10_T_pumpoutput"
            << "KSU735x10_P_pumpinput"
            << "KSU735x10_Uc"
            << "KSU735x10_Ub"
            << "KSU735x10_Ua"
            << "KSU735x10_cos_F"
            << "KSU735x10_R"
            << "KSU735x10_Ic"
            << "KSU735x10_Ib"
            << "KSU735x10_Ia"
            << "KSU735x10_Power_active";
    query.addBindValue(aliases);

    query.execBatch();
    db.commit();
}

void ProtocolDialogMapper::fillModbusKsuKindOfLaunch(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO KindOfLaunch VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    codes << 1 << 2 << 3 << 4;
    query.addBindValue(codes);

    QVariantList names;
    names << "Ручной" << "Автоматический" << "Телеуправление" << "По программе";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillModbusKsuStopReasonCodes(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO StopReasonCodes VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    for (int i = 1; i < 95; i++) {
        codes << i;
    }

    QVariantList names;
    names << "Останов ручной"
          << "Останов по сопротивлению изоляции"
          << "Останов по недогрузке"
          << "Останов по перегрузке"
          << "Останов по минимальному напряжению питания ПЭД"
          << "Останов по максимальному напряжению питания ПЭД"
          << "Резерв"
          << "Резерв"
          << "Взлом станции управления"
          << "Резерв"
          << "Резерв"
          << "Останов по контактному манометру"
          << "Останов по минимальному давлению на приеме насоса"
          << "Останов по максимальной температуре обмотки ПЭД"
          << "Резерв"
          << "Останов по превышению вибрации по оси X+Y"
          << "Останов по превышению вибрации по оси Z"
          << "Резерв"
          << "Резерв"
          << "Останов по превышению дисбаланса токов"
          << "Останов по превышению дисбаланса напряжений"
          << "Резерв"
          << "Резерв"
          << "Останов по программе"
          << "Резерв"
          << "Останов по максимальному сигналу аналогового входа 1"
          << "Останов по максимальному сигналу аналогового входа 2"
          << "Останов по телемеханике"
          << "Останов по короткому замыканию МТЗ"
          << "Останов по отключению питания"
          << "Останов по обрыву фазы А"
          << "Останов по обрыву фразы В"
          << "Останов по обрыву фазы С"
          << "Останов по обрву всех фаз"
          << "Останов по перегрузке при отключенной защите по минимальному напряжению питания"
          << "Останов по недогрузке при отключенной защите по минимальному напряжению питания"
          << "Останов по перегрузке при отключенной защите по максимальному напряжению питания"
          << "Останов по недогрузке при отключенной защите по максимальному напряжению питания"
          << "Останов по дисбалансу токов при дисбалансе напряжений"
          << "Останов по отключению провода 0-ТМПН"
          << "Останов по минимальному сигналу аналогового входа 1"
          << "Останов по минимальному сигналу аналогового входа 2"
          << "Резерв"
          << "Резерв"
          << "Останов по защите от ухода частоты сети электропитания"
          << "Резерв"
          << "Сбой программы КСУ"
          << "Резерв"
          << "Резерв"
          << "Останов по ЗП при напряжении питания &quot;Вне зоны U&quot;"
          << "Останов по ЗCП при напряжении питания &quot;Вне зоны U&quot;"
          << "Останов по дисбалансу токов при напряжении питания &quot;Вне зоны U&quot;"
          << "Останов по дисбалансу напряжений при напряжении питания &quot;Вне зоны U&quot;"
          << "Резерв"
          << "Резерв"
          << "Заклинивание"
          << "Останов по превышению напряжения на батарее конденсаторов"
          << "Останов по снижению напряжения на батарее конденсаторов"
          << "Останов по сигналу с цифрового входа 1"
          << "Останов по сигналу с цифрового входа 2"
          << "Останов по сигналу с цифрового входа 3"
          << "Останов по сигналу с цифрового входа 4"
          << "Останов по сигналу с цифрового входа 5"
          << "Останов по сигналу с цифрового входа 6"
          << "Авария IGBT1 модуля"
          << "Авария IGBT2 модуля"
          << "Авария IGBT3 модуля"
          << "Авария IGBT4 модуля"
          << "Авария IGBT5 модуля"
          << "Авария IGBT6 модуля"
          << "Останов по перегрузке IGBT модулей"
          << "Авария IGBT модулей"
          << "Неисправность датчиков температуры на охладителе силовых модулей"
          << "Неисправность датчиков температуры контроллера"
          << "Останов по недопустимо низкой температуре в контроллере"
          << "Останов по превышению температуры охладителя силовых модулей"
          << "Запрящение включения двигателя при низкой температуре охладителя"
          << "Сбой синхросигнала от сети: отсутствие одной из фаз или сильные помехи"
          << "Останов по превышению напряжения на батарее конденсаторов"
          << "Резерв"
          << "Останов по превышению мгновенного значения выходного тока ПЧ lu"
          << "Останов по превышению мгновенного значения выходного тока ПЧ lv"
          << "Останов по превышению тока Id в звене постоянного напряжения (аппаратно)"
          << "Останов по превышению напряжения Ud на батарее конденсаторов"
          << "Останов по недопустимому снижению напряжения питания контроллера"
          << "Останов по минимальному значению частоты ПЧ"
          << "Останов по максимальному значению частоты ПЧ"
          << "Батарея конденсаторов не заряжается или напряжение на ней меньше минимума"
          << "Нет связи между КСУ и КПЧ"
          << "Программный сбой КПЧ"
          << "Отказ станции управления"
          << "Перегрев радиаторов станции управления"
          << "Резерв"
          << "Защита от нестабильной работы (от гидродинамических ударов - защита насоса)";

    for (int i = 95; i < 256; i++) {
        codes << i;
        names << QStringLiteral("Резерв");
    }

    query.addBindValue(codes);
    query.addBindValue(names);

    query.execBatch();
    db.commit();
}

void ProtocolDialogMapper::fillModbusKsuArchiveEventClasses(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO ModbusArchivesEventClasses VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    for (int i = 1; i < 9; i++)
        codes << i;
    query.addBindValue(codes);

    QVariantList names;
    names << "Управление СУ"
          << "Работа оператора"
          << "Изменение комплектации СУ"
          << "События по питанию электрической сети"
          << "События Самописца"
          << "Связь с внешними устройствами"
          << "Состояние защит"
          << "Состояние оборудования";

    query.addBindValue(names);
    query.execBatch();

    db.commit();
}

void ProtocolDialogMapper::fillModbusKsuArchiveEventCodes(int protocolCode)
{
    QSqlDatabase db = DBConfig::instance()->configDatabase();
    db.transaction();

    QSqlQuery query(db);
    QString sqlString = QString("INSERT INTO ModbusArchivesEventCodes VALUES (%1, ?, ?)").arg(protocolCode);
    query.prepare(sqlString);

    QVariantList codes;
    codes << 0x0100
          << 0x0200
          << 0x0300
          << 0x0301
          << 0x0302
          << 0x0303
          << 0x0400
          << 0x0401
          << 0x0500
          << 0x0600
          << 0x0700
          << 0x0802
          << 0x0803
          << 0x0804
          << 0x0805
          << 0x0806
          << 0x0808
          << 0x0809
          << 0x080A
          << 0x080B
          << 0x080C
          << 0x080D
          << 0x0814
          << 0x0815
          << 0x081A
          << 0x0829
          << 0x081B
          << 0x082A
          << 0x0836
          << 0x0837
          << 0x0855
          << 0x0900

             ///<< ? нужно ли это ? (Задержка АПВ после срабатывания защиты)
          << 0x0902
          << 0x0903
          << 0x0904
          << 0x0905
          << 0x0906
          << 0x0908
          << 0x0909
          << 0x090A
          << 0x090B
          << 0x090C
          << 0x090D
          << 0x0914
          << 0x0915
          << 0x091A
          << 0x0929
          << 0x091B
          << 0x092A
          << 0x0936
          << 0x0937
          << 0x0955
             ///>> ? нужно ли это?


          << 0x0A00
          << 0x0B00
          << 0x0C00
          << 0x0D00
          << 0x0E00
          << 0x0F00

          << 0x1000
          << 0x1001
          << 0x1002
          << 0x1100
          << 0x1101
          << 0x1102
          << 0x1200
          << 0x1300
          << 0x1301
          << 0x1302
          << 0x1400
          << 0x1500
          << 0x1600
          << 0x1700
          << 0x1800
          << 0x1900
          << 0x1A00
          << 0x1B00
          << 0x1C00
          << 0x1D00
          << 0x1E00
          << 0x1F00
          << 0x2000
          << 0x2100
          << 0x2101
          << 0x2200
          << 0x2201
          << 0x2300
          << 0x2400
          << 0x2401
          << 0x2500
          << 0x2501
          << 0x2600
          << 0x2601
          << 0x2700
          << 0x2800
          << 0x2900
          << 0x2A00
          << 0x2B00
          << 0x2C00
          << 0x2D00
          << 0x2E00
          << 0x2F00
          << 0x3000
          << 0x3100
          << 0x3200
          << 0x3300
          << 0x3400
          << 0x3500
          << 0x3600
          << 0x3700
          << 0x3800
          << 0x3900
          << 0x3A00
          << 0x3B00
          << 0x3C00
          << 0x3D00
          << 0x3E00
          << 0x3F00
          << 0x4000
          << 0x4100
          << 0x4200
          << 0x4300
             ;

    query.addBindValue(codes);

    QVariantList names;
    names << "Пуск" ///<< 0x0100
          << "Останов" ///<< 0x200
          << "Изменение режима работы СУ: останов" ///<< 0x0300
          << "Изменение режима работы СУ: ручной" ///<< 0x0301
          << "Изменение режима работы СУ: автоматический" ///<< 0x0302
          << "Изменение режима работы СУ: по программе" ///<< 0x0303
          << "Блокировка КСУ: по количеству АПВ" ///<< 0x0400
          << "Блокировка КСУ: дистанционно" ///<< 0x0401
          << "Деблокировка КСУ" ///<< 0x0500
          << "Обнуление счетчиков ограничения количества АПВ" ///<< 0x0600
          << "Резерв" ///<< 0x0700
          << "Задержка срабатывания защиты при работающем ПЭД: сопротивление изоляции ниже допутимого" ///<< 0x0802
          << "Задержка срабатывания защиты при работающем ПЭД: недогрузка" ///<< 0x0803
          << "Задержка срабатывания защиты при работающем ПЭД: перегрузка" ///<< 0x0804
          << "Задержка срабатывания защиты при работающем ПЭД: низкое U фазы" ///<< 0x0805
          << "Задержка срабатывания защиты при работающем ПЭД: высокое U фазы" ///<< 0x0806
          << "Задержка срабатывания защиты при работающем ПЭД: сработал контактный манометр" ///<< 0x0808
          << "Задержка срабатывания защиты при работающем ПЭД: взлом станции" ///<< 0x0809
          << "Задержка срабатывания защиты при работающем ПЭД: min P на приеме насоса" ///<< 0x080A
          << "Задержка срабатывания защиты при работающем ПЭД: max T обмотки ПЭД" ///<< 0x080B
          << "Задержка срабатывания защиты при работающем ПЭД: вибрация" ///<< 0x080C
          << "Задержка срабатывания защиты при работающем ПЭД: вибрация" ///<< 0x080D
          << "Задержка срабатывания защиты при работающем ПЭД: дисбаланс I" ///<< 0x0814
          << "Задержка срабатывания защиты при работающем ПЭД: дисбаланс U" ///<< 0x0815
          << "Задержка срабатывания защиты при работающем ПЭД: cигнал с дополнительного аналогового входа 1" ///<< 0x081A
          << "Задержка срабатывания защиты при работающем ПЭД: cигнал с дополнительного аналогового входа 1" ///<< 0x0829
          << "Задержка срабатывания защиты при работающем ПЭД: cигнал с дополнительного аналогового входа 2" ///<< 0x081B
          << "Задержка срабатывания защиты при работающем ПЭД: cигнал с дополнительного аналогового входа 2" ///<< 0x082A
          << "Задержка срабатывания защиты при работающем ПЭД: min F" ///<< 0x0836
          << "Задержка срабатывания защиты при работающем ПЭД: max F" ///<< 0x0837
          << "Задержка срабатывания защиты при работающем ПЭД: нет 0-ТМПН" ///<< 0x0855

          << "Задержка АПВ после срабатывания защиты" ///<< 0x0900

             ///<< ? нужно ли это ? (Задержка АПВ после срабатывания защиты)
          << "Задержка АПВ после срабатывания защиты: сопротивление изоляции ниже допутимого" ///<< 0x0902
          << "Задержка АПВ после срабатывания защиты: недогрузка" ///<< 0x0903
          << "Задержка АПВ после срабатывания защиты: перегрузка" ///<< 0x0904
          << "Задержка АПВ после срабатывания защиты: низкое U фазы" ///<< 0x0905
          << "Задержка АПВ после срабатывания защиты: высокое U фазы" ///<< 0x0906
          << "Задержка АПВ после срабатывания защиты: сработал контактный манометр" ///<< 0x0908
          << "Задержка АПВ после срабатывания защиты: взлом станции" ///<< 0x0909
          << "Задержка АПВ после срабатывания защиты: min P на приеме насоса" ///<< 0x090A
          << "Задержка АПВ после срабатывания защиты: max T обмотки ПЭД" ///<< 0x090B
          << "Задержка АПВ после срабатывания защиты: вибрация" ///<< 0x090C
          << "Задержка АПВ после срабатывания защиты: вибрация" ///<< 0x090D
          << "Задержка АПВ после срабатывания защиты: дисбаланс I" ///<< 0x0914
          << "Задержка АПВ после срабатывания защиты: дисбаланс U" ///<< 0x0915
          << "Задержка АПВ после срабатывания защиты: cигнал с дополнительного аналогового входа 1" ///<< 0x091A
          << "Задержка АПВ после срабатывания защиты: cигнал с дополнительного аналогового входа 1" ///<< 0x0929
          << "Задержка АПВ после срабатывания защиты: cигнал с дополнительного аналогового входа 2" ///<< 0x091B
          << "Задержка АПВ после срабатывания защиты: cигнал с дополнительного аналогового входа 2" ///<< 0x092A
          << "Задержка АПВ после срабатывания защиты: min F" ///<< 0x0936
          << "Задержка АПВ после срабатывания защиты: max F" ///<< 0x0937
          << "Задержка АПВ после срабатывания защиты: нет 0-ТМПН" ///<< 0x0955
             ///>> ? нужно ли это?


          << "Резерв" ///<< 0x0A00
          << "Резерв" ///<< 0x0В00
          << "Открытие двери" ///<< 0x0C00
          << "Изменение параметра" ///<< 0x0D00
          << "Удаление хронологии" ///<< 0x0E00
          << "Считывание хронологии" ///<< 0x0F00

          << "Перепрограммирование КСУ: инициализация" ///<< 0x1000
          << "Перепрограммирование КСУ: отменено" ///<< 0x1001
          << "Перепрограммирование КСУ: завершено успешно" ///<< 0x1002
          << "Перепрограммирование ТМСН: инициализация" ///<< 0x1100
          << "Перепрограммирование ТМСН: отменено" ///<< 0x1101
          << "Перепрограммирование ТМСН: завершено успешно" ///<< 0x1102
          << "Сброс счетчиков наработки ПЭД" ///<< 0x1200
          << "Перепрограммирование КПЧ (УПП): инициализация" ///<< 0x1300
          << "Перепрограммирование КПЧ (УПП): отменено" ///<< 0x1301
          << "Перепрограммирование КПЧ (УПП): завершено успешно" ///<< 0x1302
          << "Подключение устройства" ///<< 0x1400
          << "Отключение устройства" ///<< 0x1500
          << "Резерв" ///<< 0x1600
          << "Резерв" ///<< 0x1700
          << "Появление питания электрической сети" ///<< 0x1800
          << "Обрыв питания электрической сети" ///<< 0x1900
          << "Резерв" ///<< 0x1A00
          << "Резерв" ///<< 0x1B00
          << "Запись измерений в ускоренном режиме" ///<< 0x1C00
          << "Запись измерений в нормальном режиме" ///<< 0x1D00
          << "Пусковая характеристика ПЭД" ///<< 0x1E00
          << "Переход на зимнее/летнее время" ///<< 0x1F00
          << "Запись параметров ТМС" ///<< 0x2000
          << "Связь с ТМС: связь есть" ///<< 0x2100
          << "Связь с ТМС: связи нет" ///<< 0x2101
          << "Связь с УПП: связь есть" ///<< 0x2200
          << "Связь с УПП: связи нет" ///<< 0x2201
          << "Резерв" ///<< 0x2300
          << "Связь с ТМСП: связь есть" ///<< 0x2400
          << "Связь с ТМСП: связи нет" ///<< 0x2401
          << "Связь с КПЧ: связь есть" ///<< 0x2500
          << "Связь с КПЧ: связи нет" ///<< 0x2501
          << "Связь с СЭН: связь есть" ///<< 0x2600
          << "Связь с СЭН: связи нет" ///<< 0x2601
          << "Резерв" ///<< 0x2700
          << "Резерв" ///<< 0x2800
          << "Резерв" ///<< 0x2900
          << "Резерв" ///<< 0x2A00
          << "Резерв" ///<< 0x2B00
          << "Турбинное вращение" ///<< 0x2C00
          << "Чередование фаз" ///<< 0x2D00
          << "Резерв" ///<< 0x2E00
          << "Резерв" ///<< 0x2F00
          << "Поломка контактора" ///<< 0x3000
          << "Взлом станции" ///<< 0x3100
          << "Резерв" ///<< 0x3200
          << "Резерв" ///<< 0x3300
          << "Резерв" ///<< 0x3400
          << "Резерв" ///<< 0x3500
          << "Резерв" ///<< 0x3600
          << "Резерв" ///<< 0x3700
          << "Резерв" ///<< 0x3800
          << "Резерв" ///<< 0x3900
          << "Резерв" ///<< 0x3A00
          << "Резерв" ///<< 0x3B00
          << "Резерв" ///<< 0x3C00
          << "Резерв" ///<< 0x3D00
          << "Резерв" ///<< 0x3E00
          << "Резерв" ///<< 0x3F00
          << "Резерв" ///<< 0x4000
          << "Резерв" ///<< 0x4100
          << "Резерв" ///<< 0x4200
          << "Сбой часов реального времени" ///<< 0x4300
             ;

    query.addBindValue(names);

    query.execBatch();
    db.commit();
}
