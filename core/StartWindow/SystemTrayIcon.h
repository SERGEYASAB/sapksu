#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include "Common/PrefixHeader.h"

class SystemTrayIcon : public QObject
{
    Q_OBJECT

public:
    static SystemTrayIcon *instance();

    void showMessage(const QString &title, const QString &msg,
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int msecs = 10000);


    QSystemTrayIcon *trayIcon() const;

    QAction *IPCconAction() const;
    QAction *circutor1Action() const;
    QAction *circutor2Action() const;
    QAction *uz1Action() const;
    QAction *MKTNAction() const;


private:

    SystemTrayIcon(SystemTrayIcon const&) = delete;             // Copy construct
    SystemTrayIcon(SystemTrayIcon&&) = delete;                  // Move construct
    SystemTrayIcon& operator=(SystemTrayIcon const&) = delete;  // Copy assign
    SystemTrayIcon& operator=(SystemTrayIcon &&) = delete;      // Move assign

protected:
    explicit SystemTrayIcon(QObject *parent = nullptr);

    QMenu *m_trayIconMenu = nullptr;
    QSystemTrayIcon *m_trayIcon = nullptr;

    // Диагностические экшены для сап СУ
    QAction *m_IPCconAction = nullptr;
    QAction *m_circutor1Action = nullptr;
    QAction *m_circutor2Action = nullptr;
    QAction *m_uz1Action = nullptr;
    QAction *m_MKTNAction = nullptr;
};

#endif // SYSTEMTRAYICON_H
