#ifndef IOWIDGETSFACTORY_H
#define IOWIDGETSFACTORY_H

#include <QObject>

class IOWidgetsFactory : public QObject
{
    Q_OBJECT

public:
    static QWidget *modify(QObject *ptrNode);
};

#endif // IOWIDGETSFACTORY_H
