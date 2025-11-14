//
// Created by Ruhaan on 12/08/25.
//


#include "mainWindow.h"

#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QSplitter>
#include <QStyle>
#include <QDebug>
#include <QHBoxLayout>
#include <QDir>
#include <QShortcut>

#include "../canvas/canvasview.h"
#include "../canvas/canvasScene.h"
#include "../canvas/editorWindow.h"
#include "../canvas/open_load.h"
#include "../../core/scene/nodescene.h"
#include "../../core/noderegistry/noderegister.h"
#include "../../core/codegeneration/codeTemplateManager.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    initUI();
}

void MainWindow::initUI() {
    resize(config.windowWidth, config.windowHeight);
    centerOnScreen();

    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // central layout: left icon bar + main splitter
    centralLayout = new QHBoxLayout();
    centralLayout->setContentsMargins(0,0,0,0);
    centralLayout->setSpacing(0);

    scene = new Scene();
    grScene = scene->grScene;
    view = new CanvasView(grScene);
    view->setScene(grScene);

    editWin = new EditorWindow(this);
    scene->setEditorWindow(editWin);
    editWin->setScene(scene);

    nodeList = new NodeRegistery(scene);
    // remove default frame to avoid bright border line
    nodeList->setFrameShape(QFrame::NoFrame);
    nodeList->setStyleSheet("QTreeWidget{border:0px;}");

    splitter = new QSplitter(config.splitterOrientation, this);
    splitter->addWidget(nodeList);
    splitter->addWidget(view);
    splitter->addWidget(editWin);
    splitter->setHandleWidth(1);
    splitter->setStyleSheet("QSplitter::handle{background:#222;}");

    setupLeftIconPanel();

    centralLayout->addWidget(leftPanel);
    centralLayout->addWidget(splitter);
    layout->addLayout(centralLayout);

    // keyboard shortcuts for file actions
    new QShortcut(QKeySequence::Open, this, SLOT(onFileOpen()));
    new QShortcut(QKeySequence::Save, this, SLOT(onFileSave()));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    new QShortcut(QKeySequence::SaveAs, this, SLOT(onFileSaveAs()));
#endif
    // Refresh: Cmd (Meta) + R
    new QShortcut(QKeySequence("Meta+R"), this, SLOT(onRefresh()));

    setWindowTitle(config.windowTitle);

    bool loaded = CodeTemplateManager::getInstance().loadTemplatesFromFile(":/codebase/python.json");
    if (!loaded) {
        qWarning() << "Failed to load Python templates from any location";
    } else {
        qDebug() << "Successfully loaded Python templates";
    }
}

void MainWindow::centerOnScreen() {
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }
}

void MainWindow::setWindowDimensions(int width, int height) {
    config.windowWidth = width;
    config.windowHeight = height;
    resize(width, height);
}

void MainWindow::setWindowTitle(const QString& title) {
    config.windowTitle = title;
    QWidget::setWindowTitle(title);
}

void MainWindow::setWindowTitleWithFilename(const QString& filename) {
    QString newTitle;
    if (filename.isEmpty()) {
        newTitle = "DiverseXEngine";
    } else {
        newTitle = QString("DiverseXEngine - %1").arg(filename);
    }
    setWindowTitle(newTitle);
}

void MainWindow::setupLeftIconPanel() {
    leftPanel = new QWidget(this);
    leftPanelLayout = new QVBoxLayout();
    leftPanelLayout->setContentsMargins(0,2,0,2); // compact padding
    leftPanelLayout->setSpacing(10);
    leftPanel->setLayout(leftPanelLayout);
    leftPanel->setFixedWidth(config.leftPanelWidth); // keep constant regardless of other panels
    leftPanel->setMinimumWidth(config.leftPanelWidth);
    leftPanel->setMaximumWidth(config.leftPanelWidth);
    leftPanel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    leftPanel->setObjectName("leftBar");

    auto mkBtn = [&](QToolButton*& btn, const QString& tip, const QIcon& icon){
        btn = new QToolButton(leftPanel);
        btn->setIcon(icon);
        btn->setToolTip(tip);
        btn->setIconSize(QSize(18,20));
        btn->setAutoRaise(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        leftPanelLayout->addWidget(btn);
    };

    auto loadIcon = [&](const QString& name, QStyle::StandardPixmap fallback){
        QIcon ic(QString(":/icons/%1.svg").arg(name));
        if (ic.isNull()) ic = style()->standardIcon(fallback);
        return ic;
    };

    auto tip = [&](const QString& title, const QString& shortcut){
        return shortcut.isEmpty() ? title : QString("%1\nShortcut: %2").arg(title, shortcut);
    };

    // Top controls
    mkBtn(runButton, tip("Run graph", "Cmd+Enter"), loadIcon("run", QStyle::SP_MediaPlay));
    mkBtn(refreshButton, tip("Refresh graph", "Cmd+R"), loadIcon("refresh", QStyle::SP_BrowserReload));

    mkBtn(toggleCodeButton, tip("Show/Hide Code Editor", ""), loadIcon("code", QStyle::SP_FileIcon));
    toggleCodeButton->setCheckable(true);
    toggleCodeButton->setChecked(true);

    mkBtn(toggleNodesButton, tip("Show/Hide Node List", ""), loadIcon("nodes", QStyle::SP_DirIcon));
    toggleNodesButton->setCheckable(true);
    toggleNodesButton->setChecked(true);

    // push file actions to the bottom
    leftPanelLayout->addStretch();

    mkBtn(openButton, tip("Open", "Cmd+O"), loadIcon("open", QStyle::SP_DialogOpenButton));
    mkBtn(saveButton, tip("Save", "Cmd+S"), loadIcon("save", QStyle::SP_DialogSaveButton));
    mkBtn(saveAsButton, tip("Save As", "Shift+Cmd+S"), loadIcon("save_as", QStyle::SP_DialogSaveButton));

    // removed extra right separator and wrapper to avoid width changes

    // Connections
    connect(runButton, &QToolButton::clicked, this, &MainWindow::onRun);
    connect(refreshButton, &QToolButton::clicked, this, &MainWindow::onRefresh);

    connect(toggleCodeButton, &QToolButton::clicked, this, &MainWindow::onToggleCodeEditor);
    connect(toggleNodesButton, &QToolButton::clicked, this, &MainWindow::onToggleNodes);

    connect(openButton, &QToolButton::clicked, this, &MainWindow::onFileOpen);
    connect(saveButton, &QToolButton::clicked, this, &MainWindow::onFileSave);
    connect(saveAsButton, &QToolButton::clicked, this, &MainWindow::onFileSaveAs);

    updateLeftPanelTheme();
}

void MainWindow::updateLeftPanelTheme() {
    const bool leftOn = nodeList ? nodeList->isVisible() : false;
    const bool rightOn = editWin ? editWin->isVisible() : false;
    const bool bothOn = leftOn && rightOn;

    // sync toggle states if visibility changed elsewhere
    if (toggleNodesButton) toggleNodesButton->setChecked(leftOn);
    if (toggleCodeButton) toggleCodeButton->setChecked(rightOn);

    const QString accent = bothOn ? "#2e7d32" : "#555"; // green when both panels on
    const QString bg = bothOn ? "#2f3437" : "#333";

    const QString style = QString(
        "QWidget#leftBar {"
        "  background:%1;"
        "  border-radius:12px;"
        "}"
        "QToolButton {"
        "  color:#ddd;"
        "  border:none;"
        "  padding:3px;"
        "  border-radius:10px;"
        "}"
        "QToolButton:hover { background:#444; }"
        "QToolButton:checked { background:%2; color:#fff; }"
    ).arg(bg, accent);

    // apply to wrapper (leftPanel currently points to wrapper)
    if (leftPanel) leftPanel->setStyleSheet(style);
}

void MainWindow::onFileOpen() {
    QString filename = FileIO::loadScene(scene, this);
    if (!filename.isEmpty()) {
        setWindowTitleWithFilename(filename);
    }
}

void MainWindow::onFileSave() {
    FileIO::saveScene(scene, this, false);
}

void MainWindow::onFileSaveAs() {
    FileIO::saveScene(scene, this, true);
}

void MainWindow::onToggleNodes() {
    if (!nodeList) return;
    nodeList->setVisible(!nodeList->isVisible());
    if (toggleNodesButton) toggleNodesButton->setChecked(nodeList->isVisible());
    updateLeftPanelTheme();
}

void MainWindow::onToggleCodeEditor() {
    if (!editWin) return;
    editWin->setVisible(!editWin->isVisible());
    if (toggleCodeButton) toggleCodeButton->setChecked(editWin->isVisible());
    updateLeftPanelTheme();
}

void MainWindow::onRun() {
    // Mirror Cmd+Enter behavior: update generated code from the graph
    if (scene) {
        scene->updateEditorCode();
    }
}

void MainWindow::onRefresh() {
    if (scene) {
        scene->resetNodes();
    }
}
