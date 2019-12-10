#ifndef STARTDIAGNOSTICWINDOW_H
#define STARTDIAGNOSTICWINDOW_H

#include "Common/PrefixHeader.h"

class StartDiagnosticWindow : public QWidget
{
    Q_OBJECT

public:
    static StartDiagnosticWindow *instance();

    void setupStyleSheet();

private:
    void installStyleSheet();
    void installStyleSheet(const QString &styleSheetPath);

    StartDiagnosticWindow(StartDiagnosticWindow const&) = delete;             // Copy construct
    StartDiagnosticWindow(StartDiagnosticWindow&&) = delete;                  // Move construct
    StartDiagnosticWindow& operator=(StartDiagnosticWindow const&) = delete;  // Copy assign
    StartDiagnosticWindow& operator=(StartDiagnosticWindow &&) = delete;      // Move assign

    qint8 directLogicLinkTest();
    qint8 afcLinkTest();

protected:
    explicit StartDiagnosticWindow(QWidget *parent = nullptr);
    virtual ~StartDiagnosticWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
};

#endif // STARTDIAGNOSTICWINDOW_H
