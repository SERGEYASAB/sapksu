#ifndef StartDiagnosticWindow_copy_H
#define StartDiagnosticWindow_copy_H

#include "Common/PrefixHeader.h"

class StartDiagnosticWindow_copy : public QObject
{
    Q_OBJECT

public:
    static StartDiagnosticWindow_copy *instance();
    void installStyleSheet(const QString &styleSheetPath);
private:
    StartDiagnosticWindow_copy(StartDiagnosticWindow_copy const&) = delete;             // Copy construct
    StartDiagnosticWindow_copy(StartDiagnosticWindow_copy &&) = delete;                  // Move construct
    StartDiagnosticWindow_copy& operator=(StartDiagnosticWindow_copy const&) = delete;  // Copy assign
    StartDiagnosticWindow_copy& operator=(StartDiagnosticWindow_copy &&) = delete;      // Move assign

protected:
    explicit StartDiagnosticWindow_copy(QObject *parent = nullptr);
    virtual ~StartDiagnosticWindow_copy() override;

public slots:/*
    void  startMainDialog();
    void startLoginDialog();*/
protected:
//    QTimer

    //void startDiagnosticWindowQml(); //(5 sec)
   // void mainDialog();// (есть кнопка, которая запускает LoginDialog


};

#endif // StartDiagnosticWindow_copy_H
