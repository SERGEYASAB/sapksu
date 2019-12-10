#include "SystemTrayIcon.h"
#include "Authentication/SystemUser.h"
#include "DBConfig/DBConfig.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "IOServer/IOServer.h"

SystemTrayIcon *SystemTrayIcon::instance()
{
    static SystemTrayIcon singletone;
    return &singletone;
}

void SystemTrayIcon::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msecs)
{
    m_trayIcon->showMessage(title, msg, icon, msecs);
}

SystemTrayIcon::SystemTrayIcon(QObject *parent)
    : QObject(parent)
{

    m_trayIconMenu = new QMenu();

    m_IPCconAction = new QAction(QIcon(":/Images/bulbDigitalInputEnabled.png"), "Связь с ICP");
    m_circutor1Action = new QAction(QIcon(":/Images/bulbDigitalInputEnabled.png"), "Связь CIR1");
    m_circutor2Action = new QAction(QIcon(":/Images/bulbDigitalInputEnabled.png"), "Связь CIR2");
    m_uz1Action = new QAction(QIcon(":/Images/bulbDigitalInputEnabled.png"), "Связь UZ1");
    m_MKTNAction = new QAction(QIcon(":/Images/bulbDigitalInputEnabled.png"), "Связь с MKTN");

#ifdef SAP_SU
    m_trayIconMenu->addAction(m_IPCconAction);
    m_trayIconMenu->addAction(m_circutor1Action);
    m_trayIconMenu->addAction(m_circutor2Action);
    m_trayIconMenu->addAction(m_uz1Action);
    m_trayIconMenu->addAction(m_MKTNAction);
#endif

    m_trayIcon = new QSystemTrayIcon;

    m_trayIcon->setIcon(QIcon(":/Images/bulbDigitalInputEnabled.png"));
    m_trayIcon->setToolTip("Связь СУ: Норма");
    m_trayIcon->showMessage("Сервер ввода вывода", "Начат опрос оборудования САП СУ");
    m_trayIcon->setContextMenu(m_trayIconMenu);

    m_trayIcon->show();
}

QAction *SystemTrayIcon::MKTNAction() const
{
    return m_MKTNAction;
}

QAction *SystemTrayIcon::uz1Action() const
{
    return m_uz1Action;
}

QAction *SystemTrayIcon::circutor2Action() const
{
    return m_circutor2Action;
}

QAction *SystemTrayIcon::circutor1Action() const
{
    return m_circutor1Action;
}

QAction *SystemTrayIcon::IPCconAction() const
{
    return m_IPCconAction;
}

QSystemTrayIcon *SystemTrayIcon::trayIcon() const
{
    return m_trayIcon;
}
