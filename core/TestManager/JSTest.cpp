#include "JSTest.h"
#include "JSTestStep.h"
#include "IOServer/IOServer.h"
#include "IOServer/nsVariable/IVariable.h"
#include "Common/QMessageBox/ScriptDelayMessageBox/ScriptDelayMessageBox.h"
#include "EditorWindow/EngineLocalVariable.h"
#include "TestManager/ExecutorWindow/TestCreator.h"
#include "Authentication/SystemUser.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "DBConfig/DBConfig.h"
#include "JSTestScriptEngine.h"

#include "IOServer/nsNode/Ksu735Node.h"
#include "IOServer/nsNode/Ksu735NodeArchive.h"
#include "IOServer/nsVariable/Ksu735Variable.h"
#include "IOServer/nsVariable/ModbusVariable.h"
#include "ModbusCrc.h"

#include "IOServer/nsNode/ModbusKsuNode.h"
#include "IOServer/nsNode/ModbusKsuArchive/IModbusKsuArchive.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuCurrentParametrsArchive.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuChangeParametrsArchive.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuEventsArchive.h"
#include "IOServer/nsNode/ModbusKsuArchive/ModbusKsuLaunchParametrsArchive.h"
#include "IOServer/nsConnection/KsuSerialPortConnection.h"

#include "ManualTesting/Ksu735/ManualFullTesting/Ksu735ManualFullTestingWidget.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualFullTesting/ModbusKsuManualFullTestingWidget.h"

#include "ErrorClassifier/ErrorClassifier.h"

// Сообщение с вводом значения
static QScriptValue inputMessageInt(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 2)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        QInputDialog messageBox;

        messageBox.setInputMode(QInputDialog::IntInput);
        messageBox.setIntMinimum(0);
        messageBox.setIntMaximum(2147483647);
        messageBox.setIntValue(context->argument(0).toInteger());
        messageBox.setLabelText(context->argument(1).toString());

        messageBox.setFont(QFont(context->toString(), 24));

#ifdef QT_DEBUG
        messageBox.setWindowModality(Qt::NonModal);
#endif

        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(messageBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        messageBox.setMinimumWidth(800);

        int ret = messageBox.exec();

        if (ret == QDialog::Accepted) {
            return engine->newVariant(QVariant(messageBox.intValue()));
        } else {
            return engine->newVariant(QVariant(context->argument(0).toInteger()));
        }
    }

    return engine->undefinedValue();
}


// Проверка состояния значения заданное время
static QScriptValue waitConditionDelayMessage(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 4)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        ScriptDelayMessageBox messageBox(context->argument(0).toString(),
                                         context->argument(1).toVariant(),
                                         context->argument(2).toString(),
                                         context->argument(3).toInt32());

        messageBox.button(QMessageBox::Abort)->setText("Отмена");

        messageBox.setIconPixmap(QPixmap(":/Images/dialog-information.png"));
        messageBox.setFont(QFont(context->toString(), 24));

#ifdef QT_DEBUG
        messageBox.setWindowModality(Qt::NonModal);
#endif
        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(messageBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        messageBox.setMinimumWidth(800);

        int ret = messageBox.exec();

        return engine->newVariant(QVariant(ret));

    }

    return engine->undefinedValue();
}

// Проверка состояния значения
static QScriptValue waitConditionMessage(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 3)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        ScriptDelayMessageBox messageBox(context->argument(0).toString(),
                                         context->argument(1).toVariant(),
                                         context->argument(2).toString());

        messageBox.button(QMessageBox::Abort)->setText("Отмена");

        messageBox.setIconPixmap(QPixmap(":/Images/operator.png"));
        messageBox.setFont(QFont(context->toString(), 24));


#ifdef QT_DEBUG
        messageBox.setWindowModality(Qt::NonModal);
#endif

        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(messageBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        messageBox.setMinimumWidth(800);

        int ret = messageBox.exec();

        return engine->newVariant(QVariant(ret));
    }

    return engine->undefinedValue();
}

// Бесконечная пауза
static QScriptValue showMessage(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        QMessageBox msgBox;
        msgBox.setWindowTitle("Функция бесконечной паузы");
#ifdef QT_DEBUG
        msgBox.setWindowModality(Qt::WindowModal);
#endif
        msgBox.setText(context->argument(0).toString());
        msgBox.setWindowIcon(QIcon(":/Images/irz_logo.png"));
        msgBox.setIconPixmap(QPixmap(":/Images/operator.png"));
        msgBox.setFont(QFont(context->toString(), 24));

        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        msgBox.setMinimumWidth(800);
        msgBox.setDefaultButton(QMessageBox::No);
        msgBox.exec();
    }
    return engine->undefinedValue();
}

// Принитие решения, выбор состояни
static QScriptValue showQuestion(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        QMessageBox msgBox;
        msgBox.setWindowTitle("Выбор состояния");
#ifdef QT_DEBUG
        msgBox.setWindowModality(Qt::WindowModal);
#endif
        msgBox.setText(context->argument(0).toString());
        msgBox.setWindowIcon(QIcon(":/Images/irz_logo.png"));
        msgBox.setIconPixmap(QPixmap(":/Images/question_icon.png"));
        msgBox.setFont(QFont(context->toString(), 24));

        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        QPushButton *yesButton = msgBox.addButton("Да", QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton("Нет", QMessageBox::NoRole);

        msgBox.setDefaultButton(noButton);

        msgBox.exec();

        if (msgBox.clickedButton() == yesButton) {
            return engine->newVariant(QVariant(true));
        } else if (msgBox.clickedButton() == noButton) {
            return engine->newVariant(QVariant(false));
        }
    }

    return engine->undefinedValue();
}

// Прерывание теста c вопросом
static QScriptValue abortEvaluationQuestion(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() > 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        QString message = QString();
        QString result = QString();

        if (context->argumentCount() != 0)
            message = context->argument(0).toString();

        if (!IOServer::instance())
            return engine->undefinedValue();

        QMessageBox msgBox;
        msgBox.setWindowTitle("Выбор состояния");
#ifdef QT_DEBUG
        msgBox.setWindowModality(Qt::NonModal);
#endif
        msgBox.setText("Продолжить выполнение теста со следующего шага?");
        msgBox.setWindowIcon(QIcon(":/Images/irz_logo.png"));

        msgBox.setIconPixmap(QPixmap(":/Images/error.png"));
        msgBox.setFont(QFont(context->toString(), 24));

        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        QPushButton *yesButton = msgBox.addButton("Продолжить", QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton("Прервать", QMessageBox::NoRole);
        msgBox.setDefaultButton(noButton);

        msgBox.exec();

        if (msgBox.clickedButton() == yesButton) {
            result = "Продолжить";
        } else if (msgBox.clickedButton() == noButton) {
            result = "Прервать";
        }

        QSqlDatabase db = DBConfig::instance()->configDatabase();

        QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                    "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, result, stepNumber) "
                                    "VALUES('abortQuestionEvaluation', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                .arg(SystemUser::instance()->user())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                .arg(message)
                .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                .arg(IOServer::instance()->getTestHistoryReport()->testName())
                .arg(IOServer::instance()->getTestHistoryReport()->stepName())
                .arg(result)
                .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());

        QSqlQuery query(db);
        db.transaction();
        query.exec(sqlString);
        db.commit();

        if (msgBox.clickedButton() == noButton) {
            engine->abortEvaluation();
            static_cast<JSTestScriptEngine *>(engine)->setAbortFlag(true);
        }
    }

    return engine->undefinedValue();
}

// Прерывание теста
static QScriptValue abortEvaluation(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 0)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        QSqlDatabase db = DBConfig::instance()->configDatabase();

        QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                    "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, result, stepNumber) "
                                    "VALUES('abortEvaluation', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9');")
                .arg(SystemUser::instance()->user())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                .arg("abortEvaluation")
                .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                .arg(IOServer::instance()->getTestHistoryReport()->testName())
                .arg(IOServer::instance()->getTestHistoryReport()->stepName())
                .arg("true")
                .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());

        QSqlQuery query(db);
        db.transaction();
        query.exec(sqlString);
        db.commit();

    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Ошибка выполнения теста");
    msgBox.setWindowModality(Qt::NonModal);
    msgBox.setText("Текущий тест прерван");
    msgBox.setWindowIcon(QIcon(":/Images/irz_logo.png"));

    msgBox.setIconPixmap(QPixmap(":/Images/error.png"));
    msgBox.setFont(QFont(context->toString(), 24));

    QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QGridLayout *layout = dynamic_cast<QGridLayout *>(msgBox.layout());
    if (layout)
        layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

    msgBox.setMinimumWidth(800);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.exec();

    engine->abortEvaluation();
    static_cast<JSTestScriptEngine *>(engine)->setAbortFlag(true);
    return engine->undefinedValue();
}

// Таймер обратного отчета
static QScriptValue delay(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 2)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        ScriptDelayMessageBox messageBox(context->argument(0).toUInt32(), context->argument(1).toString());
#ifdef QT_DEBUG
        messageBox.setWindowModality(Qt::NonModal);
#endif
        messageBox.setIconPixmap(QPixmap(":/Images/watch.png"));
        messageBox.setFont(QFont(context->toString(), 24));

        QSpacerItem *horizontalSpacer = new QSpacerItem(800, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
        QGridLayout *layout = dynamic_cast<QGridLayout *>(messageBox.layout());
        if (layout)
            layout->addItem(horizontalSpacer, layout->rowCount(), 0, 1, layout->columnCount());

        messageBox.setMinimumWidth(800);

        messageBox.exec();
    }

    return engine->undefinedValue();
}

static QScriptValue hasVariable(QScriptContext *context, QScriptEngine *engine) {
    if (context->argumentCount() != 1)
        return engine->newVariant(false);

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->newVariant(false);

        IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());

        if (!variable) {
            return engine->newVariant(false);
        }

        return engine->newVariant(true);
    }

    return engine->newVariant(false);
}

// Чтение параметра КСУ
static QScriptValue getValue(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());

        if (!variable)
            return engine->nullValue();

        QVariant var = variable->readValue();

        return engine->newVariant(var);
    }
    return engine->undefinedValue();
}

// Проверка связи с КСУ
static QScriptValue checkKsuLink(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 0)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        KsuSerialPortConnection *connection = IOServer::instance()->ksuConnection();
        ModbusNode *node = static_cast<ModbusNode*>(IOServer::instance()->ksuNode());
        ModbusVariable *var = static_cast<ModbusVariable*>(IOServer::instance()->variableByAlias("KSU_STATE_PED"));
        QByteArray request;
        request.append(node->address());
        request.append(0x04);
        request.append(var->address() >> 8);
        request.append(var->address());
        request.append(QChar(0x00));
        request.append(0x01);
        request.append(ModbusCrc::getInstance()->calcCrc(request, 0, 6));
        QByteArray response = connection->sendData(request, 1000);
        if (response.size() < 5 || !ModbusCrc::getInstance()->exactMatch(response)) {
            return engine->newVariant(QVariant(false));
        } else {
            return engine->newVariant(QVariant(true));
        }

    }

    return engine->undefinedValue();
}


// Запись параметра КСУ
static QScriptValue setValue(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 2)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());

        if (!variable) {
            return engine->nullValue();
        }

        variable->setValue(context->argument(1).toVariant());

        return engine->newVariant(variable->readValue());
    }

    return engine->undefinedValue();
}

static QScriptValue parametrAddress(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return engine->undefinedValue();

    IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());
    if (!variable)
        return engine->nullValue();

    Ksu735Variable *ksu735Variable = qobject_cast<Ksu735Variable *>(variable);
    if (ksu735Variable)
        return engine->newVariant(ksu735Variable->address());

    ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(variable);
    if (modbusVariable)
        return engine->newVariant(modbusVariable->address());

    return engine->nullValue();
}

static QScriptValue variableMetaInfo(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return engine->undefinedValue();

    IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());
    if (!variable)
        return engine->nullValue();

    return engine->newQObject(variable, QScriptEngine::QtOwnership);
}

// Запись в отчет пользовательского сообщения messageReport
static QScriptValue messageReport(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        QSqlDatabase db = DBConfig::instance()->configDatabase();

        QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                    "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, stepNumber) "
                                    "VALUES('messageReport', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8');")
                .arg(SystemUser::instance()->user())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                .arg(context->argument(0).toString())
                .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                .arg(IOServer::instance()->getTestHistoryReport()->testName())
                .arg(IOServer::instance()->getTestHistoryReport()->stepName())
                .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());

        QSqlQuery query(db);
        db.transaction();
        query.exec(sqlString);
        db.commit();
    }

    return engine->undefinedValue();
}

// Запись в отчет значения параметра сообщения valueReport
static QScriptValue valueReport(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() < 2 || context->argumentCount() > 3)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        QString aliasName = context->argument(0).toString();
        QString aliasValue = context->argument(1).toVariant().toString();

        QString message = QString();

        if (context->argumentCount() == 3)
            message = context->argument(2).toString();

        QSqlDatabase db = DBConfig::instance()->configDatabase();

        QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                    "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, aliasName, aliasValue, stepNumber) "
                                    "VALUES('valueReport', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10');")
                .arg(SystemUser::instance()->user())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuType())
                .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())
                .arg(message)
                .arg(IOServer::instance()->getTestHistoryReport()->testKitName())
                .arg(IOServer::instance()->getTestHistoryReport()->testName())
                .arg(IOServer::instance()->getTestHistoryReport()->stepName())
                .arg(aliasName)
                .arg(aliasValue)
                .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());

        QSqlQuery query(db);
        db.transaction();
        query.exec(sqlString);
        db.commit();
    }

    return engine->undefinedValue();
}

// Результат выполнения шага у теста
static QScriptValue stepReport(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        bool result = context->argument(0).toBool();

        QString resultString = "true";
        if (!result)
            resultString = "false";

        QSqlDatabase db = DBConfig::instance()->configDatabase();

        QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                    "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, aliasName, aliasValue, result, stepNumber) "
                                    "VALUES('stepReport', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11');")
                .arg(SystemUser::instance()->user())                                        /// %1 user
                .arg(IOServer::instance()->getTestHistoryReport()->ksuType())               /// %2 ksuName
                .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())    /// %3 softwareVersion
                .arg("")                                                                    /// %4 message
                .arg(IOServer::instance()->getTestHistoryReport()->testKitName())           /// %5 kitName
                .arg(IOServer::instance()->getTestHistoryReport()->testName())              /// %6 testName
                .arg(IOServer::instance()->getTestHistoryReport()->stepName())              /// %7 stepName
                .arg("")                                                                    /// %8 aliasName
                .arg("")                                                                    /// %9 aliasValue
                .arg(resultString)                                                          /// %10 result
                .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());           /// %11 stepNumber

        QSqlQuery query(db);
        db.transaction();
        query.exec(sqlString);
        db.commit();
    }

    QApplication::processEvents(QEventLoop::AllEvents, 500);

    return engine->undefinedValue();
}

static QScriptValue readKsu735Archive(QScriptContext *context, QScriptEngine *engine) {
    if (context->argumentCount() > 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();
    Ksu735Node *ksuNode = qobject_cast<Ksu735Node *>(node);
    if (!ksuNode)
        return engine->undefinedValue();

    int offset = 1;
    if (context->argumentCount() == 1) {
        offset = context->argument(0).toUInt16();
    }

    bool ok;
    Ksu735NodeArchive *archive = ksuNode->readArchive(offset, &ok);

    if (!ok)
        return engine->nullValue();

    if (!archive || archive->isEmpty() || !archive->isValid())
        return engine->nullValue();

    return engine->newQObject(archive, QScriptEngine::ScriptOwnership);
}

static QScriptValue readModbusKsuCurrentParametrsArchive(QScriptContext *context, QScriptEngine *engine) {
    if (context->argumentCount() > 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();
    ModbusKsuNode *ksuNode = qobject_cast<ModbusKsuNode *>(node);
    if (!ksuNode)
        return engine->undefinedValue();

    int offset = 0;
    if (context->argumentCount() == 1) {
        offset = context->argument(0).toUInt16();
    }

    IModbusKsuArchive::ModbusKsuArchiveType archiveType = IModbusKsuArchive::ModbusKsuArchiveType::CurrentParametrsArchive;

    if (!ksuNode->moveArchiveRecordToZeroPosition(archiveType)) {
        return engine->nullValue();
    }
    if (offset != 0) {
        if (!ksuNode->moveArchiveRecordToNPosition(offset, archiveType)) {
            return engine->nullValue();
        }
    }

    bool ok;
    IModbusKsuArchive *archiveRecord = ksuNode->readArchive(archiveType, &ok);

    if (!archiveRecord || archiveRecord->isEmpty())
        engine->nullValue();

    if (!ok)
        return engine->nullValue();

    ModbusKsuCurrentParametrsArchive *archive = qobject_cast<ModbusKsuCurrentParametrsArchive *>(archiveRecord);

    if (!archive)
        return engine->nullValue();

    return engine->newQObject(archive, QScriptEngine::ScriptOwnership);
}

static QScriptValue readModbusKsuChangeParametrsArchive(QScriptContext *context, QScriptEngine *engine) {
    if (context->argumentCount() > 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();
    ModbusKsuNode *ksuNode = qobject_cast<ModbusKsuNode *>(node);
    if (!ksuNode)
        return engine->undefinedValue();

    int offset = 0;
    if (context->argumentCount() == 1) {
        offset = context->argument(0).toUInt16();
    }

    IModbusKsuArchive::ModbusKsuArchiveType archiveType = IModbusKsuArchive::ModbusKsuArchiveType::ChangeParametrsArchive;

    if (!ksuNode->moveArchiveRecordToZeroPosition(archiveType)) {
        return engine->nullValue();
    }
    if (offset != 0) {
        if (!ksuNode->moveArchiveRecordToNPosition(offset, archiveType)) {
            return engine->nullValue();
        }
    }

    bool ok;
    IModbusKsuArchive *archiveRecord = ksuNode->readArchive(archiveType, &ok);

    if (!archiveRecord || archiveRecord->isEmpty())
        engine->nullValue();

    if (!ok)
        return engine->nullValue();

    ModbusKsuChangeParametrsArchive *archive = qobject_cast<ModbusKsuChangeParametrsArchive *>(archiveRecord);

    if (!archive)
        return engine->nullValue();

    return engine->newQObject(archive, QScriptEngine::ScriptOwnership);
}

static QScriptValue readModbusKsuEventsArchive(QScriptContext *context, QScriptEngine *engine) {
    if (context->argumentCount() > 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();
    ModbusKsuNode *ksuNode = qobject_cast<ModbusKsuNode *>(node);
    if (!ksuNode)
        return engine->undefinedValue();

    int offset = 0;
    if (context->argumentCount() == 1) {
        offset = context->argument(0).toUInt16();
    }

    IModbusKsuArchive::ModbusKsuArchiveType archiveType = IModbusKsuArchive::ModbusKsuArchiveType::EventsArchive;

    if (!ksuNode->moveArchiveRecordToZeroPosition(archiveType)) {
        return engine->nullValue();
    }
    if (offset != 0) {
        if (!ksuNode->moveArchiveRecordToNPosition(offset, archiveType)) {
            return engine->nullValue();
        }
    }

    bool ok;
    IModbusKsuArchive *archiveRecord = ksuNode->readArchive(archiveType, &ok);

    if (!archiveRecord || archiveRecord->isEmpty())
        engine->nullValue();

    if (!ok)
        return engine->nullValue();

    ModbusKsuEventsArchive *archive = qobject_cast<ModbusKsuEventsArchive *>(archiveRecord);

    if (!archive)
        return engine->nullValue();

    return engine->newQObject(archive, QScriptEngine::ScriptOwnership);
}

static QScriptValue readModbusKsuLaunchParametrsArchive(QScriptContext *context, QScriptEngine *engine) {
    if (context->argumentCount() > 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();
    ModbusKsuNode *ksuNode = qobject_cast<ModbusKsuNode *>(node);
    if (!ksuNode)
        return engine->undefinedValue();

    int offset = 0;
    if (context->argumentCount() == 1) {
        offset = context->argument(0).toUInt16();
    }

    IModbusKsuArchive::ModbusKsuArchiveType archiveType = IModbusKsuArchive::ModbusKsuArchiveType::LaunchParametrsArchive;

    if (!ksuNode->moveArchiveRecordToZeroPosition(archiveType)) {
        return engine->nullValue();
    }
    if (offset != 0) {
        if (!ksuNode->moveArchiveRecordToNPosition(offset, archiveType)) {
            return engine->nullValue();
        }
    }

    bool ok;
    IModbusKsuArchive *archiveRecord = ksuNode->readArchive(archiveType, &ok);

    if (!archiveRecord || archiveRecord->isEmpty())
        engine->nullValue();

    if (!ok)
        return engine->nullValue();

    ModbusKsuLaunchParametrsArchive *archive = qobject_cast<ModbusKsuLaunchParametrsArchive *>(archiveRecord);

    if (!archive)
        return engine->nullValue();

    return engine->newQObject(archive, QScriptEngine::ScriptOwnership);
}

static QScriptValue isWriteable(QScriptContext *context, QScriptEngine *engine) {

    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return engine->undefinedValue();

    IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());
    if (!variable)
        return engine->nullValue();

    Ksu735Variable *ksu735Variable = qobject_cast<Ksu735Variable *>(variable);
    if (ksu735Variable)
        return engine->newVariant(ksu735Variable->isWritable());

    ModbusVariable *modbusVariable = qobject_cast<ModbusVariable *>(variable);
    if (modbusVariable)
        return engine->newVariant(modbusVariable->isWritable());

    return engine->nullValue();
}

static QScriptValue manualFullTesting(QScriptContext *context, QScriptEngine *engine) {

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();

    bool result = false;

    ModbusKsuNode *mbNode = qobject_cast<ModbusKsuNode *>(node);
    if (mbNode) {
        ModbusKsuManualFullTestingWidget *tempMbTestingWidget = new ModbusKsuManualFullTestingWidget(mbNode);
        tempMbTestingWidget->setAutotestingmodeFlag(true);
        result = tempMbTestingWidget->autotest();
    }

    Ksu735Node *n735Node = qobject_cast<Ksu735Node *>(node);
    if (n735Node) {
        Ksu735ManualFullTestingWidget *temp735TestingWidget = new Ksu735ManualFullTestingWidget(n735Node);
        temp735TestingWidget->setAutotestingmodeFlag(true);
        result = temp735TestingWidget->autotest();
    }

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    QString sqlString = QString("INSERT INTO `TestHistoryReport` "
                                "(function, user, ksuName, softwareVersion, message, kitName, testName, stepName, aliasName, aliasValue, result, stepNumber) "
                                "VALUES('manualFullTesting', '%1', '%2', '%3', '%4', '%5', '%6', '%7', '%8', '%9', '%10', '%11');")
            .arg(SystemUser::instance()->user())                                        /// %1 user
            .arg(IOServer::instance()->getTestHistoryReport()->ksuType())               /// %2 ksuName
            .arg(IOServer::instance()->getTestHistoryReport()->ksuSoftwareVersion())    /// %3 softwareVersion
            .arg("Отчет по параметрам КСУ")                                             /// %4 message
            .arg(IOServer::instance()->getTestHistoryReport()->testKitName())           /// %5 kitName
            .arg(IOServer::instance()->getTestHistoryReport()->testName())              /// %6 testName
            .arg(IOServer::instance()->getTestHistoryReport()->stepName())              /// %7 stepName
            .arg("")                                                                    /// %8 aliasName
            .arg("")                                                                    /// %9 aliasValue
            .arg(result)                                                                /// %10 result
            .arg(IOServer::instance()->getTestHistoryReport()->stepNumber());           /// %11 stepNumber

    db.transaction();
    query.exec(sqlString);
    db.commit();

    return engine->undefinedValue();
}

static QScriptValue protocolType(QScriptContext *context, QScriptEngine *engine) {

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();

    ModbusKsuNode *mbNode = qobject_cast<ModbusKsuNode *>(node);
    if (mbNode) {
        return engine->newVariant(QString("modbus"));
    }

    Ksu735Node *n735Node = qobject_cast<Ksu735Node *>(node);
    if (n735Node) {
        return engine->newVariant(QString("ksu735"));
    }

    return engine->undefinedValue();
}

static QScriptValue readModbusPassword(QScriptContext *context, QScriptEngine *engine) {

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();

    ModbusKsuNode *mbNode = qobject_cast<ModbusKsuNode *>(node);
    if (mbNode) {

        if (context->argument(0).toString() == "1") {
            qDebug() << 1;
            return engine->newVariant(
                        QString::number(mbNode->readPassword(ModbusKsuPasswordLevel::ModbusKsuPasswordLevel1)));
        } else if (context->argument(0).toString() == "2") {

            qDebug() << 2;
            return engine->newVariant(
                        QString::number(mbNode->readPassword(ModbusKsuPasswordLevel::ModbusKsuPasswordLevel2)));
        } else {

            qDebug() << 3;
            return engine->undefinedValue();
        }
    }

    Ksu735Node *n735Node = qobject_cast<Ksu735Node *>(node);
    if (n735Node) {
        ErrorClassifier::instance()->showError(504);
        return engine->undefinedValue();
    }
    return engine->undefinedValue();
}

static QScriptValue writeModbusPassword(QScriptContext *context, QScriptEngine *engine) {

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    if (context->argumentCount() != 2)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    IOServer *ioServer = IOServer::instance();

    if (!ioServer)
        return engine->undefinedValue();

    INode *node = ioServer->ksuNode();

    ModbusKsuNode *mbNode = qobject_cast<ModbusKsuNode *>(node);
    if (mbNode) {
        if (context->argument(0).toString() == "1") {
            return engine->newVariant(
                        QString::number(mbNode->writePassword(
                                            ModbusKsuPasswordLevel::ModbusKsuPasswordLevel1,
                                            context->argument(1).toUInt16())));
        } else if (context->argument(0).toString() == "2") {
            return engine->newVariant(
                        QString::number(mbNode->writePassword(
                                            ModbusKsuPasswordLevel::ModbusKsuPasswordLevel2,
                                            context->argument(1).toUInt16())));
        } else {
            return engine->undefinedValue();
        }
    }

    Ksu735Node *n735Node = qobject_cast<Ksu735Node *>(node);
    if (n735Node) {
        ErrorClassifier::instance()->showError(504);
        return engine->undefinedValue();
    }
    return engine->undefinedValue();
}


static QScriptValue processEvents(QScriptContext *context, QScriptEngine *engine) {

    if (context->argumentCount() != 0)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    QApplication::processEvents(QEventLoop::AllEvents, 500);

    return engine->undefinedValue();
}

// Получения всех параметров КСУ
static QScriptValue ksuNodeVariables(QScriptContext *context, QScriptEngine *engine) {

    if (context->argumentCount() != 0)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (context->isCalledAsConstructor())
        return engine->undefinedValue();

    IOServer *ioServer = IOServer::instance();
    if (!ioServer)
        return engine->undefinedValue();

    QList<IVariable *> list = ioServer->ksuNode()->variables();

    QStringList names = QStringList();

    for (auto var : list) {

        if (var->alias().isEmpty() || var->alias().contains("KSU735x10", Qt::CaseSensitive))
            continue;

        names.append(var->alias());
        //qDebug() << var->alias();
    }


    return qScriptValueFromSequence(engine, names);
}

// Определения типа СУ
static QScriptValue getTypeSU(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 0)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        QString typeSu = IOServer::instance()->getTestHistoryReport()->typeSULineEdit();

        return engine->newVariant(typeSu);
    }
    return engine->undefinedValue();
}

// Определения типа KСУ
static QScriptValue getTypeKSU(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 0)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        QString typeKsu = IOServer::instance()->getTestHistoryReport()->ksuType();

        return engine->newVariant(typeKsu);
    }
    return engine->undefinedValue();
}

// Чтение параметра getMax
static QScriptValue getMax(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());

        if (!variable) {
            return engine->nullValue();
        }

        return engine->newVariant(variable->maxValue());
    }

    return engine->undefinedValue();
}




// Чтение параметра getMin
static QScriptValue getMin(QScriptContext *context, QScriptEngine *engine)
{
    if (context->argumentCount() != 1)
        return context->throwError("Не верное количество аргументов в вызываемой функции");

    if (!context->isCalledAsConstructor()) {

        if (!IOServer::instance())
            return engine->undefinedValue();

        IVariable *variable = IOServer::instance()->variableByAlias(context->argument(0).toString());

        if (!variable) {
            return engine->nullValue();
        }

        return engine->newVariant(variable->minValue());
    }

    return engine->undefinedValue();
}

JSTest::JSTest(const QString &name, const QString &path, QObject *parent)
    : ITest(parent)
{
    m_engine = new JSTestScriptEngine;

    QScriptValue showMessageScript = m_engine->newFunction(showMessage);
    QScriptValue delayScript = m_engine->newFunction(delay);
    QScriptValue hasVariableScript = m_engine->newFunction(hasVariable);
    QScriptValue getValueScript = m_engine->newFunction(getValue);
    QScriptValue setValueScript = m_engine->newFunction(setValue);
    QScriptValue parametrAddressScript = m_engine->newFunction(parametrAddress);
    QScriptValue variableMetaInfoScript = m_engine->newFunction(variableMetaInfo);
    QScriptValue showQuestionScript = m_engine->newFunction(showQuestion);
    QScriptValue abortEvaluationQuestionScript = m_engine->newFunction(abortEvaluationQuestion);
    QScriptValue messageReportScript = m_engine->newFunction(messageReport);
    QScriptValue valueReportScript = m_engine->newFunction(valueReport);
    QScriptValue stepReportScript = m_engine->newFunction(stepReport);
    QScriptValue abortEvaluationScript = m_engine->newFunction(abortEvaluation);

    QScriptValue readKsu735ArchiveScript = m_engine->newFunction(readKsu735Archive);
    QScriptValue readModbusKsuCurrentParametrsArchiveScript = m_engine->newFunction(readModbusKsuCurrentParametrsArchive);
    QScriptValue readModbusKsuChangeParametrsArchiveScript = m_engine->newFunction(readModbusKsuChangeParametrsArchive);
    QScriptValue readModbusKsuEventsArchiveScript = m_engine->newFunction(readModbusKsuEventsArchive);
    QScriptValue readModbusKsuLaunchParametrsArchiveScript = m_engine->newFunction(readModbusKsuLaunchParametrsArchive);

    QScriptValue isWriteableScript = m_engine->newFunction(isWriteable);
    QScriptValue manualFullTestingScript = m_engine->newFunction(manualFullTesting);
    QScriptValue protocolTypeScript = m_engine->newFunction(protocolType);
    QScriptValue ksuNodeVariablesScript = m_engine->newFunction(ksuNodeVariables);
    QScriptValue processEventsScript = m_engine->newFunction(processEvents);

    QScriptValue readModbusPasswordScript = m_engine->newFunction(readModbusPassword);
    QScriptValue writeModbusPasswordScript = m_engine->newFunction(writeModbusPassword);
    QScriptValue getTypeSUScript = m_engine->newFunction(getTypeSU);

    QScriptValue getMaxScript = m_engine->newFunction(getMax);
    QScriptValue getMinScript = m_engine->newFunction(getMin);
    QScriptValue waitConditionMessageScript = m_engine->newFunction(waitConditionMessage);
    QScriptValue waitConditionDelayMessageScript = m_engine->newFunction(waitConditionDelayMessage);

    QScriptValue inputMessageIntScript = m_engine->newFunction(inputMessageInt);
    QScriptValue getTypeKSUScript = m_engine->newFunction(getTypeKSU);
    QScriptValue checkKsuLinkScript = m_engine->newFunction(checkKsuLink);

    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(0).name, showMessageScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(1).name, delayScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(2).name, hasVariableScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(3).name, getValueScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(4).name, setValueScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(5).name, parametrAddressScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(6).name, variableMetaInfoScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(7).name, showQuestionScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(8).name, abortEvaluationQuestionScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(9).name, messageReportScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(10).name, valueReportScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(11).name, stepReportScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(12).name, abortEvaluationScript);

    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(13).name, readKsu735ArchiveScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(14).name, readModbusKsuCurrentParametrsArchiveScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(15).name, readModbusKsuChangeParametrsArchiveScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(16).name, readModbusKsuEventsArchiveScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(17).name, readModbusKsuLaunchParametrsArchiveScript);

    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(18).name, isWriteableScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(19).name, manualFullTestingScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(20).name, protocolTypeScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(21).name, ksuNodeVariablesScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(22).name, processEventsScript);

    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(23).name, readModbusPasswordScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(24).name, writeModbusPasswordScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(25).name, getTypeSUScript);

    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(26).name, getMinScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(27).name, getMaxScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(28).name, waitConditionMessageScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(29).name, waitConditionDelayMessageScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(30).name, inputMessageIntScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(31).name, getTypeKSUScript);
    m_engine->globalObject().setProperty(TestCreator::instance()->funcList().at(32).name, checkKsuLinkScript);

    setName(name);

    loadTestConfiguration(path);
    traverseXmlNodes(m_xmlTest);
    setSteps(m_steps);
}

JSTest::~JSTest()
{

}

JSTestScriptEngine *JSTest::engine() const
{
    return m_engine;
}

bool JSTest::abortFlag() const
{
    return m_engine->abortFlag();
}

bool JSTest::loadTestConfiguration(const QString &testPath)
{
    QFile file(testPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << tr("Не удается открыть файл теста");
        return false;
    }

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!m_xmlTest.setContent(file.readAll(), &errorMsg, &errorLine, &errorColumn)) {
        qCritical() << tr("Конфигурационный файл поврежден") << errorMsg << errorLine << errorColumn;
        file.close();
        return false;
    }

    file.close();

    return true;

}

void JSTest::traverseXmlNodes(const QDomNode &node)
{
    QDomNode domNode = node.firstChild();

    while (!domNode.isNull()) {
        if (domNode.isElement()) {

            QDomElement domElement = domNode.toElement();

            if (!domElement.isNull()) {

                if (domElement.tagName() == "ExternalVariable") {
                    IVariable *var = IOServer::instance()->variableByAlias(domElement.attribute("Alias"));
                    if (var)
                        m_externalVariables.append(var);
                }

                else if (domElement.tagName() == "InternalVariable") {
                    EngineLocalVariable *var = new EngineLocalVariable();
                    var->setName(domElement.attribute("Name"));
                    var->setDescription(domElement.attribute("Description"));
                    var->setWritable(QVariant(domElement.attribute("Writable")).toBool());
                    m_engine->globalObject().setProperty(var->name(), m_engine->newQObject((QObject*)var));
                    //                    connect(var, &EngineLocalVariable::valueChanged, [this](EngineLocalVariable *var)
                    //                    {qDebug()<<"qqqqq";
                    //                       if (m_engine->globalObject().property(var->name()).toVariant() != var->value())
                    //                           m_engine->globalObject().setProperty(var->name(), QScriptValue(var->value().toString()));

                    //                    });
                    // qDebug()<<m_engine->evaluate("local1.value").toInt32();
                    // m_engine->globalObject().setProperty(var->name(), m_engine->undefinedValue());
                    // auto i = static_cast<QScriptEngine::FunctionSignature>(var->writeScriptValue);

                    //  m_engine->globalObject().setProperty(var->name(), m_engine->newFunction(setInternalValue), QScriptValue::PropertySetter);
                    //qDebug()<<m_engine->globalObject().property(var->name()).toInt32();
                    //                    connect(var, &EngineLocalVariable::valueChanged, [this](EngineLocalVariable *var)
                    //                    {
                    //                        m_engine->globalObject().setProperty(var->name(), QScriptValue(var->value().toString()));
                    //                    });
                    m_internalVariables.append(var);
                }

                else if (domElement.tagName() == "Begin") {

                    setBeginStep(new JSTestStep(m_engine));
                    beginStep()->setName(domElement.attribute("Name"));
                    beginStep()->setTextProgram(domElement.text());
                    m_steps.append(beginStep());

                }

                else if (domElement.tagName() == "Step") {

                    JSTestStep *step = new JSTestStep(m_engine);
                    step->setName(domElement.attribute("Name"));
                    step->setTextProgram(domElement.text());
                    m_steps.append(step);

                }

                else if (domElement.tagName() == "End") {

                    setEndStep(new JSTestStep(m_engine));
                    endStep()->setName(domElement.attribute("Name"));
                    endStep()->setTextProgram(domElement.text());
                    m_steps.append(endStep());

                }


            }

        }

        traverseXmlNodes(domNode);
        domNode = domNode.nextSibling();
    }
}
