#ifndef ERRORCLASSIFIER_H
#define ERRORCLASSIFIER_H

#include "Common/PrefixHeader.h"

class ErrorClassifier : public QMessageBox
{
    Q_OBJECT

public:

    struct Error {
        enum ErrorType {
            NoError     = 0,
            Info        = 1,
            Question    = 2,
            Warning     = 3,
            Critical    = 4,
            Unknown     = 65535
        };

        ErrorType type;
        QString message;
        QString resolveMethod;
    };

    static ErrorClassifier *instance();
    int showError(quint16 code);
    QString errorName(quint16 code);
    QString errorDescription(quint16 code);
    QString errorFullText(quint16 code);

    //Для вставки пользовательского текста
    //Бывает нужно, когда идет ссылка на имя конкретного объекта
    //Вызывать до showError()
    void setErrorDescription(quint16 code, const QString &text);

private:

    QHash<quint16, Error> errors;

    explicit ErrorClassifier(QWidget *parent = 0);
    ~ErrorClassifier();

    Q_DISABLE_COPY(ErrorClassifier)
};
#endif // ERRORCLASSIFIER_H
