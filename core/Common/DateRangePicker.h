#ifndef DATERANGEPICKER_H
#define DATERANGEPICKER_H

#include "PrefixHeader.h"

class DateRangePicker : public QWidget
{
    Q_OBJECT
public:
    DateRangePicker(QWidget *parent = nullptr);

private:
    QLabel *beginTimeLabel;
    QLabel *endTimeLabel;
    QDateTimeEdit *beginTimePicker;
    QDateTimeEdit *endTimePicker;
    QPushButton *refreshButton;

signals:
    void rangeChanged(QDateTime begin, QDateTime end);

private slots:
    void rangeChangedNotify();
};

#endif // DATERANGEPICKER_H
