#ifndef CHALLENGE_PANEL_H
#define CHALLENGE_PANEL_H

#include <QWidget>
#include <QString>
#include <QList>

class QListWidget;
class QTextEdit;
class QPushButton;
class QLabel;

class ChallengePanel : public QWidget {
    Q_OBJECT
public:
    explicit ChallengePanel(QWidget* parent = nullptr);

private slots:
    void onChallengeSelected();
    void onCheck();

private:
    struct Item { QString name; QString path; };

    void scanChallenges();
    QStringList candidateRoots() const;
    bool loadChallengeFolder(const QString& folder);
    static bool readUtf8File(const QString& path, QString& out);
    static QJsonValue canonicalize(const QJsonValue& v);

    QListWidget* list_{};
    QTextEdit* problemView_{};
    QTextEdit* testsView_{};
    QTextEdit* userSolution_{};
    QTextEdit* solutionView_{};
    QPushButton* checkBtn_{};
    QLabel* status_{};

    QList<Item> items_;
    QString currentSolutionPretty_;
};

#endif // CHALLENGE_PANEL_H

