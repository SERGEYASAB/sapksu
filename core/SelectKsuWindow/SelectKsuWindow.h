#ifndef SELECTKSUWINDOW_H
#define SELECTKSUWINDOW_H

#include "Common/PrefixHeader.h"

class KSUPortConnectionWidget;
class KsuInfoWindow;

class SelectKsuWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SelectKsuWindow(QWidget *parent = nullptr);

public slots:
    void selectKsuTypes();
    void selectKsuSoftwareVersion(int idKsuType);
    void okButtonPressed();
    void cancelButtonPressed();
    void currenKsuTypeChanged(int index);
    void currentSoftwareVersionChanged(int index);
    bool checkUnfilledFields();
    void search();

signals:
    void ksuSelected();
    void ksuUnselected();

private:

    int m_currentKsuTypeId = -1;

    SapKsu::KSUProtocolType m_currentKsuProtocolType = SapKsu::Ksu735;

    QLabel *m_ksuAddressLabel = nullptr;
    QSpinBox *m_ksuAddressSpinBox = nullptr;

    QLabel *m_startSearchAddressLabel = nullptr;
    QSpinBox *m_startSearchAddressSpinBox = nullptr;

    QLabel *m_timeoutLabel = nullptr;
    QSpinBox *m_timeoutSpinbox = nullptr;

    QLabel *m_ksuTypeLabel = nullptr;
    QComboBox *m_ksuTypeComboBox = nullptr;

    QLabel *m_manufacturerLabel = nullptr;
    QLineEdit *m_manufacturerLineEdit = nullptr;

    QLabel *m_ksuSoftwareVersionLabel = nullptr;
    QComboBox *m_ksuSoftwareVersionComboBox = nullptr;

    QLabel *errorMessageLabel = nullptr;

    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    QPushButton *m_beginSearchButton;
    QPushButton *m_cancelSearchButton;

    KSUPortConnectionWidget *m_portSettings;

    QSqlQueryModel *m_ksuTypeModel = nullptr;
    QSqlQueryModel *m_ksuSoftwareVersionModel = nullptr;

    KsuInfoWindow *m_infoWindow = nullptr;


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
};

#endif // SELECTKSUWINDOW_H
