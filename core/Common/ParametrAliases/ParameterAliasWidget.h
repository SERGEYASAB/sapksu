#ifndef PARAMETERALIASWIDGET_H
#define PARAMETERALIASWIDGET_H

#include "Common/PrefixHeader.h"

class ParameterAliasWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ParameterAliasWidget(QWidget *parent = Q_NULLPTR);
    QString selectedAlias();

    // QObject interface

signals:
    void aliasSelected(QString alias);

public:
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    //Название неудачно. Вызывает окно со списком алиасов
    void selectAlias();
    void setupAlias(QString alias);
    void clearAlias();

private:
    QHBoxLayout *m_mainLayout;
    QLineEdit *m_aliasLineEdit;
    QToolButton *m_aliasSelectToolButton;


};

#endif // PARAMETERALIASWIDGET_H
