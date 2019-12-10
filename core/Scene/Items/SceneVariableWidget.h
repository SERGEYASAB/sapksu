#ifndef SCENEVARIABLEWIDGET_H
#define SCENEVARIABLEWIDGET_H

#include "Common/PrefixHeader.h"
#include "ISceneItem.h"

class IVariable;

class SceneVariableWidget : public ISceneItem
{
    Q_OBJECT
public:
    SceneVariableWidget(const QString &alias, bool editable, bool resizeable = false, QWidget *parent = 0);

    QString getAlias() const;

    //То же, что и getPath \ setPath,
    //Только работает непосредственно через переменную
    IVariable *getVariable();
    void setVariable(IVariable *var);
    void poll();

    QFont getFont() override;
    void setFont(const QFont &font) override;
    QColor getFontColor() override;
    void setFontColor(const QColor &color) override;

public slots:
    void setAlias(const QString &alias);

signals:
    void aliasChanged(const QString &alias);

protected slots:
    //Просто перерисовывает значение
    void refresh();

private:
    IVariable *m_variable;
    QLabel *m_valueLabel;
    QString m_alias;
    QVBoxLayout *m_mainLayout;
    bool m_editable;
};

#endif // SCENEVARIABLEWIDGET_H
