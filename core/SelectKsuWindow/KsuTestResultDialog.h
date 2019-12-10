#ifndef KSUTESTRESULTDIALOG_H
#define KSUTESTRESULTDIALOG_H

#include "Common/PrefixHeader.h"

class KsuTestResultDialog : public QDialog
{
public:
    KsuTestResultDialog(QWidget *parent = nullptr);

    enum TestResult {
        NotOk = 0,
        Ok = 1
    };

protected slots:
    void save();

private:
    QVBoxLayout *m_mainLayout = nullptr;
    QFormLayout *m_dataLayout = nullptr;
    QHBoxLayout *m_buttonsLayout = nullptr;
    QLabel *m_resultLabel = nullptr;
    QComboBox *m_resultCombo = nullptr;
    QPushButton *m_saveButton = nullptr;
    QPushButton *m_dontSaveButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QLabel *m_infoLabel = nullptr;

    QHash<TestResult, QString> m_resultNames;
};

#endif // KSUTESTRESULTDIALOG_H
