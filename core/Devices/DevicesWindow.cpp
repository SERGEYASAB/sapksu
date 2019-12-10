#include "DevicesWindow.h"
#include "StartWindow/ActionStyledFlowLayout.h"
#include "StartWindow/ActionStyledWidget.h"
#include "StartWindow/ActionStyledGroupMenuWidget.h"
#include "StartWindow/ActionControlWidgetsShow.h"
#include "StartWindow/StartWindow.h"
#include "Devices/DevicesTabWidget.h"
#include "Devices/EnergoFormDevice/EnergoFormRunningControl.h"
#include "IOServer/IOServer.h"
#include "ReportManager/OperatorClickReport.h"

DevicesWindow::DevicesWindow(QWidget *parent) : QWidget(parent)
{
    m_controllerAction = new QAction(this);
    m_voltmetrAction = new QAction(this);
    m_frecurencyConvertorAction = new QAction(this);
    m_mktnAction = new QAction(this);
    m_mktn2Action = new QAction(this);
    m_nlRizAction = new QAction(this);
    m_energoFormAction = new QAction(this);
    m_upsAction = new QAction(this);

#ifdef SAP_KSU
    connect(m_controllerAction, &QAction::triggered, [this]() { devicesTabWidget(0); });
    connect(m_frecurencyConvertorAction, &QAction::triggered, [this]() { devicesTabWidget(1); });
    connect(m_voltmetrAction, &QAction::triggered, [this]() { devicesTabWidget(2); });
    connect(m_mktnAction, &QAction::triggered, [this]() { devicesTabWidget(3); });
    connect(m_nlRizAction, &QAction::triggered, [this]() { devicesTabWidget(4); });
    connect(m_upsAction, &QAction::triggered, [this]() { devicesTabWidget(5); });
    connect(m_energoFormAction, &QAction::triggered, this, &DevicesWindow::energoForm);
#endif
#ifdef SAP_SU
    connect(m_controllerAction, &QAction::triggered, [this]() { devicesTabWidget(0); });
    connect(m_frecurencyConvertorAction, &QAction::triggered, [this]() { devicesTabWidget(1); });
    connect(m_voltmetrAction, &QAction::triggered, [this]() { devicesTabWidget(2); });
    connect(m_mktnAction, &QAction::triggered, [this]() { devicesTabWidget(3); });
#endif
#ifdef MECHOS
    connect(m_controllerAction, &QAction::triggered, [this]() { devicesTabWidget(0); });
    connect(m_frecurencyConvertorAction, &QAction::triggered, [this]() { devicesTabWidget(1); });
    connect(m_voltmetrAction, &QAction::triggered, [this]() { devicesTabWidget(2); });
    connect(m_mktnAction, &QAction::triggered, [this]() { devicesTabWidget(3); });
    connect(m_mktn2Action, &QAction::triggered, [this]() { devicesTabWidget(4); });
#endif

    m_controllerWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR, m_controllerAction,
                                                new QLabel("Контроллер \r\n Directlogic "),
                                                QIcon(":/Images/gnome-network-properties.png"));


    m_voltmetrWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                              m_voltmetrAction,
                                              new QLabel("Вольтметры"),
                                              QIcon(":/Images/voltmetr.png"));

    m_frecurencyConvertorWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                         m_frecurencyConvertorAction,
                                                         new QLabel("Частотные \r\n преобразователи"),
                                                         QIcon(":/Images/frecurencyConvertor.png"));

    m_mktnWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                          m_mktnAction,
                                          new QLabel("Плата \r\n МКТН"),
                                          QIcon(":/Images/mktn.png"));

    m_mktn2Widget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                          m_mktnAction,
                                          new QLabel("Плата \r\n МКТН2"),
                                          QIcon(":/Images/mktn.png"));

    m_energoFormWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                m_energoFormAction,
                                                new QLabel("Энергоформа "),
                                                QIcon(":/Images/energoForm.png"));

    m_upsWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                         m_upsAction,
                                         new QLabel("ИБП"),
                                         QIcon(":/Images/powerSupply.png"));

    m_nlRizActionWidget = new ActionStyledWidget(ACCESSLEVEL_OPERATOR,
                                                 m_nlRizAction,
                                                 new QLabel("NL-16DO \r\n R - изоляции"),
                                                 QIcon(":/Images/nl16d0.png"));

#ifdef SAP_KSU
    ActionStyledFlowLayout *devicesFlowLayout = new ActionStyledFlowLayout;
    devicesFlowLayout->addWidget(m_controllerWidget);
    devicesFlowLayout->addWidget(m_voltmetrWidget);
    devicesFlowLayout->addWidget(m_frecurencyConvertorWidget);
    devicesFlowLayout->addWidget(m_mktnWidget);
    devicesFlowLayout->addWidget(m_nlRizActionWidget);
    devicesFlowLayout->addWidget(m_energoFormWidget);
    devicesFlowLayout->addWidget(m_upsWidget);
#endif
#ifdef SAP_SU
    ActionStyledFlowLayout *devicesFlowLayout = new ActionStyledFlowLayout;
    devicesFlowLayout->addWidget(m_controllerWidget);
    devicesFlowLayout->addWidget(m_voltmetrWidget);
    devicesFlowLayout->addWidget(m_frecurencyConvertorWidget);
    devicesFlowLayout->addWidget(m_mktnWidget);
#endif
#ifdef MECHOS
    ActionStyledFlowLayout *devicesFlowLayout = new ActionStyledFlowLayout;
    devicesFlowLayout->addWidget(m_controllerWidget);
    devicesFlowLayout->addWidget(m_voltmetrWidget);
    devicesFlowLayout->addWidget(m_frecurencyConvertorWidget);
    devicesFlowLayout->addWidget(m_mktnWidget);
    devicesFlowLayout->addWidget(m_mktn2Widget);
#endif

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(new ActionStyledGroupMenuWidget(tr("Оборудование стенда")));
    mainLayout->addLayout(devicesFlowLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

DevicesWindow::~DevicesWindow()
{
    delete m_devicesTabWidget;
}

void DevicesWindow::devicesTabWidget(int index)
{
    if (m_devicesTabWidget == nullptr) {
        m_devicesTabWidget = new DevicesTabWidget();
        m_devicesTabWidget->setWindowModality(Qt::NonModal);
        m_devicesTabWidget->setWindowTitle("Оборудование стенда");
        m_devicesTabWidget->setWindowIcon(QIcon(":/Images/gnome-network-properties.png"));
        m_devicesTabWidget->resize(QSize(1024, 768));
        m_devicesTabWidget->move(QApplication::desktop()->screenGeometry().center() - m_devicesTabWidget->rect().center());
    }

    m_devicesTabWidget->setCurrentIndex(index);
    m_devicesTabWidget->show();
    m_devicesTabWidget->activateWindow();
    m_devicesTabWidget->showMaximized();
}

void DevicesWindow::energoForm()
{
    setWindowState(Qt::WindowMinimized);
    auto energoform = EnergoFormRunningControl::instance();
    energoform->runEnergoForm();
}

void DevicesWindow::closeEvent(QCloseEvent *event)
{
    IOServer::deleteIOServer();

    StartWindow *startWindow = new StartWindow;
    startWindow->setAttribute(Qt::WA_DeleteOnClose);
    startWindow->setWindowModality(Qt::NonModal);

    startWindow->setWindowTitle(titleString);

    startWindow->setWindowIcon(QIcon(":/Images/irz_logo.png"));
    startWindow->resize(QSize(1024, 768));
    startWindow->move(QApplication::desktop()->screenGeometry().center() - startWindow->rect().center());
    startWindow->show();
    startWindow->activateWindow();
    startWindow->showNormal();
    event->accept();
    OperatorClickReport::instance()->write(QStringLiteral("Окно оборудования стенда: закрытие окна"));
}
