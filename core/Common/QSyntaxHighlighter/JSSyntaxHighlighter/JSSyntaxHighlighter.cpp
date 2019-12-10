#include "JSSyntaxHighlighter.h"

JSSyntaxHighlighter::JSSyntaxHighlighter(QTextDocument *parent)
    :QSyntaxHighlighter(parent)
{
    BacklightRule rule;

   // parent->setPageSize(QSizeF(100,100));
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns     << "\\babstract\\b" << "\\barguments\\b" << "\\bboolean\\b"
                        << "\\bbreak\\b" << "\\bbyte\\b" << "\\bcase\\b"
                        << "\\bcatch\\b" << "\\bchar\\b" << "\\bclass\\b"
                        << "\\bcontinue\\b" << "\\bdebugger\\b" << "\\bdefault\\b"
                        << "\\bdelete\\b" << "\\bdo\\b" << "\\bdouble\\b"
                        << "\\belse\\b" << "\\benum\\b" << "\\beval\\b"
                        << "\\bexport\\b" << "\\bextends\\b" << "\\bfalse\\b"
                        << "\\bfinal\\b" << "\\bfinally\\b" << "\\bfloat\\b"
                        << "\\bfor\\b" << "\\bfunction\\b" << "\\bgoto\\b"
                        << "\\bif\\b" << "\\byield\\b" << "\\bimplements\\b"
                        << "\\bimport\\b" << "\\bin\\b" << "\\binstanceof\\b"
                        << "\\blet\\b" << "\\blong\\b" << "\\bnative\\b"
                        << "\\bnew\\b" << "\\bnull\\b" << "\\bpackage\\b"
                        << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                        << "\\breturn\\b" << "\\bshort\\b" << "\\bstatic\\b"
                        << "\\bsuper\\b" << "\\bswitch\\b" << "\\bsynchronized\\b"
                        << "\\bthis\\b" << "\\bthrow\\b" << "\\bthrows\\b"
                        << "\\btransient\\b" << "\\btrue\\b" << "\\btry\\b"
                        << "\\btypeof\\b" << "\\bvar\\b" << "\\bvoid\\b"
                        << "\\bvolatile\\b" << "\\bwhile\\b" << "\\bwith\\b";

    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        backLightRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    backLightRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = singleLineCommentFormat;
    backLightRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.pattern.setMinimal(true);
    rule.format = quotationFormat;
    backLightRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    backLightRules.append(rule);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");
}

void JSSyntaxHighlighter::highlight(QStringList expressions, QTextCharFormat format)
{
    for (auto i : expressions)
        userRules.append({QRegExp(i), format});
    rehighlight();
}

void JSSyntaxHighlighter::clearHighlight()
{
    userRules.clear();
    rehighlight();
}

void JSSyntaxHighlighter::highlightBlock(const QString &text)
{
    QVector<BacklightRule> both = backLightRules + userRules;
    for (auto rule : both) {
        int index = rule.pattern.indexIn(text);
        while (index >= 0) {
            int length = rule.pattern.matchedLength();
            setFormat(index, length, rule.format);
            index = rule.pattern.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {

        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }

}
