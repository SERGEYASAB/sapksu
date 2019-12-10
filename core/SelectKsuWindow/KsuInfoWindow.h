#ifndef KSUINFOWINDOW_H
#define KSUINFOWINDOW_H

#include "Common/PrefixHeader.h"

class KsuInfoWindow : public QDialog
{
    Q_OBJECT

public:
    KsuInfoWindow(QWidget *parent = nullptr);

public slots:
    void save();

private slots:
    void readAgain();

private:
    QLineEdit *m_protokolNumberLineEdit = nullptr;
    QComboBox *m_typeSULineEdit = nullptr;
    QLineEdit *m_serialNumberSULineEdit = nullptr;
    QLineEdit *m_issueYearSULineEdit = nullptr;
    QLineEdit *m_invNumberSULineEdit = nullptr;
    QComboBox *m_typeKSULineEdit = nullptr;
    QLineEdit *m_serialNumberKSULineEdit = nullptr;
    QLineEdit *m_versionPOKSULineEdit = nullptr;
    QLineEdit *m_kindOfRepairsLineEdit = nullptr;
    QPushButton *m_okButton = nullptr;
    QPushButton *m_readAgain = nullptr;


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // KSUINFOWINDOW_H
