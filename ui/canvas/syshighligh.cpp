// syshighligh.cpp

#include "syshighligh.h"
#include <QColor>
#include <QFont>

SysHighliter::SysHighliter(QTextDocument* parent) : QSyntaxHighlighter(parent) {
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214)); // #569CD6
    keywordFormat.setFontWeight(QFont::Bold);

    const QStringList keywordPatterns = {
        "\\bif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b", "\\breturn\\b",
        "\\bbreak\\b", "\\bcontinue\\b", "\\bswitch\\b", "\\bcase\\b", "\\bdefault\\b",
        "\\bnamespace\\b", "\\bclass\\b", "\\bstruct\\b", "\\bpublic\\b", "\\bprivate\\b", "\\bprotected\\b",
        "\\btry\\b", "\\bcatch\\b", "\\bthrow\\b", "\\bnew\\b", "\\bdelete\\b"
    };

    for (const QString &pattern : keywordPatterns) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        rules.append(rule);
    }

    QTextCharFormat typeFormat;
    typeFormat.setForeground(QColor(78, 201, 176));
    typeFormat.setFontWeight(QFont::Bold);
    const QStringList typePatterns = {"\\bint\\b", "\\bfloat\\b", "\\bdouble\\b", "\\bchar\\b", "\\bvoid\\b", "\\bbool\\b", "\\bQString\\b"};
    for (const QString &pattern : typePatterns) {
        HighlightRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = typeFormat;
        rules.append(rule);
    }

    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor(106, 153, 85));
    HighlightRule slc;
    slc.pattern = QRegularExpression("//.*");
    slc.format = singleLineCommentFormat;
    rules.append(slc);

    multiLineCommentFormat.setForeground(QColor(106, 153, 85));
    commentStart = QRegularExpression("/\\*");
    commentEnd = QRegularExpression("\\*/");

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(214, 157, 133));
    HighlightRule strRule;
    strRule.pattern = QRegularExpression(R"(("([^"\\]|\\.)*"))");
    strRule.format = stringFormat;
    rules.append(strRule);

    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168));
    HighlightRule numRule;
    numRule.pattern = QRegularExpression("\\b[0-9]+(\\.[0-9]+)?\\b");
    numRule.format = numberFormat;
    rules.append(numRule);
}

void SysHighliter::highlightBlock(const QString &text) {
    for (const HighlightRule &rule : qAsConst(rules)) {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStart);

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = commentEnd.match(text, startIndex);
        int endIndex = endMatch.hasMatch() ? endMatch.capturedStart() : -1;
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        if (endIndex == -1)
            break;
        startIndex = text.indexOf(commentStart, startIndex + commentLength);
    }
}

