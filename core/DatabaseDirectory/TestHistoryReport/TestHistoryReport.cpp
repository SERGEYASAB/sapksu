#include "TestHistoryReport.h"

TestHistoryReport::TestHistoryReport(QObject *parent)
{

}

QString TestHistoryReport::ksuType() const
{
    return m_ksuType;
}

void TestHistoryReport::setKsuType(const QString &ksuType)
{
    m_ksuType = ksuType;
}

QString TestHistoryReport::ksuSoftwareVersion() const
{
    return m_ksuSoftwareVersion;
}

void TestHistoryReport::setKsuSoftwareVersion(const QString &KsuSoftwareVersion)
{
    m_ksuSoftwareVersion = KsuSoftwareVersion;
}

QString TestHistoryReport::testKitName() const
{
    return m_testKitName;
}

void TestHistoryReport::setTestKitName(const QString &testKitName)
{
    m_testKitName = testKitName;
}

QString TestHistoryReport::testName() const
{
    return m_testName;
}

void TestHistoryReport::setTestName(const QString &testName)
{
    m_testName = testName;
}

QString TestHistoryReport::stepName() const
{
    return m_stepName;
}

void TestHistoryReport::setStepName(const QString &stepName)
{
    m_stepName = stepName;
}

QString TestHistoryReport::stepNumber() const
{
    return m_stepNumber;
}

void TestHistoryReport::setStepNumber(const QString &stepNumber)
{
    m_stepNumber = stepNumber;
}

QString TestHistoryReport::protokolNumberLineEdit() const
{
    return m_protokolNumberLineEdit;
}

void TestHistoryReport::setProtokolNumberLineEdit(const QString &protokolNumberLineEdit)
{
    m_protokolNumberLineEdit = protokolNumberLineEdit;
}

QString TestHistoryReport::typeSULineEdit() const
{
    return m_typeSULineEdit;
}

void TestHistoryReport::setTypeSULineEdit(const QString &typeSULineEdit)
{
    m_typeSULineEdit = typeSULineEdit;
}

QString TestHistoryReport::serialNumberSULineEdit() const
{
    return m_serialNumberSULineEdit;
}

void TestHistoryReport::setSerialNumberSULineEdit(const QString &serialNumberSULineEdit)
{
    m_serialNumberSULineEdit = serialNumberSULineEdit;
}

QString TestHistoryReport::issueYearSULineEdit() const
{
    return m_issueYearSULineEdit;
}

void TestHistoryReport::setIssueYearSULineEdit(const QString &issueYearSULineEdit)
{
    m_issueYearSULineEdit = issueYearSULineEdit;
}

QString TestHistoryReport::invNumberSULineEdit() const
{
    return m_invNumberSULineEdit;
}

void TestHistoryReport::setInvNumberSULineEdit(const QString &invNumberSULineEdit)
{
    m_invNumberSULineEdit = invNumberSULineEdit;
}

QString TestHistoryReport::typeKSULineEdit() const
{
    return m_typeKSULineEdit;
}

void TestHistoryReport::setTypeKSULineEdit(const QString &typeKSULineEdit)
{
    m_typeKSULineEdit = typeKSULineEdit;
}

QString TestHistoryReport::serialNumberKSULineEdit() const
{
    return m_serialNumberKSULineEdit;
}

void TestHistoryReport::setSerialNumberKSULineEdit(const QString &serialNumberKSULineEdit)
{
    m_serialNumberKSULineEdit = serialNumberKSULineEdit;
}

QString TestHistoryReport::versionPOKSULineEdit() const
{
    return m_versionPOKSULineEdit;
}

void TestHistoryReport::setVersionPOKSULineEdit(const QString &versionPOKSULineEdit)
{
    m_versionPOKSULineEdit = versionPOKSULineEdit;
}

QString TestHistoryReport::kindOfRepairsLineEdit() const
{
    return m_kindOfRepairsLineEdit;
}

void TestHistoryReport::setKindOfRepairsLineEdit(const QString &kindOfRepairsLineEdit)
{
    m_kindOfRepairsLineEdit = kindOfRepairsLineEdit;
}


