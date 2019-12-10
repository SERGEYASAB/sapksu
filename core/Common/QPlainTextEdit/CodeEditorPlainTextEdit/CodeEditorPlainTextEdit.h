#ifndef CODEEDITORPLAINTEXTEDIT_H
#define CODEEDITORPLAINTEXTEDIT_H

#include <QPlainTextEdit>
#include <QObject>

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;

class LineNumberArea;

class CodeEditorPlainTextEdit : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditorPlainTextEdit(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    bool isUndoAvailable();
    bool isRedoAvailable();

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    void setRedoAvailable(bool state);
    void setUndoAvailable(bool state);

    bool m_isUndoAvailable;
    bool m_isRedoAvailable;
    QWidget *m_lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditorPlainTextEdit *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    CodeEditorPlainTextEdit *m_codeEditor;
};

#endif // CODEEDITORPLAINTEXTEDIT_H
