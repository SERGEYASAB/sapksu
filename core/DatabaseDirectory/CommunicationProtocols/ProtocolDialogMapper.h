#ifndef PROTOCOLDIALOGMAPPER_H
#define PROTOCOLDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class ProtocolDialogMapper : public QWidget
{
    Q_OBJECT

public:
    explicit ProtocolDialogMapper(QAbstractItemModel *model, bool disableType = false, QWidget *parent = Q_NULLPTR);

public:
    void setCurrentModelIndex(const QModelIndex &index);

private slots:
    void save();

private:
    void fillDependentTables(SapKsu::KSUProtocolType protocolType, int protocolCode);

    void fillKsu735WorkModes(int protocolCode);
    void fillKsu735KindOfLaunch(int protocolCode);
    void fillKsu735StopReasonCodes(int protocolCode);
    void fillKsu735StatusWordInterpretationCodes(int protocolCode);
    void fillKsu735ErrorCodes(int protocolCode);
    void fillKsu735ReturnFlags10(int protocolCode);

    void fillModbusKsuKindOfLaunch(int protocolCode);
    void fillModbusKsuStopReasonCodes(int protocolCode);
    void fillModbusKsuArchiveEventClasses(int protocolCode);
    void fillModbusKsuArchiveEventCodes(int protocolCode);

private:
    QLabel *m_code;
    QLabel *m_name;
    QLabel *m_protocolType;

    QSpinBox *m_codeSpinBox;
    QLineEdit *m_nameLineEdit;
    QComboBox *m_protocolTypeComboBox;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;

    QDataWidgetMapper *m_mapper;

signals:
    void dataChanged();
};
#endif // PROTOCOLDIALOGMAPPER_H
