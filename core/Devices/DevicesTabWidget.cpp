#include "DevicesTabWidget.h"
#include "DirectLogic/DirectLogicWidget.h"
#include "AFC/AFCWidget.h"
#include "Voltmeter/VoltmeterWidget.h"
#include "NL/NLWidget.h"
#include "MKTN/MKTNWidget.h"
#include "IcpCon/IcpConWidget.h"
#include "IcpCon/IcpConMechWidget.h"
#include "Circutor/CircutorWidget.h"
#include "IRZFC/IRZFCWidget.h"
#include "UPS/UPSWidget.h"
#include "ReportManager/OperatorClickReport.h"

DevicesTabWidget::DevicesTabWidget(QWidget *parent) : QTabWidget(parent)
{
#ifdef SAP_KSU
    DirectLogicWidget *directlogic = new DirectLogicWidget(this);
    AFCWidget *frecurencyConvertor = new AFCWidget(this);
    VoltmeterWidget *voltmeter = new VoltmeterWidget(this);
    NLWidget *nl16DO = new NLWidget(this);
    MKTNWidget *mktn = new MKTNWidget(this, "PV5");
    UPSWidget *ups = new UPSWidget(this);

    insertTab(Equipment::Controller, directlogic, QIcon(":/Images/gnome-network-properties.png"), "DIRECTLOGIC PLC");
    insertTab(Equipment::Voltmeters, voltmeter, QIcon(":/Images/voltmetr.png"), "Вольтметры");
    insertTab(Equipment::FC, frecurencyConvertor, QIcon(":/Images/frecurencyConvertor.png"), "AFC");
    insertTab(Equipment::NL16DO, nl16DO, QIcon(":/Images/nl16d0.png"), "NL-16DO");
    insertTab(Equipment::MKTN, mktn, QIcon(":/Images/mktn.png"), "МКТН");
    insertTab(Equipment::UPS, ups, QIcon(":/Images/powerSupply.png"), "ИБП");
#endif
#ifdef SAP_SU
    IcpConWidget *icpcon = new IcpConWidget(this);
    CircutorWidget *circutor = new CircutorWidget(this);
    MKTNWidget *mktn = new MKTNWidget(this, "MKTN");
    IRZFCWidget *irzfc = new IRZFCWidget(IRZFCWidget::IRZFCWidgetMode::Full, this);

    insertTab(Equipment::Controller, icpcon, QIcon(":/Images/gnome-network-properties.png"), "ICPCON PLC");
    insertTab(Equipment::Voltmeters, circutor, QIcon(":/Images/voltmetr.png"), "Circutor");
    insertTab(Equipment::FC, irzfc, QIcon(":/Images/frecurencyConvertor.png"), "ПЧ");
    insertTab(Equipment::MKTN, mktn, QIcon(":/Images/mktn.png"), "МКТН");
#endif
#ifdef MECHOS
    IcpConMechWidget *icpcon = new IcpConMechWidget(this);
    CircutorWidget *circutor = new CircutorWidget(false, this);
    MKTNWidget *mktn = new MKTNWidget(this, "MKTN");
    MKTNWidget *mktn2 = new MKTNWidget(this, "MKTN2");
    IRZFCWidget *irzfc = new IRZFCWidget(IRZFCWidget::IRZFCWidgetMode::Full, this);
    AFCWidget *frecurencyConvertor = new AFCWidget(this);

    insertTab(Equipment::Controller, icpcon, QIcon(":/Images/gnome-network-properties.png"), "ICPCON PLC");
    insertTab(Equipment::Voltmeters, circutor, QIcon(":/Images/voltmetr.png"), "Circutor");
    insertTab(Equipment::FC, irzfc, QIcon(":/Images/frecurencyConvertor.png"), "ПЧ");
    insertTab(Equipment::MKTN, mktn, QIcon(":/Images/mktn.png"), "МКТН");
    insertTab(Equipment::MKTN, mktn2, QIcon(":/Images/mktn.png"), "МКТН2");
    insertTab(Equipment::FC, frecurencyConvertor, QIcon(":/Images/frecurencyConvertor.png"), "AFC200");
#endif
}

void DevicesTabWidget::closeEvent(QCloseEvent *event)
{
    OperatorClickReport::instance()->write(QStringLiteral("Окно оборудования стенда: закрытие окна"));
    QTabWidget::closeEvent(event);
}
