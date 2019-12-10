#ifndef MODBUSKSUMANUALFULLTESTINGWIDGET_H
#define MODBUSKSUMANUALFULLTESTINGWIDGET_H

#include "Common/PrefixHeader.h"
#include "ManualTesting/ModbusKsu/ModbusKsuManualTestingParametr.h"

class ModbusKsuNode;
class ModbusKsuManualFullTestingModel;
struct ModbusKsuManualTestingParametr;

namespace Ui {
    class ModbusKsuManualFullTestingWidget;

}

class ProgressBarMessageBox;

class ModbusKsuManualFullTestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusKsuManualFullTestingWidget(ModbusKsuNode *node, QWidget *parent = 0);
    ~ModbusKsuManualFullTestingWidget();

    bool getAutotestingmodeFlag() const;
    void setAutotestingmodeFlag(bool value);

    int getProgressBarProgress() const;
    void setProgressBarProgress(int value);

    ProgressBarMessageBox *getProgressBarMessageBox() const;

    QList<ModbusKsuManualTestingParametr *> enabledParametrs(bool writeOnly = false);

signals:
    void progressBarChanged(int value);

public slots:
    void stopTesting();
    bool autotest();
    void readAll();
    void readSettings();
    void testLink();

private slots:
    void showAllCheckBoxChanged(int state);
    void enableAllCheckBoxChanged(int state);
    void enableDefault();

    void writeDefault();

    void clear();

    void writeAllMinimums();
    void writeAllMaximums();

    void writeAll();
    void clearWriting();

    void okColorButton();
    void notSameColorButton();
    void errorColorButton();

private:
    void startStopTest();

    bool readOneParametr(ModbusKsuManualTestingParametr *parametr);
    bool writeOneParametr(ModbusKsuManualTestingParametr *parametr, const QVariant &value);
    bool autotestParametr(ModbusKsuManualTestingParametr *parametr);

    QVariant incrementValue(const QVariant &value, ModbusVariable *variable);

    void clearAll();
    void processEventsAndSleep(int sleepTime) const;

    bool compareReadedAndWritedValue(const QVariant &rval, const QVariant &lval, ModbusVariable *variable);

private:
    Ui::ModbusKsuManualFullTestingWidget *ui;

    ModbusKsuNode *m_node = nullptr;

    ModbusKsuManualFullTestingModel *m_model = nullptr;
    QList<ModbusKsuManualTestingParametr *> m_parametrsList;

    bool m_showAll;
    volatile bool m_testing = false;
    bool autotestingmodeFlag = false;
    int progressBarProgress = 0;

    ProgressBarMessageBox *m_progressBarMessageBox = nullptr;

    QDateTime m_testBeginDt;
    int m_txCount, m_rxCount, m_errorCount;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event) override;
};

#endif // MODBUSKSUMANUALFULLTESTINGWIDGET_H
