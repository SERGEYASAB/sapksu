#include "DateRangePicker.h"

DateRangePicker::DateRangePicker(QWidget *parent) : QWidget(parent)
{   
    beginTimeLabel = new QLabel(tr("С"), this);
    endTimeLabel = new QLabel(tr("по"), this);
    beginTimePicker = new QDateTimeEdit(this);
    beginTimePicker->setCalendarPopup(true);
    beginTimePicker->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    QDateTime date = QDateTime::currentDateTime();
    date.setTime(QTime(0, 0, 0));
    beginTimePicker->setDateTime(date);
    //connect(beginTimePicker, &QDateTimeEdit::dateTimeChanged, this, DateRangePicker::rangeChangedNotify);
    endTimePicker = new QDateTimeEdit(this);
    endTimePicker->setCalendarPopup(true);
    endTimePicker->setDisplayFormat("yyyy-MM-dd hh:mm:ss");
    date.setTime(QTime(23, 59, 59));
    endTimePicker->setDateTime(date);
    refreshButton = new QPushButton(QIcon(":/Images/lupa.png"), "");
    //connect(endTimePicker, &QDateTimeEdit::dateTimeChanged, this, DateRangePicker::rangeChangedNotify);
    connect(refreshButton, &QPushButton::clicked, this, &DateRangePicker::rangeChangedNotify);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->addWidget(beginTimeLabel);
    lay->addWidget(beginTimePicker);
    lay->addWidget(endTimeLabel);
    lay->addWidget(endTimePicker);
    lay->addWidget(refreshButton);
}

void DateRangePicker::rangeChangedNotify()
{
    emit rangeChanged(beginTimePicker->dateTime(), endTimePicker->dateTime());
}
