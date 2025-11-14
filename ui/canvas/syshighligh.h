// syshighligh.h
// Simple syntax highlighter header (SysHighliter)

#ifndef SYSHIGHLIGH_H
#define SYSHIGHLIGH_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class SysHighliter : public QSyntaxHighlighter {
public:
    explicit SysHighliter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightRule { QRegularExpression pattern; QTextCharFormat format; };
    QVector<HighlightRule> rules;
    QRegularExpression commentStart;
    QRegularExpression commentEnd;
    QTextCharFormat multiLineCommentFormat;
};

#endif // SYSHIGHLIGH_H

