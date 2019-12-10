#ifndef TESTHISTORYREPORT_H
#define TESTHISTORYREPORT_H

#include "Common/PrefixHeader.h"

class TestHistoryReport : public QObject
{
    Q_OBJECT

public:
    explicit TestHistoryReport(QObject* parent = Q_NULLPTR);

    QString ksuType() const;
    void setKsuType(const QString &ksuType);

    QString ksuSoftwareVersion() const;
    void setKsuSoftwareVersion(const QString &ksuSoftwareVersion);

    QString testKitName() const;
    void setTestKitName(const QString &testKitName);

    QString testName() const;
    void setTestName(const QString &testName);

    QString stepName() const;
    void setStepName(const QString &stepName);

    QString stepNumber() const;
    void setStepNumber(const QString &stepNumber);

    QString protokolNumberLineEdit() const;
    void setProtokolNumberLineEdit(const QString &protokolNumberLineEdit);

    QString typeSULineEdit() const;
    void setTypeSULineEdit(const QString &typeSULineEdit);

    QString serialNumberSULineEdit() const;
    void setSerialNumberSULineEdit(const QString &serialNumberSULineEdit);

    QString issueYearSULineEdit() const;
    void setIssueYearSULineEdit(const QString &issueYearSULineEdit);

    QString invNumberSULineEdit() const;
    void setInvNumberSULineEdit(const QString &invNumberSULineEdit);

    QString typeKSULineEdit() const;
    void setTypeKSULineEdit(const QString &typeKSULineEdit);

    QString serialNumberKSULineEdit() const;
    void setSerialNumberKSULineEdit(const QString &serialNumberKSULineEdit);

    QString versionPOKSULineEdit() const;
    void setVersionPOKSULineEdit(const QString &versionPOKSULineEdit);

    QString kindOfRepairsLineEdit() const;
    void setKindOfRepairsLineEdit(const QString &kindOfRepairsLineEdit);

private:
    QString m_ksuType;
    QString m_ksuSoftwareVersion;
    QString m_testKitName;
    QString m_testName;
    QString m_stepNumber;
    QString m_stepName;

    // Заполнение этих полей проискходи в классе TestKitSlectWindow
    QString m_protokolNumberLineEdit;
    QString m_typeSULineEdit;
    QString m_serialNumberSULineEdit;
    QString m_issueYearSULineEdit;
    QString m_invNumberSULineEdit;
    QString m_typeKSULineEdit;
    QString m_serialNumberKSULineEdit;
    QString m_versionPOKSULineEdit;
    QString m_kindOfRepairsLineEdit;
};

#endif // TESTHISTORYREPORT_H
