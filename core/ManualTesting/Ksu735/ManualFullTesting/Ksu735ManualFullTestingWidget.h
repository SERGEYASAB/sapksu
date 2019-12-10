#ifndef KSU735MANUALFULLTESTINGWIDGET_H
#define KSU735MANUALFULLTESTINGWIDGET_H

#include "Common/PrefixHeader.h"
#include "Ksu735ManualFullTestingParametr.h"

class Ksu735Node;
class Ksu735ManualFullTestingModel;

namespace Ui {
    class Ksu735ManualFullTestingWidget;
}

class ProgressBarMessageBox;

class Ksu735ManualFullTestingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit Ksu735ManualFullTestingWidget(Ksu735Node *node, QWidget *parent = 0);
    ~Ksu735ManualFullTestingWidget();

    bool getAutotestingmodeFlag() const;
    void setAutotestingmodeFlag(bool value);

    int getProgressBarProgress() const;
    void setProgressBarProgress(int value);

    ProgressBarMessageBox *getProgressBarMessageBox() const;
    Ksu735ManualFullTestingParametrsList enabledParametrs(bool writeOnly = false) const;

signals:
    void progressBarChanged(int value);

public slots:
    void stopTesting();
    bool autotest();
    void readAll();

private slots:
    void showAllCheckBoxChanged(int state);
    void enableAllCheckBoxChanged(int state);
    void enableDefault();

    void writeDefault();

    void clear();

    void writeAllMinimums();
    void writeAllMaximums();
    void writeAboveMaximum();
    void writeLessMinimum();

    void writeAll();
    void clearWriting();

    void okColorButton();
    void notSameColorButton();
    void errorColorButton();

private:
    void startStopTest();

    void readOneParametr(Ksu735ManualFullTestingParametr *parametr);
    bool writeOneParametr(Ksu735ManualFullTestingParametr *parametr, const QVariant &value);
    bool autotestParametr(Ksu735ManualFullTestingParametr *parametr);

    QVariant incrementValue(const QVariant &value, Ksu735Variable *variable, bool ignoreMaximum = false);

    void clearAll();
    void processEventsAndSleep(int sleepTime) const;

    bool compareReadedAndWritedValue(const QVariant &rval, const QVariant &lval, SapKsu::Ksu735ParametrFormat format);

private:
    Ui::Ksu735ManualFullTestingWidget *ui;

    Ksu735Node *m_node = nullptr;

    Ksu735ManualFullTestingModel *m_model = nullptr;
    Ksu735ManualFullTestingParametrsList m_parametrsList;

    bool m_showAll;
    volatile bool m_testing = false;
    bool autotestingmodeFlag = false;
    int progressBarProgress = 0;

    ProgressBarMessageBox *m_progressBarMessageBox = nullptr;
};

#endif // KSU735MANUALFULLTESTINGWIDGET_H
