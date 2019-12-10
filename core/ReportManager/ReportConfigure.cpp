#include "ReportConfigure.h"

#include <QPrintPreviewWidget>
#include <QPrintPreviewDialog>
#include <QPrintDialog>

#include "DBConfig/DBConfig.h"
#include "Authentication/SystemUser.h"
#include "Common/DateRangePicker.h"
#include "IOServer/IOServer.h"
#include "DatabaseDirectory/TestHistoryReport/TestHistoryReport.h"
#include "AppSettings/AppSettings.h"
#include "ReportManager/OperatorClickReport.h"
#include "ErrorClassifier/ErrorClassifier.h"

const QString breakPageString = QStringLiteral("-----------------------------------------------------------------------------------------");


ReportConfigure::ReportConfigure(QWidget *parent) : QMainWindow(parent)
{
    m_textEdit = new QTextEdit(this);
    m_textEdit->setReadOnly(true);

    setCentralWidget(m_textEdit);

    m_printAction = new QAction(QIcon(":/Images/print.png"), tr("&Печать"), this);
    m_printAction->setShortcuts(QKeySequence::Print);
    m_printAction->setStatusTip(tr("Распечатать отчет"));
    connect(m_printAction, &QAction::triggered, this, &ReportConfigure::print);

    m_printToPdfAction = new QAction(QIcon(":/Images/pdf.png"), tr("&Сохранить"), this);
    m_printToPdfAction->setShortcuts(QKeySequence::Print);
    m_printToPdfAction->setStatusTip(tr("Сохранить отчет"));
    connect(m_printToPdfAction, &QAction::triggered, this, &ReportConfigure::printToPdf);

    m_previewAction = new QAction(QIcon(":/Images/document-new.png"), tr("&Предпросмотр"), this);
    m_previewAction->setStatusTip(tr("Предварительный просмотр отчета"));
    connect(m_previewAction, &QAction::triggered, this, &ReportConfigure::preview);

    m_topMenuToolBar = new QToolBar(this);
    m_topMenuToolBar->setObjectName("reportPrintToolBar");
    m_topMenuToolBar->addAction(m_printAction);
    m_topMenuToolBar->addAction(m_printToPdfAction);
    m_topMenuToolBar->addAction(m_previewAction);

    dateLabel = new QLabel(tr("Период "), this);

    m_reportNames[TestHistoryShortReport] = tr("Краткий протокол автоматизированного тестирования СУ");
    m_reportNames[TestHistoryReport] = tr("Полный протокол автоматизированного тестирования СУ");
    m_reportNames[LinkStabilityReport] = tr("Отчеты по устойчивости связи");
    m_reportNames[OperatorClickReport] = tr("Отчет по действиям оператора");
    m_reportNames[KsuParamChangeReport] = tr("Отчет по изменению параметров КСУ");
    m_reportNames[KsuParamChangeFaultReport] = tr("Отчет по отрицательным изменениям параметров КСУ");
    m_reportNames[KsuUpdateReport] = tr("Отчет по соответствию параметров после перепрограммирования");

    typeLabel = new QLabel(tr("Тип отчета "), this);

    m_reportTypeCombo = new QComboBox(this);
    m_reportTypeCombo->setEditable(false);
    for (auto i : m_reportNames.values())
        m_reportTypeCombo->addItem(i);
    connect(m_reportTypeCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ReportConfigure::selectReports);

    m_fromDtPicker = new QDateTimeEdit(this);
    m_fromDtPicker->setCalendarPopup(true);
    m_fromDtPicker->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_fromDtPicker->setDate(QDate::currentDate());
    m_fromDtPicker->setTime(QTime(0, 0));
    connect(m_fromDtPicker, &QDateTimeEdit::dateTimeChanged, this, &ReportConfigure::selectReports);

    QLabel *dateDivideLabel = new QLabel(" - ", this);

    m_toDtPicker = new QDateTimeEdit(this);
    m_toDtPicker->setCalendarPopup(true);
    m_toDtPicker->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_toDtPicker->setDate(QDate::currentDate());
    m_toDtPicker->setTime(QTime(23, 59, 59, 999));
    connect(m_toDtPicker, &QDateTimeEdit::dateTimeChanged, this, &ReportConfigure::selectReports);

    makeReportButton = new QPushButton(tr("Сформировать"), this);
    connect(makeReportButton, &QPushButton::clicked, this, &ReportConfigure::selectReportsData);

    m_reportParamToolBar = new QToolBar(this);
    m_reportParamToolBar->setObjectName("reportParamToolBar");
    m_reportParamToolBar->addWidget(typeLabel);
    m_reportParamToolBar->addWidget(m_reportTypeCombo);
    m_reportParamToolBar->addSeparator();
    m_reportParamToolBar->addWidget(dateLabel);
    m_reportParamToolBar->addWidget(m_fromDtPicker);
    m_reportParamToolBar->addWidget(dateDivideLabel);
    m_reportParamToolBar->addWidget(m_toDtPicker);
    m_reportParamToolBar->addSeparator();
    m_reportParamToolBar->addWidget(makeReportButton);

    QLabel *reportRemoveLabel = new QLabel(tr("Удаление отчетов "));

    m_removeFromDtPicker = new QDateTimeEdit(this);
    m_removeFromDtPicker->setCalendarPopup(true);
    m_removeFromDtPicker->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_removeFromDtPicker->setDate(QDate::currentDate());
    m_removeFromDtPicker->setTime(QTime(0, 0));

    QLabel *removeDateDivideLabel = new QLabel(" - ", this);

    m_removeToDtPicker = new QDateTimeEdit(this);
    m_removeToDtPicker->setCalendarPopup(true);
    m_removeToDtPicker->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    m_removeToDtPicker->setDate(QDate::currentDate());
    m_removeToDtPicker->setTime(QTime(23, 59, 59, 999));

    m_removeSelectedRadio = new QRadioButton("Выбранные");
    connect(m_removeSelectedRadio, &QRadioButton::toggled, m_removeFromDtPicker, &QDateTimeEdit::setDisabled);
    connect(m_removeSelectedRadio, &QRadioButton::toggled, m_removeToDtPicker, &QDateTimeEdit::setDisabled);
    m_removeByIntervalRadio = new QRadioButton("За период");
    m_removeByIntervalRadio->setChecked(true);

    m_removeReportButton = new QPushButton("Удалить", this);
    connect(m_removeReportButton, &QPushButton::clicked, this, &ReportConfigure::deleteReports);

    m_reportRemoveToolBar = new QToolBar(this);
    m_reportRemoveToolBar->setObjectName("reportRemoveToolBar");
    m_reportRemoveToolBar->addWidget(reportRemoveLabel);
    m_reportRemoveToolBar->addSeparator();
    m_reportRemoveToolBar->addWidget(m_removeSelectedRadio);
    m_reportRemoveToolBar->addSeparator();
    m_reportRemoveToolBar->addWidget(m_removeByIntervalRadio);
    m_reportRemoveToolBar->addWidget(m_removeFromDtPicker);
    m_reportRemoveToolBar->addWidget(removeDateDivideLabel);
    m_reportRemoveToolBar->addWidget(m_removeToDtPicker);
    m_reportRemoveToolBar->addSeparator();
    m_reportRemoveToolBar->addWidget(m_removeReportButton);

    m_reportTable = new QTableWidget(this);
    m_reportTable->setColumnCount(3);
    m_reportTable->setHorizontalHeaderLabels(QStringList()
                                             <<tr("Название теста")
                                             <<tr("Дата начала")
                                             <<tr("Дата окончания"));
    m_reportTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_reportTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_reportTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_reportListDock = new QDockWidget("Список тестов за выбранный период", this);
    m_reportListDock->setWidget(m_reportTable);
    m_reportListDock->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    m_reportListDock->setObjectName("reportListDock");
    addDockWidget(Qt::LeftDockWidgetArea, m_reportListDock);
    addToolBar(Qt::TopToolBarArea, m_topMenuToolBar);
    addToolBar(Qt::TopToolBarArea, m_reportParamToolBar);
    addToolBar(Qt::TopToolBarArea, m_reportRemoveToolBar);

    selectReports();

    auto settings = AppSettings::settings();
    restoreGeometry(settings->value(QString(metaObject()->className()) + "/geometry", 100).toByteArray());
    restoreState(settings->value(QString(metaObject()->className()) + "/windowState", 100).toByteArray());
    for (int i = 0; i < m_reportTable->columnCount(); i++)
        m_reportTable->setColumnWidth(i, settings->value(QString(metaObject()->className()) + QString("/column%1Width").arg(i), 100).toInt());
}

ReportConfigure::~ReportConfigure()
{

}

void ReportConfigure::previewReport(ReportType type, QDateTime &from, QDateTime &to)
{
    m_reportTypeCombo->setCurrentIndex(type);
    m_fromDtPicker->setDateTime(from);
    m_toDtPicker->setDateTime(to);
    selectReports();
    //    if (m_reportTable->selectionMode() == QAbstractItemView::SingleSelection)
    //        m_reportTable->selectRow(m_reportTable->rowCount() - 1);
    //    else
    //        m_reportTable->selectAll();
    m_reportTable->selectRow(m_reportTable->rowCount() - 1);
    selectReportsData();
    preview();
}

void ReportConfigure::printToPdfReport(ReportType type, QDateTime &from, QDateTime &to)
{
    m_reportTypeCombo->setCurrentIndex(type);
    m_fromDtPicker->setDateTime(from);
    m_toDtPicker->setDateTime(to);
    selectReports();
    //    if (m_reportTable->selectionMode() == QAbstractItemView::SingleSelection)
    //        m_reportTable->selectRow(m_reportTable->rowCount() - 1);
    //    else
    //        m_reportTable->selectAll();
    m_reportTable->selectRow(m_reportTable->rowCount() - 1);
    if (!selectReportsData())
        return;
    printToPdf();
}

void ReportConfigure::selectDataTestHistoryReport()
{
    // Очищаем документ перед  формирование отчета
    m_textEdit->clear();
    m_pdfName.clear();
    if (m_reportTable->selectedItems().isEmpty())
        return;

    int currentRow = -1;
    bool goden = true;
    for (auto i : m_reportTable->selectedItems()) {
        if (currentRow == i->row())
            continue;
        currentRow = i->row();
        QString sqlString = QString("SELECT * FROM `TestHistoryReport` WHERE([id] >= '%1' AND [id] <= '%2')")
                            .arg(m_selectedSessions.at(i->row()).fromId)
                            .arg(m_selectedSessions.at(i->row()).toId);
        QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

        // Формируем шапку документа
        QTextCursor cursor = QTextCursor(m_textEdit->textCursor());

        QTextCharFormat textFormat;
        textFormat.setFontPointSize(12);
        textFormat.setFontWeight(QFont::Bold);

        cursor.setCharFormat(textFormat);
        cursor.insertText("Протокол тестирования");
        cursor.insertBlock();
        cursor.insertBlock();
        insertUserInfoFragment();
        cursor.insertBlock();
        cursor.insertBlock();
        textFormat.setFontPointSize(10);
        cursor.setCharFormat(textFormat);

        // Фомируем тело документа
        QStringList columnsName = {"datetime", "stepNumber", "stepName", "message", "aliasName", "aliasValue", "result"};
        QStringList columnsTableHeader = {"Дата/время", "Номер шага", "Название", "Сообщение", "Переменная", "Значение", "Результат"};
        int rows = 1;
        QHash<int, QStringList> tableData;
        tableData[0] = columnsTableHeader;

        cursor.beginEditBlock();
        while (query.next()) {

            if (query.value("function") == "init") {
                insertTestInfoFragment(query.value("user").toString(), query.value("kitName").toString(), query.value("testName").toString());
                insertDeviceInfoFragment(query.value("ksuName").toString(), query.value("softwareVersion").toString());
#ifdef SAP_KSU
                QString queryMessage = query.value("message").toString();

                int index6 = queryMessage.indexOf("Тип КСУ: ");
                int index7 = queryMessage.indexOf("Заводской номер КСУ: ");
                int index8 = queryMessage.indexOf("Версия ПО КСУ: ");
                int index9 = queryMessage.indexOf("Вид ремонта СУ: ");

                textFormat.setFontPointSize(10);
                textFormat.setFontWeight(QFont::Normal);
                textFormat.setFontItalic(false);
                cursor.setCharFormat(textFormat);

                cursor.insertBlock();
                cursor.insertText("   " + queryMessage.mid(index6, index7 - index6));
                cursor.insertBlock();
                cursor.insertText("   " + queryMessage.mid(index7, index8 - index7));
                cursor.insertBlock();
                cursor.insertText("   " + queryMessage.mid(index8, index9 - index8));
#endif

#ifdef SAP_SU
                textFormat.setFontPointSize(10);
                textFormat.setFontWeight(QFont::Bold);
                textFormat.setFontItalic(false);

                cursor.setCharFormat(textFormat);

                cursor.insertBlock();
                cursor.insertBlock();
                cursor.insertText("Станция управления:");
                cursor.insertBlock();
                cursor.insertBlock();

                int index1 = query.value("message").toString().indexOf("Номер протокола: ");
                int index2 = query.value("message").toString().indexOf("Тип СУ: ");
                int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
                int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
                int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
                int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
                int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
                int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
                int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

                QString queryMessage = query.value("message").toString();

                textFormat.setFontWeight(QFont::Normal);
                textFormat.setFontItalic(false);
                cursor.setCharFormat(textFormat);

                QString protNum = queryMessage.mid(index1, index2 - index1);
                QString date = query.value("datetime").toString();
                QString repairType = queryMessage.mid(index9);
                QString suType = queryMessage.mid(index2, index3 - index2);
                QString suSn = queryMessage.mid(index3, index4 - index3);
                m_pdfName = protNum.mid(protNum.lastIndexOf(":") + 1, protNum.length()) + "_" +
                            date.replace(":", "-") + "_" +
                            repairType.mid(repairType.lastIndexOf(":") + 1, repairType.length()) + "_" +
                            suType.mid(suType.lastIndexOf(":") + 1, suType.length()) + "_" +
                            suSn.mid(suSn.lastIndexOf(":") + 1, suSn.length());

                cursor.insertText("  " + protNum);
                cursor.insertBlock();
                cursor.insertText("  " + suType);
                cursor.insertBlock();
                cursor.insertText("  " + suSn);
                cursor.insertBlock();
                cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
                cursor.insertBlock();
                cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
                //cursor.insertBlock();
                //cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
                //cursor.insertBlock();
                //cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
                //cursor.insertBlock();
                //cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
                cursor.insertBlock();
                cursor.insertText("  " + repairType);
#endif

#ifdef MECHOS
                textFormat.setFontPointSize(10);
                textFormat.setFontWeight(QFont::Bold);
                textFormat.setFontItalic(false);

                cursor.setCharFormat(textFormat);

                cursor.insertBlock();
                cursor.insertBlock();
                cursor.insertText("Станция управления:");
                cursor.insertBlock();
                cursor.insertBlock();

                int index1 = query.value("message").toString().indexOf("Номер протокола: ");
                int index2 = query.value("message").toString().indexOf("Тип СУ: ");
                int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
                int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
                int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
                int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
                int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
                int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
                int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

                QString queryMessage = query.value("message").toString();

                textFormat.setFontWeight(QFont::Normal);
                textFormat.setFontItalic(false);
                cursor.setCharFormat(textFormat);

                QString protNum = queryMessage.mid(index1, index2 - index1);
                QString date = query.value("datetime").toString();
                QString repairType = queryMessage.mid(index9);
                QString suType = queryMessage.mid(index2, index3 - index2);
                QString suSn = queryMessage.mid(index3, index4 - index3);
                m_pdfName = protNum.mid(protNum.lastIndexOf(":") + 1, protNum.length()) + "_" +
                            date.replace(":", "-") + "_" +
                            repairType.mid(repairType.lastIndexOf(":") + 1, repairType.length()) + "_" +
                            suType.mid(suType.lastIndexOf(":") + 1, suType.length()) + "_" +
                            suSn.mid(suSn.lastIndexOf(":") + 1, suSn.length());

                cursor.insertText("  " + protNum);
                cursor.insertBlock();
                cursor.insertText("  " + suType);
                cursor.insertBlock();
                cursor.insertText("  " + suSn);
                cursor.insertBlock();
                cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
                cursor.insertBlock();
                cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
                //cursor.insertBlock();
                //cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
                //cursor.insertBlock();
                //cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
                //cursor.insertBlock();
                //cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
                cursor.insertBlock();
                cursor.insertText("  " + repairType);
#endif
                cursor.insertBlock();
                cursor.insertBlock();
                continue;
            }

            if (query.value("function") == "deinit") {
                QTextTableFormat format;
                format.setCellPadding(5);
                format.setCellSpacing(2);


                QTextTable *table = cursor.insertTable(rows, columnsName.count(), format);

                QString ksuValidity = QString("Годен");

                QTextBlockFormat centerAlignmentBlockFormat;
                centerAlignmentBlockFormat.setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

                QTextBlockFormat leftAlignmentBlockFormat;
                leftAlignmentBlockFormat.setAlignment(Qt::AlignLeft);

                QTextCharFormat normalCharFormat;
                normalCharFormat.setFontWeight(QFont::Normal);
                normalCharFormat.setBackground(Qt::white);

                QTextCharFormat boldCharFormat;
                boldCharFormat.setFontWeight(QFont::Bold);

                for(int row = 0; row < rows; row++)
                {
                    for(int column = 0; column < columnsName.count(); column++)
                    {
                        QTextTableCell cell = table->cellAt(row, column);

                        // Стилизация таблицы
                        if (row == 0) {

                            QTextCursor cellCursor = cell.firstCursorPosition();

                            cellCursor.setBlockFormat(centerAlignmentBlockFormat);
                            cellCursor.setCharFormat(boldCharFormat);

                            cellCursor.insertText(tableData.value(row).at(column));

                        } else if (column == 0 || column == 1) {

                            QTextCursor cellCursor = cell.firstCursorPosition();

                            cellCursor.setBlockFormat(centerAlignmentBlockFormat);
                            cellCursor.setCharFormat(normalCharFormat);

                            cellCursor.insertText(tableData.value(row).at(column));

                        } else if (column == 6) {

                            QTextCursor cellCursor = cell.firstCursorPosition();

                            QString result = tableData.value(row).at(column);

                            QTextCharFormat resultCharFormat;

                            if (result == "true" || result == "1") {

                                result = "Норма";
                                resultCharFormat.setFontWeight(QFont::Normal);
                                //resultCharFormat.setBackground(Qt::green);

                            } else if (result == "false" || result == "0"){

                                result = "Отказ";
                                resultCharFormat.setFontWeight(QFont::Normal);
                                //resultCharFormat.setBackground(Qt::red);
                                ksuValidity = "Не годен";
                                goden = false;
                            }

                            cellCursor.setBlockFormat(centerAlignmentBlockFormat);
                            cellCursor.setCharFormat(resultCharFormat);

                            cellCursor.insertText(result);

                        } else {

                            QTextCursor cellCursor = cell.firstCursorPosition();

                            cellCursor.setBlockFormat(leftAlignmentBlockFormat);
                            cellCursor.setCharFormat(normalCharFormat);

                            cellCursor.insertText(tableData.value(row).at(column));
                        }
                    }
                }

                cursor.endEditBlock();
                // Втавка заключения о годности СУ

                textFormat.setFontPointSize(12);
                textFormat.setFontWeight(QFont::Bold);
                textFormat.setFontItalic(false);

                cursor.movePosition(QTextCursor::MoveOperation::End);
                cursor.insertBlock();
                cursor.insertBlock();
                cursor.setCharFormat(textFormat);
                cursor.insertText(ksuValidity);
                cursor.insertBlock();
                cursor.insertBlock();
                insertSign();
                rows = 0;
                tableData.clear();
                continue;
            }

            QStringList data;

            for (auto column : columnsName) {
                data << query.value(column).toString();
            }

            tableData[rows] = data;

            rows++;
        }

        cursor.insertBlock();
        cursor.insertBlock();
        cursor.insertText(breakPageString);
        cursor.insertBlock();
        cursor.insertBlock();
//        if (currentRow == m_reportTable->selectedItems().last()->row()) {
//            textFormat.setFontPointSize(12);
//            textFormat.setFontWeight(QFont::Bold);
//            textFormat.setFontItalic(false);
//            cursor.setCharFormat(textFormat);
//            cursor.movePosition(QTextCursor::MoveOperation::End);
//            cursor.insertText(tr("Заключение: "));
//            cursor.insertBlock();
//            cursor.insertBlock();

//            if (goden) {
//                //textFormat.setBackground(Qt::green);
//                cursor.setCharFormat(textFormat);
//                cursor.insertText(tr("Годен"));
//            }
//            else {
//                //textFormat.setBackground(Qt::red);
//                cursor.setCharFormat(textFormat);
//                cursor.insertText(tr("Не годен"));
//            }


//            textFormat.setBackground(Qt::transparent);
//            cursor.setCharFormat(textFormat);
//            cursor.insertBlock();
//            cursor.insertBlock();
//            insertSign();
//        }
    }
}

void ReportConfigure::selectLinkStabilityReport()
{
    // Очищаем документ перед  формирование отчета
    m_textEdit->clear();

    int currentRow = -1;
    for (auto i : m_reportTable->selectedItems()) {
        if (currentRow == i->row())
            continue;
        currentRow = i->row();

        QString sqlString = QString("SELECT * FROM `LinkStabilityReport` WHERE [id] = %1")
                            .arg(m_selectedSessions.at(currentRow).fromId);
        QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());
        query.first();

        QTextCursor cursor = QTextCursor(m_textEdit->textCursor());

        QTextCharFormat textFormat;
        textFormat.setFontPointSize(12);
        textFormat.setFontWeight(QFont::Bold);

        cursor.setCharFormat(textFormat);
        cursor.insertText("Отчет по устойчивости связи");
        cursor.insertBlock();
        cursor.insertBlock();
        insertUserInfoFragment();
        cursor.insertBlock();
        cursor.insertBlock();
        textFormat.setFontPointSize(10);
        cursor.setCharFormat(textFormat);

        insertTestInfoFragment(query.value("user").toString(), "Ручное тестирование", "Тестирование устойчивости связи");
        insertDeviceInfoFragment(query.value("ksuName").toString(), query.value("softwareVersion").toString());

#ifdef SAP_KSU
        QString queryMessage = query.value("message").toString();

        int index6 = queryMessage.indexOf("Тип КСУ: ");
        int index7 = queryMessage.indexOf("Заводской номер КСУ: ");
        int index8 = queryMessage.indexOf("Версия ПО КСУ: ");
        int index9 = queryMessage.indexOf("Вид ремонта СУ: ");

        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Normal);
        textFormat.setFontItalic(false);
        cursor.setCharFormat(textFormat);

        cursor.insertBlock();
        cursor.insertText("   " + queryMessage.mid(index6, index7 - index6));
        cursor.insertBlock();
        cursor.insertText("   " + queryMessage.mid(index7, index8 - index7));
        cursor.insertBlock();
        cursor.insertText("   " + queryMessage.mid(index8, index9 - index8));
#endif

#ifdef SAP_SU
        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);
        cursor.setCharFormat(textFormat);
        cursor.insertBlock();
        cursor.insertBlock();
        cursor.insertText("Станция управления:");
        cursor.insertBlock();
        cursor.insertBlock();

        int index1 = query.value("message").toString().indexOf("Номер протокола: ");
        int index2 = query.value("message").toString().indexOf("Тип СУ: ");
        int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
        int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
        int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
        int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
        int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
        int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
        int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

        QString queryMessage = query.value("message").toString();

        textFormat.setFontWeight(QFont::Normal);
        textFormat.setFontItalic(false);
        cursor.setCharFormat(textFormat);

        QString protNum = queryMessage.mid(index1, index2 - index1);
        //        QString date = query.value("datetime").toString();
        QString repairType = queryMessage.mid(index9);
        QString suType = queryMessage.mid(index2, index3 - index2);
        QString suSn = queryMessage.mid(index3, index4 - index3);
        //        m_pdfName = protNum.mid(protNum.lastIndexOf(":") + 1, protNum.length()) + "_" +
        //                    date.replace(":", "-") + "_" +
        //                    repairType.mid(repairType.lastIndexOf(":") + 1, repairType.length()) + "_" +
        //                    suType.mid(suType.lastIndexOf(":") + 1, suType.length()) + "_" +
        //                    suSn.mid(suSn.lastIndexOf(":") + 1, suSn.length());

        cursor.insertText("  " + protNum);
        cursor.insertBlock();
        cursor.insertText("  " + suType);
        cursor.insertBlock();
        cursor.insertText("  " + suSn);
        cursor.insertBlock();
        cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
        cursor.insertBlock();
        cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
        //cursor.insertBlock();
        //cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
        //cursor.insertBlock();
        //cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
        //cursor.insertBlock();
        //cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
        cursor.insertBlock();
        cursor.insertText("  " + repairType);
#endif

#ifdef MECHOS
        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);
        cursor.setCharFormat(textFormat);
        cursor.insertBlock();
        cursor.insertBlock();
        cursor.insertText("Станция управления:");
        cursor.insertBlock();
        cursor.insertBlock();

        int index1 = query.value("message").toString().indexOf("Номер протокола: ");
        int index2 = query.value("message").toString().indexOf("Тип СУ: ");
        int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
        int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
        int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
        int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
        int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
        int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
        int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

        QString queryMessage = query.value("message").toString();

        textFormat.setFontWeight(QFont::Normal);
        textFormat.setFontItalic(false);
        cursor.setCharFormat(textFormat);

        QString protNum = queryMessage.mid(index1, index2 - index1);
        //        QString date = query.value("datetime").toString();
        QString repairType = queryMessage.mid(index9);
        QString suType = queryMessage.mid(index2, index3 - index2);
        QString suSn = queryMessage.mid(index3, index4 - index3);
        //        m_pdfName = protNum.mid(protNum.lastIndexOf(":") + 1, protNum.length()) + "_" +
        //                    date.replace(":", "-") + "_" +
        //                    repairType.mid(repairType.lastIndexOf(":") + 1, repairType.length()) + "_" +
        //                    suType.mid(suType.lastIndexOf(":") + 1, suType.length()) + "_" +
        //                    suSn.mid(suSn.lastIndexOf(":") + 1, suSn.length());

        cursor.insertText("  " + protNum);
        cursor.insertBlock();
        cursor.insertText("  " + suType);
        cursor.insertBlock();
        cursor.insertText("  " + suSn);
        cursor.insertBlock();
        cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
        cursor.insertBlock();
        cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
        //cursor.insertBlock();
        //cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
        //cursor.insertBlock();
        //cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
        //cursor.insertBlock();
        //cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
        cursor.insertBlock();
        cursor.insertText("  " + repairType);
#endif

        cursor.insertBlock();
        cursor.insertBlock();

        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);

        cursor.setCharFormat(textFormat);
        cursor.insertText("Дата начала тестирования: ");

        textFormat.setFontWeight(QFont::Normal);

        cursor.setCharFormat(textFormat);
        cursor.insertText(query.value("begindatetime").toString());
        cursor.insertBlock();
        cursor.insertBlock();

        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);

        cursor.setCharFormat(textFormat);
        cursor.insertText("Дата окончания тестирования: ");

        textFormat.setFontWeight(QFont::Normal);

        cursor.setCharFormat(textFormat);
        cursor.insertText(query.value("enddatetime").toString());
        cursor.insertBlock();
        cursor.insertBlock();

        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);

        cursor.setCharFormat(textFormat);
        cursor.insertText("Настройки порта: ");

        textFormat.setFontWeight(QFont::Normal);

        cursor.setCharFormat(textFormat);
        cursor.insertText(query.value("portsettings").toString());
        cursor.insertBlock();
        cursor.insertBlock();

        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);

        cursor.setCharFormat(textFormat);
        cursor.insertText(QString("Количество запросов: %1, Количество ответов: %2, Количество неответов: %3")
                          .arg(query.value("txcount").toString())
                          .arg(query.value("rxcount").toString())
                          .arg(query.value("errorcount").toString()));

        cursor.insertBlock();
        cursor.insertBlock();

        cursor.setCharFormat(textFormat);
        cursor.insertText(QString("Процент браковочного признака : %1, Допустимое значение: %2, Результат тестирования: %3")
                          .arg(query.value("quality").toString())
                          .arg(query.value("spquality").toString())
                          .arg(query.value("result").toString()));

        cursor.insertBlock();
        cursor.insertBlock();

        // Втавка заключения о годности СУ

        textFormat.setFontPointSize(12);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);

        cursor.movePosition(QTextCursor::MoveOperation::End);
        cursor.insertBlock();
        cursor.insertBlock();
        cursor.setCharFormat(textFormat);
        cursor.insertText(query.value("result").toString());

        cursor.movePosition(QTextCursor::MoveOperation::EndOfWord);
        textFormat.setFontPointSize(10);
        textFormat.setFontWeight(QFont::Bold);
        textFormat.setFontItalic(false);
        cursor.setCharFormat(textFormat);
        cursor.insertBlock();
        cursor.insertBlock();
        insertSign();

        if (currentRow != m_reportTable->selectedItems().last()->row()) {
            cursor.insertBlock();
            cursor.insertBlock();
            cursor.insertText(breakPageString);
            cursor.insertBlock();
            cursor.insertBlock();
        }
    }
}

void ReportConfigure::selectOperatorClickReport()
{
    m_textEdit->clear();

    QStringList columnsTableHeader = {"Дата\\время", "Пользователь", "Действие"};
    QStringList columnsName = {"datetime", "user", "message"};

    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());
    QTextCharFormat normalCharFormat;
    normalCharFormat.setFontWeight(QFont::Normal);

    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setCellSpacing(2);

    boldCharFormat.setFontPointSize(12);
    cursor.setCharFormat(boldCharFormat);
    cursor.insertText("Отчет по действиям оператора");
    cursor.insertBlock();
    cursor.insertBlock();
    insertUserInfoFragment();
    cursor.insertBlock();
    cursor.insertBlock();
    boldCharFormat.setFontPointSize(10);
    cursor.setCharFormat(boldCharFormat);

    QTextTable *table = cursor.insertTable(1, columnsTableHeader.count(), tableFormat);
    QTextTableCell cell;
    QTextCursor cellCursor;

    cursor.beginEditBlock();
    for (int i = 0; i < columnsTableHeader.count(); i++) {
        cell = table->cellAt(0, i);
        cellCursor = cell.firstCursorPosition();
        cellCursor.setCharFormat(boldCharFormat);
        cellCursor.insertText(columnsTableHeader.at(i));
    }

    QString sqlString = QString("SELECT * FROM `OperatorClickReport` WHERE([datetime] BETWEEN '%1' AND '%2')")
                        .arg(m_fromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                        .arg(m_toDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

    int currentRow = 1;
    cellCursor.setCharFormat(normalCharFormat);
    while (query.next()) {
        table->appendRows(1);
        for (int i = 0; i < columnsName.count(); i++) {
            cell = table->cellAt(currentRow, i);
            cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(query.value(columnsName.at(i)).toString());
        }
        ++currentRow;
    }

    cursor.insertBlock();
    cursor.insertBlock();
    insertSign();

    cursor.endEditBlock();
}

void ReportConfigure::selectKsuParamChangeReport()
{
    m_textEdit->clear();

    QStringList columnsTableHeader = {"Дата\\время", "Пользователь", "Тип КСУ", "Версия ПО КСУ", "Тип СУ", "Функция", "TX", "RX", "Название перем.", "Псевдоним", "Значение", "Результат"};
    QStringList columnsName = {"datetime", "user", "ksuName", "softwareVersion", "typeSU", "function", "tx", "rx", "name", "aliasName", "aliasValue", "result"};

    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());
    QTextCharFormat normalCharFormat;
    normalCharFormat.setFontWeight(QFont::Normal);

    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setCellSpacing(2);

    boldCharFormat.setFontPointSize(12);
    cursor.setCharFormat(boldCharFormat);
    cursor.insertText("Отчет по изменению параметров КСУ");
    cursor.insertBlock();
    cursor.insertBlock();
    insertUserInfoFragment();
    cursor.insertBlock();
    cursor.insertBlock();
    boldCharFormat.setFontPointSize(10);
    cursor.setCharFormat(boldCharFormat);

    QTextTable *table = cursor.insertTable(1, columnsTableHeader.count(), tableFormat);
    QTextTableCell cell;
    QTextCursor cellCursor;

    cursor.beginEditBlock();
    for (int i = 0; i < columnsTableHeader.count(); i++) {
        cell = table->cellAt(0, i);
        cellCursor = cell.firstCursorPosition();
        cellCursor.setCharFormat(boldCharFormat);
        cellCursor.insertText(columnsTableHeader.at(i));
    }

    QString sqlString = QString("SELECT `ParamsReport`.*, `ParametrAliases`.description AS `name` FROM `ParamsReport` "
                                "INNER JOIN `ParametrAliases` ON `ParametrAliases`.alias = `ParamsReport`.aliasName "
                                "WHERE([datetime] BETWEEN '%1' AND '%2')")
                        .arg(m_fromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                        .arg(m_toDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

    int currentRow = 1;
    cellCursor.setCharFormat(normalCharFormat);
    while (query.next()) {
        table->appendRows(1);
        for (int i = 0; i < columnsName.count(); i++) {
            cell = table->cellAt(currentRow, i);
            cellCursor = cell.firstCursorPosition();
            if (columnsName.at(i) == "function")
                cellCursor.insertText(query.value(columnsName.at(i)).toInt() == 1 ? tr("Чтение") : tr("Запись"));
            else if (columnsName.at(i) == "result") {
                if (query.value(columnsName.at(i)).toInt()) {
                    //normalCharFormat.setBackground(Qt::green);
                    cellCursor.setCharFormat(normalCharFormat);
                    cellCursor.insertText(tr("Успешно"));
                }
                else {
                    //normalCharFormat.setBackground(Qt::red);
                    cellCursor.setCharFormat(normalCharFormat);
                    cellCursor.insertText(tr("Ошибка"));
                }
                normalCharFormat.setBackground(Qt::transparent);
                cellCursor.setCharFormat(normalCharFormat);
            }
            else
                cellCursor.insertText(query.value(columnsName.at(i)).toString());
        }
        ++currentRow;
    }

    cursor.insertBlock();
    cursor.insertBlock();
    insertSign();
    cursor.endEditBlock();
}

void ReportConfigure::selectKsuParamChangeFaultReport()
{
    m_textEdit->clear();

    QStringList columnsTableHeader = {"Дата\\время", "Пользователь", "Тип КСУ", "Версия ПО КСУ", "Тип СУ", "Функция", "TX", "RX", "Название перем.", "Псевдоним", "Значение", "Результат"};
    QStringList columnsName = {"datetime", "user", "ksuName", "softwareVersion", "typeSU", "function", "tx", "rx", "name", "aliasName", "aliasValue", "result"};

    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());
    QTextCharFormat normalCharFormat;
    normalCharFormat.setFontWeight(QFont::Normal);

    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setCellSpacing(2);

    boldCharFormat.setFontPointSize(12);
    cursor.setCharFormat(boldCharFormat);
    cursor.insertText("Отчет по отрицательным результатам изменения параметров КСУ");
    cursor.insertBlock();
    cursor.insertBlock();
    insertUserInfoFragment();
    cursor.insertBlock();
    cursor.insertBlock();
    boldCharFormat.setFontPointSize(10);
    cursor.setCharFormat(boldCharFormat);

    QTextTable *table = cursor.insertTable(1, columnsTableHeader.count(), tableFormat);
    QTextTableCell cell;
    QTextCursor cellCursor;

    cursor.beginEditBlock();
    for (int i = 0; i < columnsTableHeader.count(); i++) {
        cell = table->cellAt(0, i);
        cellCursor = cell.firstCursorPosition();
        cellCursor.setCharFormat(boldCharFormat);
        cellCursor.insertText(columnsTableHeader.at(i));
    }

    QString sqlString = QString("SELECT `ParamsReport`.*, `ParametrAliases`.description AS `name` FROM `ParamsReport` "
                                "INNER JOIN `ParametrAliases` ON `ParametrAliases`.alias = `ParamsReport`.aliasName "
                                "WHERE([datetime] BETWEEN '%1' AND '%2') "
                                "AND [result] = 0")
                        .arg(m_fromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                        .arg(m_toDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

    int currentRow = 1;
    cellCursor.setCharFormat(normalCharFormat);
    while (query.next()) {
        table->appendRows(1);
        for (int i = 0; i < columnsName.count(); i++) {
            cell = table->cellAt(currentRow, i);
            cellCursor = cell.firstCursorPosition();
            if (columnsName.at(i) == "function")
                cellCursor.insertText(query.value(columnsName.at(i)).toInt() == 1 ? tr("Чтение") : tr("Запись"));
            else if (columnsName.at(i) == "result") {
                if (query.value(columnsName.at(i)).toInt()) {
                    //normalCharFormat.setBackground(Qt::green);
                    cellCursor.setCharFormat(normalCharFormat);
                    cellCursor.insertText(tr("Успешно"));
                }
                else {
                    //normalCharFormat.setBackground(Qt::red);
                    cellCursor.setCharFormat(normalCharFormat);
                    cellCursor.insertText(tr("Ошибка"));
                }
                normalCharFormat.setBackground(Qt::transparent);
                cellCursor.setCharFormat(normalCharFormat);
            }
            else
                cellCursor.insertText(query.value(columnsName.at(i)).toString());
        }
        ++currentRow;
    }

    cursor.insertBlock();
    cursor.insertBlock();
    insertSign();
    cursor.endEditBlock();
}

void ReportConfigure::selectKsuUpdateReport()
{
    m_textEdit->clear();

    if (m_reportTable->selectedItems().isEmpty())
        return;

    QStringList columnsTableHeader = {"Переменная", "Адрес", "Название", "Старое значение", "Новое значение", "Результат"};
    QStringList columnsName = {"aliasName", "address", "name", "aliasOldValue", "aliasNewValue", "result"};

    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());
    QTextCharFormat normalCharFormat;
    normalCharFormat.setFontWeight(QFont::Normal);

    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setCellSpacing(2);

    boldCharFormat.setFontPointSize(12);
    cursor.setCharFormat(boldCharFormat);
    cursor.insertText("Отчет о соответствии параметров КСУ после перепрограммирования");
    cursor.insertBlock();
    cursor.insertBlock();
    insertUserInfoFragment();
    cursor.insertBlock();
    cursor.insertBlock();
    boldCharFormat.setFontPointSize(10);
    boldCharFormat.setFontWeight(QFont::Bold);

    int infoPos = m_textEdit->textCursor().position();
    //insertDeviceInfoFragment(query.value("ksuName").toString(), query.value("softwareVersion").toString());
    //insertTestInfoFragment(query.value("user").toString(), query.value("kitName").toString(), query.value("testName").toString());

    boldCharFormat.setFontPointSize(10);
    boldCharFormat.setFontWeight(QFont::Bold);
    boldCharFormat.setFontItalic(false);

    cursor.setCharFormat(boldCharFormat);

    int currentReport = -1;
    bool godenTotal = true;
    //QString lastMessage = "";
    QTextTable *table = nullptr;
    QTextTableCell cell;
    QTextCursor cellCursor;
    QString verBefore, verAfter, user, ksuSV, ksuName, pedState1, pedState2;
    //bool newTable = false;
    //QString beginTime;
    cursor.beginEditBlock();
    for (auto i : m_reportTable->selectedItems()) {
        if (currentReport == i->row())
            continue;
        currentReport = i->row();

        int currentReportRow = 1;
        bool goden = true;

        QString sqlString = QString("SELECT * FROM `UpdateKsuReport` WHERE([id] >= '%1' AND [id] <= '%2')")
                            .arg(m_selectedSessions.at(i->row()).fromId)
                            .arg(m_selectedSessions.at(i->row()).toId);
        QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());
        while (query.next()) {
            if (query.value("aliasName") == "init") {
                //                if (lastMessage != query.value("message").toString()) {
                //                    if (currentReport != m_reportTable->selectedItems().first()->row()) {
                //                        cursor.insertBlock();
                //                        cursor.insertBlock();
                //                        cursor.insertText(breakPageString);
                //                        cursor.insertBlock();
                //                        cursor.insertBlock();
                //                    }

                pedState1 = query.value("aliasOldValue").toString();
                pedState2 = query.value("aliasNewValue").toString();

                table = cursor.insertTable(1, columnsTableHeader.count(), tableFormat);
                for (int i = 0; i < columnsTableHeader.count(); i++) {
                    cell = table->cellAt(0, i);
                    cellCursor = cell.firstCursorPosition();
                    cellCursor.setCharFormat(boldCharFormat);
                    cellCursor.insertText(columnsTableHeader.at(i));
                }
                verBefore = query.value("softwareVersion").toString();
                //                    cursor.insertText("Станция управления:");
                //                    cursor.insertBlock();
                //                    cursor.insertBlock();

                //                    int index1 = query.value("message").toString().indexOf("Номер протокола: ");
                //                    int index2 = query.value("message").toString().indexOf("Тип СУ: ");
                //                    int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
                //                    int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
                //                    int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
                //                    int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
                //                    int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
                //                    int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
                //                    int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

                //                    QString queryMessage = query.value("message").toString();

                //                    boldCharFormat.setFontWeight(QFont::Normal);
                //                    boldCharFormat.setFontItalic(false);
                //                    cursor.setCharFormat(boldCharFormat);

                //                    cursor.insertText("  " + queryMessage.mid(index1, index2 - index1));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index2, index3 - index2));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index3, index4 - index3));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
                //                    cursor.insertBlock();
                //                    cursor.insertText("  " + queryMessage.mid(index9));
                //                    cursor.insertBlock();
                //                    cursor.insertBlock();

                //                    boldCharFormat.setFontWeight(QFont::Bold);
                //                    lastMessage = query.value("message").toString();
                //                    beginTime = query.value(columnsName.at(0)).toString();
                //                    newTable = true;
                //                    currentReportRow = 1;
                //                }
            }
            else if (query.value("aliasName") == "deinit") {
                cursor.movePosition(QTextCursor::MoveOperation::End);
                boldCharFormat.setFontPointSize(12);
                cursor.setCharFormat(boldCharFormat);
                cursor.insertBlock();
                cursor.insertBlock();
                cursor.insertText(tr("Заключение: "));
                cursor.insertBlock();
                cursor.insertBlock();

                if (godenTotal) {
                    //boldCharFormat.setBackground(Qt::green);
                    cursor.setCharFormat(boldCharFormat);
                    cursor.insertText(tr("Годен"));
                }
                else {
                    //boldCharFormat.setBackground(Qt::red);
                    cursor.setCharFormat(boldCharFormat);
                    cursor.insertText(tr("Не годен"));
                }

                boldCharFormat.setBackground(Qt::transparent);
                cursor.setCharFormat(boldCharFormat);
                cursor.insertBlock();
                cursor.insertBlock();
                insertSign();
                verAfter = query.value("softwareVersion").toString();
            }
            else {
                user = query.value("user").toString();
                ksuSV = query.value("softwareVersion").toString();
                ksuName = query.value("ksuName").toString();

                table->appendRows(1);
                for (int i = 0; i < columnsName.count(); i++) {
                    cell = table->cellAt(currentReportRow, i);
                    cellCursor = cell.firstCursorPosition();
                    QString value = query.value(columnsName.at(i)).toString();

                    if (columnsName.at(i) == "result") {
                        if (value == "true" || value == "1") {
                            //normalCharFormat.setBackground(Qt::green);
                            cellCursor.setCharFormat(normalCharFormat);
                            cellCursor.insertText(tr("Годен"));
                        }
                        else if (value == "false" || value == "0"){
                            //normalCharFormat.setBackground(Qt::red);
                            cellCursor.setCharFormat(normalCharFormat);
                            cellCursor.insertText(tr("Не годен"));
                            godenTotal = goden = false;
                        }
                        normalCharFormat.setBackground(Qt::transparent);
                        cellCursor.setCharFormat(normalCharFormat);
                    }
                    //                    else if(columnsName.at(i) == "aliasOldValue" || columnsName.at(i) == "aliasNewValue") {
                    //                        value = QString::number(query.value(columnsName.at(i)).toFloat(), 'f', 3);
                    //                    }
                    else
                        cellCursor.insertText(value);
                }
                currentReportRow++;
            }
        }
    }

    cursor.setPosition(infoPos);

    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setFontPointSize(10);

    cursor.setCharFormat(format);
    cursor.insertText("Перепрограммирование выполнено пользователем: ");

    format.setFontWeight(QFont::Normal);
    format.setFontItalic(true);

    cursor.setCharFormat(format);
    cursor.insertText(user);
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Название КСУ: ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(ksuName);
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Версия ПО КСУ (шаблон классификатора): ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(ksuSV);
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Версия ПО КСУ до перепрограммирования: ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(verBefore);
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Версия ПО КСУ после перепрограммирования: ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(verAfter);
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Состояние работы ПЭД до перепрограммирования: ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(pedState1);
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Состояние работы ПЭД после перепрограммирования: ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(pedState2);
    cursor.insertBlock();
    cursor.insertBlock();

    cursor.endEditBlock();
}

void ReportConfigure::insertUserInfoFragment()
{
    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());

    QTextCharFormat format;
    format.setFontWeight(QFont::Bold);
    format.setFontPointSize(10);

    cursor.setCharFormat(format);
    cursor.insertText("Отчет выполнен пользователем: ");

    format.setFontWeight(QFont::Normal);
    format.setFontItalic(true);

    cursor.setCharFormat(format);
    cursor.insertText(SystemUser::instance()->user());
    cursor.insertBlock();

    format.setFontPointSize(10);
    format.setFontWeight(QFont::Bold);
    format.setFontItalic(false);

    cursor.setCharFormat(format);
    cursor.insertText("Дата формирования отчета: ");

    format.setFontWeight(QFont::Normal);

    cursor.setCharFormat(format);
    cursor.insertText(QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz"));
}

void ReportConfigure::insertDeviceInfoFragment(const QString &ksuName, const QString &softwareVersion)
{
    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(10);
    textFormat.setFontWeight(QFont::Bold);
    textFormat.setFontItalic(false);

    cursor.setCharFormat(textFormat);

    cursor.insertText("Оборудование:");
    cursor.insertBlock();
    cursor.insertBlock();

    textFormat.setFontWeight(QFont::Normal);
    cursor.setCharFormat(textFormat);

    cursor.insertText("   Название КСУ: ");

    cursor.insertText(ksuName);

    cursor.insertBlock();

    cursor.insertText("   Версия ПО КСУ (шаблон): ");

    cursor.insertText(softwareVersion);
}

void ReportConfigure::insertTestInfoFragment(const QString &user, const QString &kit, const QString &test)
{
    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());

    QTextCharFormat textFormat;
    textFormat.setFontPointSize(10);
    textFormat.setFontWeight(QFont::Bold);
    textFormat.setFontItalic(false);

    cursor.setCharFormat(textFormat);

    cursor.insertText("Тестирование:");
    cursor.insertBlock();
    cursor.insertBlock();

    textFormat.setFontWeight(QFont::Normal);
    cursor.setCharFormat(textFormat);

    cursor.insertText("   Пользователь: ");

    cursor.insertText(user);

    cursor.insertBlock();

    cursor.insertText("   Название набора: ");

    cursor.insertText(kit);

    cursor.insertBlock();

    cursor.insertText("   Название теста: ");

    cursor.insertText(test);

    cursor.insertBlock();
    cursor.insertBlock();
}

void ReportConfigure::insertSign()
{
    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());
    cursor.movePosition(QTextCursor::MoveOperation::End);
    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontPointSize(12);
    boldCharFormat.setFontWeight(QFont::Bold);
    cursor.setCharFormat(boldCharFormat);
    cursor.insertText(QStringLiteral("Подпись: ________________"));
}

void ReportConfigure::print()
{
    QPrintDialog dialog;
    dialog.setWindowTitle(tr("Печать отчета"));

    if (dialog.exec() == QDialog::Accepted)
        drawPages(dialog.printer());
    OperatorClickReport::instance()->write(QStringLiteral("Окно отчетов: нажата кнопка \"Печать\""));
}

void ReportConfigure::printToPdf()
{
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    QString name;
    switch (m_reportTypeCombo->currentIndex()) {
        case TestHistoryShortReport:
        case TestHistoryReport: {
            name = m_pdfName.replace(" ", "_");
            break;
        }

        default: {
            name = m_reportTypeCombo->currentText() + "_с_" +
                   m_fromDtPicker->dateTime().toString("yyyy-MM-dd_hh-mm-ss") + "_по_" +
                   m_toDtPicker->dateTime().toString("yyyy-MM-dd_hh-mm-ss");
            break;
        }
    }

    auto config = AppSettings::settings();
    config->beginGroup(metaObject()->className());
    QString lastFilePath = config->value("pdfDefaultPath", QDir::currentPath() + QDir::separator() + "Reports" + QDir::separator()).toString();
    config->endGroup();

    QString newFilePath = QFileDialog::getSaveFileName(this,
                                                       tr("Сохранить отчет..."),
                                                       lastFilePath + QDir::separator() + name,
                                                       "Файл конфигурации (*.pdf)");
    if (!newFilePath.isEmpty()) {
        config->beginGroup("ReportConfigure");
        config->setValue("pdfDefaultPath", newFilePath.left(newFilePath.lastIndexOf('/')));
        config->endGroup();
        printer.setOutputFileName(newFilePath);
        m_textEdit->print(&printer);
    }
    OperatorClickReport::instance()->write(QStringLiteral("Окно отчетов: нажата кнопка \"Вывести в PDF\""));
}

void ReportConfigure::preview()
{
    QPrintPreviewDialog dialog;
    dialog.setWindowTitle("Предварительный просмотр отчета");
    connect(&dialog, &QPrintPreviewDialog::paintRequested, this, &ReportConfigure::drawPages);
    dialog.exec();
    OperatorClickReport::instance()->write(QStringLiteral("Окно отчетов: нажата кнопка \"Предпросмотр\""));
}

void ReportConfigure::showReport(ReportConfigure::ReportType type, QDateTime &from, QDateTime &to)
{
    m_reportTypeCombo->setCurrentIndex(type);
    m_fromDtPicker->setDateTime(from);
    m_toDtPicker->setDateTime(to);
    selectReports();
    //    if (m_reportTable->selectionMode() == QAbstractItemView::SingleSelection)
    //        m_reportTable->selectRow(m_reportTable->rowCount() - 1);
    //    else
    //        m_reportTable->selectAll();
    m_reportTable->selectRow(m_reportTable->rowCount() - 1);
    selectReportsData();
    show();
}

void ReportConfigure::drawPages(QPrinter *p)
{
    m_textEdit->print(p);
}

void ReportConfigure::selectDataTestHistoryShortReport()
{
    m_textEdit->clear();
    m_pdfName.clear();
    if (m_reportTable->selectedItems().isEmpty())
        return;

    QStringList columnsTableHeader = {"Дата окончания", "Название набора", "Название теста", "Результат"};
    QStringList columnsName = {"datetime", "kitName", "testName", "result"};

    QTextCursor cursor = QTextCursor(m_textEdit->textCursor());
    QTextCharFormat normalCharFormat;
    normalCharFormat.setFontWeight(QFont::Normal);

    QTextCharFormat boldCharFormat;
    boldCharFormat.setFontWeight(QFont::Bold);

    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setCellSpacing(2);

    int currentReportRow = 1;
    int currentReport = -1;
    bool godenTotal = true;
    QString lastMessage = "";
    QTextTable *table = nullptr;
    QTextTableCell cell;
    QTextCursor cellCursor;
    bool newTable = false;
    QString beginTime;
    cursor.beginEditBlock();
    for (auto i : m_reportTable->selectedItems()) {
        if (currentReport == i->row())
            continue;
        currentReport = i->row();

        bool goden = true;

        QString sqlString = QString("SELECT * FROM `TestHistoryReport` WHERE([id] >= '%1' AND [id] <= '%2')")
                            .arg(m_selectedSessions.at(i->row()).fromId)
                            .arg(m_selectedSessions.at(i->row()).toId);
        QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());
        while (query.next()) {
            if (query.value("function") == "init") {
                if (lastMessage != query.value("message").toString()) {
                    if (currentReport != m_reportTable->selectedItems().first()->row()) {
                        cursor.movePosition(QTextCursor::MoveOperation::End);
                        cursor.insertBlock();
                        cursor.insertBlock();
                        boldCharFormat.setFontPointSize(12);
                        cursor.setCharFormat(boldCharFormat);
                        cursor.insertText(godenTotal ? "Годен" : "Не годен");
                        cursor.insertBlock();
                        cursor.insertBlock();
                        insertSign();

                        cursor.insertBlock();
                        cursor.insertBlock();
                        cursor.insertText(breakPageString);
                        cursor.insertBlock();
                        cursor.insertBlock();

                        godenTotal = true;
                    }
                    boldCharFormat.setFontPointSize(12);
                    cursor.setCharFormat(boldCharFormat);
                    cursor.insertText("Протокол тестирования");
                    cursor.insertBlock();
                    cursor.insertBlock();
                    insertUserInfoFragment();
                    cursor.insertBlock();
                    cursor.insertBlock();
                    boldCharFormat.setFontPointSize(10);
                    boldCharFormat.setFontWeight(QFont::Bold);

                    insertTestInfoFragment(query.value("user").toString(), query.value("kitName").toString(), query.value("testName").toString());
                    insertDeviceInfoFragment(query.value("ksuName").toString(), query.value("softwareVersion").toString());

#ifdef SAP_KSU
                QString queryMessage = query.value("message").toString();

                int index6 = queryMessage.indexOf("Тип КСУ: ");
                int index7 = queryMessage.indexOf("Заводской номер КСУ: ");
                int index8 = queryMessage.indexOf("Версия ПО КСУ: ");
                int index9 = queryMessage.indexOf("Вид ремонта СУ: ");

                normalCharFormat.setFontPointSize(10);
                normalCharFormat.setFontWeight(QFont::Normal);
                normalCharFormat.setFontItalic(false);
                cursor.setCharFormat(normalCharFormat);

                cursor.insertBlock();
                cursor.insertText("   " + queryMessage.mid(index6, index7 - index6));
                cursor.insertBlock();
                cursor.insertText("   " + queryMessage.mid(index7, index8 - index7));
                cursor.insertBlock();
                cursor.insertText("   " + queryMessage.mid(index8, index9 - index8));
#endif

#ifdef SAP_SU

                    int index1 = query.value("message").toString().indexOf("Номер протокола: ");
                    int index2 = query.value("message").toString().indexOf("Тип СУ: ");
                    int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
                    int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
                    int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
                    int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
                    int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
                    int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
                    int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

                    QString queryMessage = query.value("message").toString();

                    normalCharFormat.setFontPointSize(10);
                    normalCharFormat.setFontWeight(QFont::Normal);
                    normalCharFormat.setFontItalic(false);
                    cursor.setCharFormat(normalCharFormat);

                    cursor.insertBlock();
                    cursor.insertText("   " + queryMessage.mid(index6, index7 - index6));
                    cursor.insertBlock();
                    cursor.insertText("   " + queryMessage.mid(index7, index8 - index7));
                    cursor.insertBlock();
                    cursor.insertText("   " + queryMessage.mid(index8, index9 - index8));

                    boldCharFormat.setFontPointSize(10);
                    boldCharFormat.setFontWeight(QFont::Bold);
                    boldCharFormat.setFontItalic(false);

                    cursor.setCharFormat(boldCharFormat);

                    cursor.insertBlock();
                    cursor.insertBlock();
                    cursor.insertText("Станция управления:");
                    cursor.insertBlock();
                    cursor.insertBlock();

                    boldCharFormat.setFontWeight(QFont::Normal);
                    boldCharFormat.setFontItalic(false);
                    cursor.setCharFormat(boldCharFormat);

                    QString protNum = queryMessage.mid(index1, index2 - index1);
                    QString date = query.value("datetime").toString().replace(" ", "_");
                    QString repairType = queryMessage.mid(index9);
                    QString suType = queryMessage.mid(index2, index3 - index2);
                    QString suSn = queryMessage.mid(index3, index4 - index3);
                    m_pdfName = protNum.mid(protNum.lastIndexOf(":") + 1, protNum.length()) + "_" +
                                date.replace(":", "-") + "_" +
                                repairType.mid(repairType.lastIndexOf(":") + 1, repairType.length()) + "_" +
                                suType.mid(suType.lastIndexOf(":") + 1, suType.length()) + "_" +
                                suSn.mid(suSn.lastIndexOf(":") + 1, suSn.length());

                    cursor.insertText("  " + protNum);
                    cursor.insertBlock();
                    cursor.insertText("  " + suType);
                    cursor.insertBlock();
                    cursor.insertText("  " + suSn);
                    cursor.insertBlock();
                    cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
                    cursor.insertBlock();
                    cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
                    //cursor.insertBlock();
                    //cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
                    //cursor.insertBlock();
                    //cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
                    //cursor.insertBlock();
                    //cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
                    cursor.insertBlock();
                    cursor.insertText("  " + repairType);
#endif

#ifdef MECHOS
                    int index1 = query.value("message").toString().indexOf("Номер протокола: ");
                    int index2 = query.value("message").toString().indexOf("Тип СУ: ");
                    int index3 = query.value("message").toString().indexOf("Заводской номер СУ: ");
                    int index4 = query.value("message").toString().indexOf("Год выпуска СУ: ");
                    int index5 = query.value("message").toString().indexOf("Инвентарный номер СУ: ");
                    int index6 = query.value("message").toString().indexOf("Тип КСУ: ");
                    int index7 = query.value("message").toString().indexOf("Заводской номер КСУ: ");
                    int index8 = query.value("message").toString().indexOf("Версия ПО КСУ: ");
                    int index9 = query.value("message").toString().indexOf("Вид ремонта СУ: ");

                    QString queryMessage = query.value("message").toString();

                    normalCharFormat.setFontPointSize(10);
                    normalCharFormat.setFontWeight(QFont::Normal);
                    normalCharFormat.setFontItalic(false);
                    cursor.setCharFormat(normalCharFormat);

                    cursor.insertBlock();
                    cursor.insertText("   " + queryMessage.mid(index6, index7 - index6));
                    cursor.insertBlock();
                    cursor.insertText("   " + queryMessage.mid(index7, index8 - index7));
                    cursor.insertBlock();
                    cursor.insertText("   " + queryMessage.mid(index8, index9 - index8));

                    boldCharFormat.setFontPointSize(10);
                    boldCharFormat.setFontWeight(QFont::Bold);
                    boldCharFormat.setFontItalic(false);

                    cursor.setCharFormat(boldCharFormat);

                    cursor.insertBlock();
                    cursor.insertBlock();
                    cursor.insertText("Станция управления:");
                    cursor.insertBlock();
                    cursor.insertBlock();

                    boldCharFormat.setFontWeight(QFont::Normal);
                    boldCharFormat.setFontItalic(false);
                    cursor.setCharFormat(boldCharFormat);

                    QString protNum = queryMessage.mid(index1, index2 - index1);
                    QString date = query.value("datetime").toString().replace(" ", "_");
                    QString repairType = queryMessage.mid(index9);
                    QString suType = queryMessage.mid(index2, index3 - index2);
                    QString suSn = queryMessage.mid(index3, index4 - index3);
                    m_pdfName = protNum.mid(protNum.lastIndexOf(":") + 1, protNum.length()) + "_" +
                                date.replace(":", "-") + "_" +
                                repairType.mid(repairType.lastIndexOf(":") + 1, repairType.length()) + "_" +
                                suType.mid(suType.lastIndexOf(":") + 1, suType.length()) + "_" +
                                suSn.mid(suSn.lastIndexOf(":") + 1, suSn.length());

                    cursor.insertText("  " + protNum);
                    cursor.insertBlock();
                    cursor.insertText("  " + suType);
                    cursor.insertBlock();
                    cursor.insertText("  " + suSn);
                    cursor.insertBlock();
                    cursor.insertText("  " + queryMessage.mid(index4, index5 - index4));
                    cursor.insertBlock();
                    cursor.insertText("  " + queryMessage.mid(index5, index6 - index5));
                    //cursor.insertBlock();
                    //cursor.insertText("  " + queryMessage.mid(index6, index7 - index6));
                    //cursor.insertBlock();
                    //cursor.insertText("  " + queryMessage.mid(index7, index8 - index7));
                    //cursor.insertBlock();
                    //cursor.insertText("  " + queryMessage.mid(index8, index9 - index8));
                    cursor.insertBlock();
                    cursor.insertText("  " + repairType);
#endif

                    cursor.insertBlock();
                    cursor.insertBlock();
                    boldCharFormat.setFontWeight(QFont::Bold);
                    lastMessage = query.value("message").toString();
                    beginTime = query.value(columnsName.at(0)).toString();
                    newTable = true;
                    currentReportRow = 1;
                }
            }
            else if (query.value("function") == "deinit") {
                if (newTable) {
                    table = cursor.insertTable(1, columnsTableHeader.count(), tableFormat);
                    for (int i = 0; i < columnsTableHeader.count(); i++) {
                        cell = table->cellAt(0, i);
                        cellCursor = cell.firstCursorPosition();
                        cellCursor.setCharFormat(boldCharFormat);
                        cellCursor.insertText(columnsTableHeader.at(i));
                    }
                    newTable = false;
                }

                table->appendRows(1);
                //Дата начала
                //                cell = table->cellAt(currentReportRow, 0);
                //                cellCursor = cell.firstCursorPosition();

                //                cellCursor.insertText(beginTime);

                //Если дата начала снова появится, здесь тоже править надо
                cell = table->cellAt(currentReportRow, 0);
                cellCursor = cell.firstCursorPosition();
                cellCursor.insertText(query.value(columnsName.at(0)).toString());
                for (int i = 1; i < columnsName.count() - 1; i++) {
                    cell = table->cellAt(currentReportRow, i);
                    cellCursor = cell.firstCursorPosition();

                    cellCursor.insertText(query.value(columnsName.at(i)).toString());
                }
                cell = table->cellAt(currentReportRow, columnsName.count() - 1);
                cellCursor = cell.firstCursorPosition();
                if (goden) {
                    //normalCharFormat.setBackground(Qt::green);
                    cellCursor.setCharFormat(normalCharFormat);
                    cellCursor.insertText(tr("Годен"));
                }
                else {
                    //normalCharFormat.setBackground(Qt::red);
                    cellCursor.setCharFormat(normalCharFormat);
                    cellCursor.insertText(tr("Не годен"));
                }
                normalCharFormat.setBackground(Qt::transparent);

                cursor.movePosition(QTextCursor::MoveOperation::End);
                //                cursor.insertBlock();
                //                cursor.insertBlock();
                //cellCursor.setCharFormat(normalCharFormat);
            }
            else {
                QString result = query.value("result").toString();
                if (result == "false" || result == "0")
                    goden = false;
            }
        }

        if (!goden)
            godenTotal = false;
        currentReportRow++;
    }

//    cursor.movePosition(QTextCursor::MoveOperation::End);
//    boldCharFormat.setFontPointSize(12);
//    cursor.setCharFormat(boldCharFormat);
//    cursor.insertBlock();
//    cursor.insertBlock();
//    cursor.insertText(tr("Заключение: "));
//    cursor.insertBlock();
//    cursor.insertBlock();

//    if (godenTotal) {
//        //boldCharFormat.setBackground(Qt::green);
//        cursor.setCharFormat(boldCharFormat);
//        cursor.insertText(tr("Годен"));
//    }
//    else {
//        //boldCharFormat.setBackground(Qt::red);
//        cursor.setCharFormat(boldCharFormat);
//        cursor.insertText(tr("Не годен"));
//    }

//    boldCharFormat.setBackground(Qt::transparent);
//    cursor.setCharFormat(boldCharFormat);
//    cursor.insertBlock();
//    cursor.insertBlock();
//    insertSign();
    cursor.movePosition(QTextCursor::MoveOperation::End);
    cursor.insertBlock();
    cursor.insertBlock();
    boldCharFormat.setFontPointSize(12);
    cursor.setCharFormat(boldCharFormat);
    cursor.insertText(godenTotal ? "Годен" : "Не годен");
    cursor.insertBlock();
    cursor.insertBlock();
    insertSign();

    cursor.insertBlock();
    cursor.insertBlock();
    cursor.insertText(breakPageString);
    cursor.insertBlock();
    cursor.insertBlock();

    cursor.endEditBlock();
}

void ReportConfigure::selectReports()
{
    m_textEdit->clear();

    switch (m_reportTypeCombo->currentIndex()) {
        case TestHistoryShortReport:
        case TestHistoryReport: {
            m_toDtPicker->setDisabled(true);
            m_reportListDock->setDisabled(false);
            m_reportTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
            m_removeSelectedRadio->setEnabled(true);
            QDateTime from = m_fromDtPicker->dateTime();
            QDateTime to = from;
            to.setTime(QTime(23, 59, 59));

            QString sqlString = QString("SELECT * FROM `TestHistoryReport` WHERE([datetime] BETWEEN '%1' AND '%2')")
                                .arg(from.toString("yyyy-MM-dd hh:mm:ss"))
                                .arg(to.toString("yyyy-MM-dd hh:mm:ss"));
            QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

            quint64 id = 0;
            m_selectedSessions.clear();
            while (query.next()) {
                if (query.value("function") == "init") {
                    from = query.value("datetime").toDateTime();
                    id = query.value("id").toULongLong();
                }
                else if(query.value("function").toString() == "deinit")
                    m_selectedSessions.append({query.value("testName").toString(), from, query.value("datetime").toDateTime(), id, query.value("id").toULongLong()});
            }

            m_reportTable->clearContents();
            m_reportTable->setRowCount(m_selectedSessions.count());
            for (int i = 0; i < m_selectedSessions.count(); i++) {
                m_reportTable->setItem(i, 0, new QTableWidgetItem(m_selectedSessions.at(i).name));
                m_reportTable->setItem(i, 1, new QTableWidgetItem(m_selectedSessions.at(i).from.toString("yyyy-MM-dd hh:mm:ss")));
                m_reportTable->setItem(i, 2, new QTableWidgetItem(m_selectedSessions.at(i).to.toString("yyyy-MM-dd hh:mm:ss")));
            }
            break;
        }

        case LinkStabilityReport: {
            m_toDtPicker->setDisabled(true);
            m_reportListDock->setDisabled(false);
            m_reportTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
            m_removeSelectedRadio->setEnabled(true);
            QDateTime from = m_fromDtPicker->dateTime();
            QDateTime to = from;
            to.setTime(QTime(23, 59, 59));

            QString sqlString = QString("SELECT * FROM `LinkStabilityReport` WHERE([begindatetime] BETWEEN '%1' AND '%2')")
                                .arg(from.toString("yyyy-MM-dd hh:mm:ss"))
                                .arg(to.toString("yyyy-MM-dd hh:mm:ss"));
            QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

            quint64 id = 0;
            m_selectedSessions.clear();

            while (query.next()) {
                m_selectedSessions.append({"Тест стабильности связи",
                                           query.value("begindatetime").toDateTime(),
                                           query.value("enddatetime").toDateTime(),
                                           query.value("id").toULongLong(),
                                           0});
            }

            m_reportTable->clearContents();
            m_reportTable->setRowCount(m_selectedSessions.count());
            for (int i = 0; i < m_selectedSessions.count(); i++) {
                m_reportTable->setItem(i, 0, new QTableWidgetItem(m_selectedSessions.at(i).name));
                m_reportTable->setItem(i, 1, new QTableWidgetItem(m_selectedSessions.at(i).from.toString("yyyy-MM-dd hh:mm:ss")));
                m_reportTable->setItem(i, 2, new QTableWidgetItem(m_selectedSessions.at(i).to.toString("yyyy-MM-dd hh:mm:ss")));
            }
            break;
        }

        case OperatorClickReport:
        case KsuParamChangeReport:
        case KsuParamChangeFaultReport: {
            m_toDtPicker->setDisabled(false);
            m_reportListDock->setDisabled(true);
            m_reportTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
            m_reportTable->clearContents();
            m_removeSelectedRadio->setEnabled(false);
            m_removeByIntervalRadio->setChecked(true);
            break;
        }

        case KsuUpdateReport: {
            m_toDtPicker->setDisabled(true);
            m_reportListDock->setDisabled(false);
            m_reportTable->setSelectionMode(QAbstractItemView::SingleSelection);
            m_removeSelectedRadio->setEnabled(true);
            QDateTime from = m_fromDtPicker->dateTime();
            QDateTime to = from;
            to.setTime(QTime(23, 59, 59));

            QString sqlString = QString("SELECT * FROM `UpdateKsuReport` WHERE([datetime] BETWEEN '%1' AND '%2')")
                                .arg(from.toString("yyyy-MM-dd hh:mm:ss"))
                                .arg(to.toString("yyyy-MM-dd hh:mm:ss"));
            QSqlQuery query(sqlString, DBConfig::instance()->configDatabase());

            quint64 id = 0;
            m_selectedSessions.clear();

            while (query.next()) {
                if (query.value("aliasName") == "init") {
                    from = query.value("datetime").toDateTime();
                    id = query.value("id").toULongLong();
                }
                else if (query.value("aliasName") == "deinit") {
                    m_selectedSessions.append({"Тест на соответствие параметров после перепрограммирования",
                                               from,
                                               query.value("datetime").toDateTime(),
                                               id,
                                               query.value("id").toULongLong()});
                }
            }

            m_reportTable->clearContents();
            m_reportTable->setRowCount(m_selectedSessions.count());
            for (int i = 0; i < m_selectedSessions.count(); i++) {
                m_reportTable->setItem(i, 0, new QTableWidgetItem(m_selectedSessions.at(i).name));
                m_reportTable->setItem(i, 1, new QTableWidgetItem(m_selectedSessions.at(i).from.toString("yyyy-MM-dd hh:mm:ss")));
                m_reportTable->setItem(i, 2, new QTableWidgetItem(m_selectedSessions.at(i).to.toString("yyyy-MM-dd hh:mm:ss")));
            }
            break;
        }

        default: {
            return;
        }
    }
}

bool ReportConfigure::selectReportsData()
{
    switch (m_reportTypeCombo->currentIndex()) {
        case TestHistoryShortReport:
            selectDataTestHistoryShortReport();
            break;

        case TestHistoryReport: {
            selectDataTestHistoryReport();
            break;
        }

        case LinkStabilityReport: {
            selectLinkStabilityReport();
            break;
        }

        case OperatorClickReport:
            selectOperatorClickReport();
            return true;

        case KsuParamChangeReport:
            selectKsuParamChangeReport();
            return true;

        case KsuParamChangeFaultReport:
            selectKsuParamChangeFaultReport();
            return true;

        case KsuUpdateReport:
            selectKsuUpdateReport();
            break;

        default: {
            return false;
        }
    }
    if (!m_reportTable->selectedItems().count()) {
        ErrorClassifier::instance()->showError(33);
        return false;
    }
    return true;
}

void ReportConfigure::deleteReports()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Удаление отчетов"));
    msgBox.setText("Вы действительно хотите удалить " + (m_removeSelectedRadio->isChecked() ? "выбранные отчеты?" :
                                                                                              "отчеты за период с " + m_removeFromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss") + " по " + m_removeToDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss") + "?"));
    msgBox.setInformativeText(tr("Отчеты будут удалены без возможности восстановления."));
    msgBox.setIcon(QMessageBox::Warning);

    QPushButton *yesButton = msgBox.addButton(tr("&Да"), QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton(tr("&Отмена"), QMessageBox::RejectRole);

    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();

    if (msgBox.clickedButton() == cancelButton)
        return;

    QSqlDatabase db = DBConfig::instance()->configDatabase();
    QSqlQuery query(db);

    switch (m_reportTypeCombo->currentIndex()) {
        case TestHistoryShortReport:
        case TestHistoryReport: {
            int currentReport = -1;

            db.transaction();
            if (m_removeSelectedRadio->isChecked()) {
                for (auto i : m_reportTable->selectedItems()) {
                    if (currentReport == i->row())
                        continue;
                    currentReport = i->row();

                    query.exec(QString("DELETE FROM `TestHistoryReport` WHERE([id] >= '%1' AND [id] <= '%2')")
                               .arg(m_selectedSessions.at(currentReport).fromId)
                               .arg(m_selectedSessions.at(i->row()).toId));
                }
            }
            else {
                query.exec(QString("DELETE FROM `TestHistoryReport` WHERE([datetime] >= '%1' AND [datetime] <= '%2')")
                           .arg(m_removeFromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                           .arg(m_removeToDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            db.commit();
            selectReports();
            break;
        }

        case LinkStabilityReport: {
            int currentReport = -1;

            db.transaction();
            if (m_removeSelectedRadio->isChecked()) {
                for (auto i : m_reportTable->selectedItems()) {
                    if (currentReport == i->row())
                        continue;
                    currentReport = i->row();

                    query.exec(QString("DELETE FROM `LinkStabilityReport` WHERE([id] = '%1')")
                               .arg(m_selectedSessions.at(currentReport).fromId));
                }
            }
            else {
                query.exec(QString("DELETE FROM `LinkStabilityReport` WHERE([begindatetime] >= '%1' AND [begindatetime] <= '%2')")
                           .arg(m_removeFromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                           .arg(m_removeToDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            db.commit();
            selectReports();
            break;
        }

        case OperatorClickReport: {
            query.exec(QString("DELETE FROM `OperatorClickReport` WHERE([datetime] >= '%1' AND [datetime] <= '%2')")
                       .arg(m_removeFromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                       .arg(m_removeToDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
            selectReports();
            return;
        }

        case KsuParamChangeReport:
        case KsuParamChangeFaultReport: {
            query.exec(QString("DELETE FROM `ParamsReport` WHERE([datetime] >= '%1' AND [datetime] <= '%2')")
                       .arg(m_removeFromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                       .arg(m_removeToDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
            selectReports();
            return;
        }

        case KsuUpdateReport: {
            int currentReport = -1;

            db.transaction();
            if (m_removeSelectedRadio->isChecked()) {
                for (auto i : m_reportTable->selectedItems()) {
                    if (currentReport == i->row())
                        continue;
                    currentReport = i->row();

                    query.exec(QString("DELETE FROM `UpdateKsuReport` WHERE([id] >= '%1' AND [id] <= '%2')")
                               .arg(m_selectedSessions.at(currentReport).fromId)
                               .arg(m_selectedSessions.at(i->row()).toId));
                }
            }
            else {
                query.exec(QString("DELETE FROM `UpdateKsuReport` WHERE([datetime] >= '%1' AND [datetime] <= '%2')")
                           .arg(m_removeFromDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss"))
                           .arg(m_removeToDtPicker->dateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            db.commit();
            selectReports();
            break;
        }

        default: {
            return;
        }
    }
}

void ReportConfigure::closeEvent(QCloseEvent *event)
{
    auto settings = AppSettings::settings();
    settings->beginGroup(metaObject()->className());
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
    for (int i = 0; i < m_reportTable->columnCount(); i++)
        settings->setValue(QString("column%1Width").arg(i), m_reportTable->columnWidth(i));
    settings->endGroup();
    settings->sync();
    OperatorClickReport::instance()->write(QStringLiteral("Окно отчетов: закрытие окна"));
    QMainWindow::closeEvent(event);
}
