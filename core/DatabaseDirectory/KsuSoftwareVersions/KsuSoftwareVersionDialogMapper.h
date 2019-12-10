#ifndef KSUSOFTWAREVERSIONDIALOGMAPPER_H
#define KSUSOFTWAREVERSIONDIALOGMAPPER_H

#include "Common/PrefixHeader.h"

class KsuSoftwareVersionDialogMapper : public QWidget
{
    Q_OBJECT
public:
    explicit KsuSoftwareVersionDialogMapper(QWidget *parent = Q_NULLPTR);

    void setKSUSoftwareVersionId(int softwareVersionId);

signals:
    void dataChanged();

private slots:
    void save();

    void protocolTypeChanged(int protocolType);
    void classifierTemplateChanged(int index);

private:
    void selectCurrentSoftwareVersion();
    bool copyClassifierTemplate(const QString &sourcePath, const QString &destinationPath, bool replaceIfExist = false);

    void fillKSUTypes();
    void setCurrentKSUType(int ksuTypeId);

    void fillCommunicationProtocols();
    void setCurrentCommunicationProtocol(int protocolCode);

    void fillClassifierTemplates();
    void setCurrentClassifierTemplate(int templateId);

    bool showChangeClassifierTemplateWarning();
    bool checkUnfilledFields();

private:
    QSqlQueryModel *m_model = nullptr;

    QLabel *m_ksuTypeLabel = nullptr;
    QComboBox *m_ksuTypeComboBox = nullptr;

    QLabel *m_softwareVersionLabel = nullptr;
    QLineEdit *m_softwareVersionLineEdit = nullptr;

    QLabel *m_protocolTypeLabel = nullptr;
    QComboBox *m_protocolTypeComboBox = nullptr;

    QLabel *m_communicationProtocolLabel = nullptr;
    QComboBox *m_communicationProtocolComboBox = nullptr;

    QLabel *m_classifierTemplateLabel = nullptr;
    QComboBox *m_classifierTemplateComboBox = nullptr;

    QLabel *m_classifierNameLabel = nullptr;
    QLineEdit *m_classifierNameLineEdit = nullptr;

    QPushButton *m_saveButton = nullptr;
    QPushButton *m_cancelButton = nullptr;

    bool m_operationIsUpdate = false;
    bool m_changeClassifierWarningShown = false;

    int m_softwareVersionId = -1;
    SapKsu::KSUProtocolType m_protocolType = SapKsu::Ksu735;
    int m_currentClassifierTemplateId = -1;
    QString m_parametsConfigFileName;

    QSqlQueryModel *m_ksuTypesModel = nullptr;
    QSqlQueryModel *m_communicationProtocolsModel = nullptr;
    QSqlQueryModel *m_classifierTemplatesModel = nullptr;
};

#endif // KSUSOFTWAREVERSIONDIALOGMAPPER_H
