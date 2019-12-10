#ifndef REPORTCONFIGURE_H
#define REPORTCONFIGURE_H

#include "Common/PrefixHeader.h"

class QPrinter;

struct SessionRange
{
    QString name;
    QDateTime from, to;
    quint64 fromId, toId;
};

class ReportConfigure: public QMainWindow
{
    Q_OBJECT
public:
    explicit ReportConfigure(QWidget *parent = Q_NULLPTR);
    ~ReportConfigure();
    enum ReportType {
        TestHistoryShortReport = 0,
        TestHistoryReport = 1,
        LinkStabilityReport = 2,
        OperatorClickReport = 3,
        KsuParamChangeReport = 4,
        KsuParamChangeFaultReport = 5,
        KsuUpdateReport = 6
    };

    QDateTime dateTime = QDateTime::fromString("1.30.1", "M.d.s");

private:
    QTextEdit  *m_textEdit = nullptr;
    QToolBar *m_topMenuToolBar = nullptr;
    QToolBar *m_reportParamToolBar = nullptr;
    QAction *m_printAction = nullptr;
    QAction *m_printToPdfAction = nullptr;
    QAction *m_previewAction = nullptr;
    QTableWidget *m_reportTable = nullptr;
    QDockWidget *m_reportListDock = nullptr;
    QLabel *typeLabel = nullptr;
    QComboBox *m_reportTypeCombo = nullptr;
    QLabel *dateLabel = nullptr;
    //QDateEdit *m_datePicker = nullptr;
    QDateTimeEdit *m_fromDtPicker = nullptr;
    QDateTimeEdit *m_toDtPicker = nullptr;
    QPushButton *makeReportButton = nullptr;
    QList<SessionRange> m_selectedSessions;
    void drawPages(QPrinter *p);
    QHash<ReportType, QString> m_reportNames;
    QString m_pdfName;

    QToolBar *m_reportRemoveToolBar = nullptr;
    QRadioButton *m_removeSelectedRadio = nullptr;
    QRadioButton *m_removeByIntervalRadio = nullptr;
    QDateTimeEdit *m_removeFromDtPicker = nullptr;
    QDateTimeEdit *m_removeToDtPicker = nullptr;
    QPushButton *m_removeReportButton = nullptr;

public slots:
    // Краткий протокол тестирования СУ в автоматическом режиме
    void selectDataTestHistoryShortReport();

    // Полный протокол тестирования СУ в автоматическом режиме
    void selectDataTestHistoryReport();

    // Отчет по устойчивости связи
    void selectLinkStabilityReport();

    // Отчет по действиям оператора
    void selectOperatorClickReport();

    // Отчет по изменениям параметров КСУ
    void selectKsuParamChangeReport();

    // Отчет по отрицательным результатам изменения параметров КСУ
    void selectKsuParamChangeFaultReport();

    // Отчет по соответствию параметров после перепрограммирования
    void selectKsuUpdateReport();



    // Вставка текущего пользователя и даты формирования отчета
    void insertUserInfoFragment();

    // Вставка инфы по оборудованию
    void insertDeviceInfoFragment(const QString &ksuName, const QString &softwareVersion);

    // Вставка инфы по тесту
    void insertTestInfoFragment(const QString &user, const QString &kit, const QString &test);

    // Вставка поля для подписи
    void insertSign();


    void print();
    void printToPdf();
    void preview();

    //Для работы с невидимым окном
    void showReport(ReportType type, QDateTime &from, QDateTime &to);
    void previewReport(ReportType type, QDateTime &from, QDateTime &to);
    void printToPdfReport(ReportType type, QDateTime &from, QDateTime &to);

private slots:
    // Формирование списка отчетов слева
    void selectReports();

    // Выборка по выбранным отчетам
    bool selectReportsData();

    // Удаление отчетов
    void deleteReports();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // REPORTCONFIGURE_H

