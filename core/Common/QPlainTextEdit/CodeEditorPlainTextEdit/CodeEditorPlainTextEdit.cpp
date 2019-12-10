#include "CodeEditorPlainTextEdit.h"
#include <QtWidgets>

CodeEditorPlainTextEdit::CodeEditorPlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent),
      m_isUndoAvailable(false),
      m_isRedoAvailable(false)
{
    m_lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    connect(this, &CodeEditorPlainTextEdit::undoAvailable, this, &CodeEditorPlainTextEdit::setUndoAvailable);
    connect(this, &CodeEditorPlainTextEdit::redoAvailable, this, &CodeEditorPlainTextEdit::setRedoAvailable);

    QFont font = document()->defaultFont();
#if defined Q_OS_MACOS
    font.setFamily("Menlo");
    font.setPointSize(12);
#elif defined Q_OS_WIN
    font.setFamily("Consolas");
#endif
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    document()->setDefaultFont(font);

    const int tabStop = 4;

    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    setAcceptDrops(true);
}

int CodeEditorPlainTextEdit::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 13 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

bool CodeEditorPlainTextEdit::isRedoAvailable()
{
    return m_isRedoAvailable;
}

void CodeEditorPlainTextEdit::setRedoAvailable(bool state)
{
    m_isRedoAvailable = state;
}

bool CodeEditorPlainTextEdit::isUndoAvailable()
{
    return m_isUndoAvailable;
}

void CodeEditorPlainTextEdit::setUndoAvailable(bool state)
{
    m_isUndoAvailable = state;
}

void CodeEditorPlainTextEdit::updateLineNumberAreaWidth(int newBlockCount)
{
    Q_UNUSED(newBlockCount);
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditorPlainTextEdit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea->scroll(0, dy);
    else
        m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditorPlainTextEdit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditorPlainTextEdit::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditorPlainTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumberArea);
#ifdef Q_OS_MAC
    QFont font = painter.font();
    font.setFamily("Menlo");
    font.setPointSize(12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    painter.setFont(font);
#endif
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::darkCyan);
            painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

LineNumberArea::LineNumberArea(CodeEditorPlainTextEdit *editor) : QWidget(editor) {
    m_codeEditor = editor;
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    m_codeEditor->lineNumberAreaPaintEvent(event);
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
}
