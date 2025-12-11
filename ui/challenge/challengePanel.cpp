#include "challengePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <algorithm>

static QString prettyJson(const QByteArray& raw) {
    QJsonParseError err{}; QJsonDocument d = QJsonDocument::fromJson(raw, &err);
    if (err.error == QJsonParseError::NoError && !d.isNull()) return d.toJson(QJsonDocument::Indented);
    return QString::fromUtf8(raw);
}

ChallengePanel::ChallengePanel(QWidget* parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    auto *top = new QHBoxLayout();

    list_ = new QListWidget(this);
    list_->setMinimumWidth(160);
    list_->setSelectionMode(QAbstractItemView::SingleSelection);
    top->addWidget(list_, 0);

    auto *right = new QVBoxLayout();
    status_ = new QLabel("Select a challenge.", this);
    status_->setStyleSheet("color:#888");
    right->addWidget(status_);

    right->addWidget(new QLabel("Problem:", this));
    problemView_ = new QTextEdit(this);
    problemView_->setReadOnly(true);
    right->addWidget(problemView_, 1);

    right->addWidget(new QLabel("Tests (optional):", this));
    testsView_ = new QTextEdit(this);
    testsView_->setReadOnly(true);
    right->addWidget(testsView_, 1);

    right->addWidget(new QLabel("Your solution JSON:", this));
    userSolution_ = new QTextEdit(this);
    userSolution_->setPlaceholderText("Paste your JSON solution and click Check");
    right->addWidget(userSolution_, 1);

    right->addWidget(new QLabel("Canonical solution:", this));
    solutionView_ = new QTextEdit(this);
    solutionView_->setReadOnly(true);
    solutionView_->setVisible(false);
    right->addWidget(solutionView_, 1);

    auto *btns = new QHBoxLayout();
    btns->addStretch();
    checkBtn_ = new QPushButton("Check", this);
    checkBtn_->setEnabled(false);
    btns->addWidget(checkBtn_);
    right->addLayout(btns);

    top->addLayout(right, 1);
    root->addLayout(top, 1);

    connect(list_, &QListWidget::itemSelectionChanged, this, &ChallengePanel::onChallengeSelected);
    connect(checkBtn_, &QPushButton::clicked, this, &ChallengePanel::onCheck);

    scanChallenges();
}

QStringList ChallengePanel::candidateRoots() const {
    QStringList roots;
    const QString appDir = QCoreApplication::applicationDirPath();
    roots << QDir(appDir).filePath("challenge");
    roots << QDir(appDir + "/../Resources").filePath("challenge");
    roots << QDir(appDir + "/../../..").filePath("challenge");
    roots << QDir(appDir + "/../../../../").filePath("challenge");
    roots << QDir::current().filePath("challenge");
    return roots;
}

void ChallengePanel::scanChallenges() {
    items_.clear();
    list_->clear();

    QSet<QString> seen;
    for (const QString& rootPath : candidateRoots()) {
        QDir root(rootPath);
        if (!root.exists()) continue;
        QFileInfoList subs = root.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo& fi : subs) {
            const QString folder = fi.absoluteFilePath();
            const QString problemPath = QDir(folder).filePath("problem.txt");
            const QString solutionPath = QDir(folder).filePath("solution.json");
            if (QFileInfo::exists(problemPath) && QFileInfo::exists(solutionPath)) {
                if (seen.contains(folder)) continue;
                seen.insert(folder);
                items_.append({fi.fileName(), folder});
                auto *it = new QListWidgetItem(fi.fileName(), list_);
                it->setToolTip(folder);
            }
        }
    }
}

bool ChallengePanel::readUtf8File(const QString& path, QString& out) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return false;
    out = QString::fromUtf8(f.readAll());
    f.close();
    return true;
}

QJsonValue ChallengePanel::canonicalize(const QJsonValue& v) {
    if (v.isObject()) {
        QJsonObject obj = v.toObject();
        QStringList keys = obj.keys();
        std::sort(keys.begin(), keys.end(), [](const QString& a, const QString& b){ return a.localeAwareCompare(b) < 0; });
        QJsonObject out;
        for (const QString& k : keys) out.insert(k, canonicalize(obj.value(k)));
        return out;
    } else if (v.isArray()) {
        QJsonArray arr = v.toArray();
        QJsonArray out;
        for (const auto& e : arr) out.append(canonicalize(e));
        return out;
    }
    return v;
}

bool ChallengePanel::loadChallengeFolder(const QString& folder) {
    const QString problemPath = QDir(folder).filePath("problem.txt");
    const QString solutionPath = QDir(folder).filePath("solution.json");
    const QString testsPath = QDir(folder).filePath("tests.json");

    QString problem;
    if (!readUtf8File(problemPath, problem)) return false;
    problemView_->setPlainText(problem);

    QString solText;
    if (!readUtf8File(solutionPath, solText)) return false;
    currentSolutionPretty_ = prettyJson(solText.toUtf8());
    solutionView_->setPlainText(currentSolutionPretty_);
    solutionView_->setVisible(false);

    testsView_->clear();
    QString tests;
    if (QFileInfo::exists(testsPath) && readUtf8File(testsPath, tests)) {
        testsView_->setPlainText(prettyJson(tests.toUtf8()));
    }

    status_->setText("Challenge loaded. Paste your solution below.");
    checkBtn_->setEnabled(true);
    return true;
}

void ChallengePanel::onChallengeSelected() {
    int row = list_->currentRow();
    if (row < 0 || row >= items_.size()) {
        problemView_->clear(); testsView_->clear(); solutionView_->clear(); solutionView_->setVisible(false);
        currentSolutionPretty_.clear();
        checkBtn_->setEnabled(false);
        status_->setText("Select a challenge.");
        return;
    }
    loadChallengeFolder(items_.at(row).path);
}

void ChallengePanel::onCheck() {
    if (currentSolutionPretty_.isEmpty()) return;
    const QString userText = userSolution_->toPlainText().trimmed();
    if (userText.isEmpty()) { status_->setText("Please paste your JSON solution."); return; }

    QJsonParseError uerr{}; QJsonDocument udoc = QJsonDocument::fromJson(userText.toUtf8(), &uerr);
    if (uerr.error != QJsonParseError::NoError || udoc.isNull()) { status_->setText(QString("Invalid JSON: %1").arg(uerr.errorString())); return; }

    QJsonParseError serr{}; QJsonDocument sdoc = QJsonDocument::fromJson(currentSolutionPretty_.toUtf8(), &serr);
    if (serr.error != QJsonParseError::NoError || sdoc.isNull()) { status_->setText("Internal: bad stored solution."); return; }

    QJsonValue uval = canonicalize(udoc.isArray()? QJsonValue(udoc.array()) : QJsonValue(udoc.object()));
    QJsonValue sval = canonicalize(sdoc.isArray()? QJsonValue(sdoc.array()) : QJsonValue(sdoc.object()));

    if (uval == sval) {
        solutionView_->setVisible(true);
        status_->setText("Correct! Showing canonical solution.");
    } else {
        status_->setText("Not correct. Try again.");
    }
}

