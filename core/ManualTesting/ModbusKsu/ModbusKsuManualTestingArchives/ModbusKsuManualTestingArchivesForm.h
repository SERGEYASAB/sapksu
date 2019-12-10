#ifndef MODBUSKSUMANUALTESTINGARCHIVESFORM_H
#define MODBUSKSUMANUALTESTINGARCHIVESFORM_H

#include "Common/PrefixHeader.h"
#include "ModbusKsuManualTestingArchiveRecordsModel.h"

namespace Ui {
    class ModbusKsuManualTestingArchivesForm;
}

class ModbusKsuManualTestingArchivesForm : public QWidget
{
    Q_OBJECT

public:
    explicit ModbusKsuManualTestingArchivesForm(QWidget *parent = 0);
    ~ModbusKsuManualTestingArchivesForm();

public slots:
    void stopTesting();

protected:
    QTableView *tableView() const;
    QSpinBox *recordCountSpinBox() const;
    QSpinBox *offsetSpinBox() const;

    void startStopTest();
    inline bool testing() const { return m_testing; }

    inline ModbusKsuManualTestingArchiveRecordsModel &recordsModel() { return m_recordsModel; }

protected:
    virtual void request();
    virtual void readAll();
    virtual void clear() = 0;
    virtual void readArchives(int count = -1, int offset = 0) = 0;

private slots:
    void requestButtonClicked();
    void readAllButtonClicked();
    void clearButtonClicked();

private:
    Ui::ModbusKsuManualTestingArchivesForm *ui;

    ModbusKsuManualTestingArchiveRecordsModel m_recordsModel;

    volatile bool m_testing = false;
};

#endif // MODBUSKSUMANUALTESTINGARCHIVESFORM_H
