#include "CommunicationProtocolClassifiers.h"
#include "KsuCommunicationProtocolClassifiers/WorkMode/WorkMode.h"
#include "KsuCommunicationProtocolClassifiers/KindOfLaunch/KindOfLaunch.h"
#include "KsuCommunicationProtocolClassifiers/StopReasonCodes/StopReasonCodes.h"
#include "KsuCommunicationProtocolClassifiers/StatusWordInterpretationCodes/StatusWordInterpretationCodes.h"
#include "KsuCommunicationProtocolClassifiers/ErrorCodes/ErrorCodes.h"
#include "KsuCommunicationProtocolClassifiers/ReturnFlags10/ReturnFlags10.h"
#include "KsuCommunicationProtocolClassifiers/ModbusArchivesEventClasses/ModbusArchivesEventClasses.h"
#include "KsuCommunicationProtocolClassifiers/ModbusArchivesEventCodes/ModbusArchivesEventCodes.h"

CommunicationProtocolClassifiers::CommunicationProtocolClassifiers(int comProtocolId, SapKsu::KSUProtocolType protocolType, QObject *parent)
{
    if (protocolType == SapKsu::KSUProtocolType::Ksu735) {

        WorkMode *workMode = new WorkMode(comProtocolId, this);
        KindOfLaunch *kindOfLaunch = new KindOfLaunch(comProtocolId, this);
        StopReasonCodes *stopReasonCodes = new StopReasonCodes(comProtocolId, this);
        StatusWordInterpretationCodes *statusWordInterpretationCodes = new StatusWordInterpretationCodes(comProtocolId, this);
        ErrorCodes *errorCodes = new ErrorCodes(comProtocolId, this);
        ReturnFlags10 *returnFlags10 = new ReturnFlags10(comProtocolId, this);

        addTab(returnFlags10, "Флаги возвращаемых параметров функции 0x10");
        addTab(statusWordInterpretationCodes, "Расшифровка слова состояния");
        addTab(workMode, "Режим работы");
        addTab(kindOfLaunch, "Вид запуска");
        addTab(stopReasonCodes, "Первопричины останова");
        addTab(errorCodes, "Коды ошибок");
    } else if (protocolType == SapKsu::KSUProtocolType::Modbus) {
        KindOfLaunch *kindOfLaunch = new KindOfLaunch(comProtocolId, this);
        StopReasonCodes *stopReasonCodes = new StopReasonCodes(comProtocolId, this);
        ModbusArchivesEventClasses *eventClasses = new ModbusArchivesEventClasses(comProtocolId, this);
        ModbusArchivesEventCodes *eventCodes = new ModbusArchivesEventCodes(comProtocolId, this);

        addTab(kindOfLaunch, QStringLiteral("Вид запуска ПЭД"));
        addTab(stopReasonCodes, QStringLiteral("Коды первопричины останова"));
        addTab(eventClasses, QStringLiteral("Классы событий в журнале хронологии КСУ"));
        addTab(eventCodes, QStringLiteral("Коды событий в журнале хронологии КСУ"));
    }

    resize(1024, 768);

    const QIcon iconWindow = QIcon(":/Images/package_system.png");
    setWindowIcon(iconWindow);

    QRect screen  = QApplication::desktop()->screenGeometry();
    move(screen.center() - rect().center());
    setWindowModality(Qt::ApplicationModal);
    setWindowTitle(tr("Настройка параметров запроса 0x10"));
}
