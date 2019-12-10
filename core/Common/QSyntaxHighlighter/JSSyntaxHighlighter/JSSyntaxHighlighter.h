#ifndef JSSYNTAXHIGHLIGHTER_H
#define JSSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextEdit>

class JSSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit JSSyntaxHighlighter(QTextDocument *parent = 0);
    void highlight(QStringList expressions, QTextCharFormat format);
    void clearHighlight();

protected:
    void highlightBlock(const QString &text) override;
private:

    struct BacklightRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };

    QVector<BacklightRule> backLightRules;
    QVector<BacklightRule> userRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // JSSYNTAXHIGHLIGHTER_H
