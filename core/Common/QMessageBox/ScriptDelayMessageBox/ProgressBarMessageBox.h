#ifndef PROGRESSBARMESSAGEBOX_H
#define PROGRESSBARMESSAGEBOX_H

#include "Common/PrefixHeader.h"

class ModbusKsuManualFullTestingWidget;
class Ksu735ManualFullTestingWidget;

class ProgressBarMessageBox: public QWidget
{
    Q_OBJECT

public:
    explicit ProgressBarMessageBox(ModbusKsuManualFullTestingWidget *widget);
    explicit ProgressBarMessageBox(Ksu735ManualFullTestingWidget *widget);

    int value() const;
    void setValue(int value);

private slots:

private:
    QProgressBar *m_bar;

    void windowbehavior();
    void setupProgressBar();

    bool m_closeFlag = false;
    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event) override;
};

#endif // PROGRESSBARMESSAGEBOX_H
