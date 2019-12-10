#ifndef UPDATEKSUWIZARD_H
#define UPDATEKSUWIZARD_H


#include "Common/PrefixHeader.h"

class ModbusKsuManualFullTestingWidget;
class Ksu735ManualFullTestingWidget;
class ClassReadBeforePage;
struct ModbusKsuManualTestingParametr;
struct Ksu735ManualFullTestingParametr;
class ReportConfigure;
class IVariable;
class ReportConfigure;

class UpdateKsuWizard : public QWizard
{
    Q_OBJECT

public:
    explicit UpdateKsuWizard(QObject *parent = nullptr);

private:
    QWizardPage *introPage();
    QWizardPage *finalPage();
    QWizardPage *updatePage();

    void fillValuesBefore(QList<IVariable *> params);
    void fillValuesAfter(QList<IVariable *> params);

    void init();
    void writeResult();

    ClassReadBeforePage *m_readBeforePage = nullptr;
    ClassReadBeforePage *m_readAfterPage = nullptr;
    QDateTime m_initTime;
    QStringList m_aliases;
    QStringList m_names;
    QStringList m_address;
    QList<QVariant> m_oldValues;
    QList<QVariant> m_newValues;
    QPushButton *m_cancelButton = nullptr;
    ReportConfigure *m_reportConfig = nullptr;
    QString m_versionBefore, m_versionAfter;

private slots:
    bool cancelButtonClicked();
    //void showReportWindow();
    void printReport();
    void printToPdf();

protected:
    void closeEvent(QCloseEvent *event);

    // QDialog interface
public slots:
    void reject();
};

#endif // UPDATEKSUWIZARD_H
