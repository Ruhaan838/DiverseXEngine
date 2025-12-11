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
#include <QTabWidget>
#include <QFileInfo>
#include <QTabBar>
#include <QIcon>

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

    // central layout: left icon bar + tabs
    centralLayout = new QHBoxLayout();
    centralLayout->setContentsMargins(0,0,0,0);
    centralLayout->setSpacing(0);

    // QTabWidget to host per-tab splitters
    tabs = new QTabWidget(this);
    tabs->setTabsClosable(true);
    tabs->setMovable(true);
    tabs->setDocumentMode(true);

    connect(tabs, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onTabCloseRequested);

    setupLeftIconPanel();

    centralLayout->addWidget(leftPanel);
    centralLayout->addWidget(tabs);
    centralLayout->setStretchFactor(tabs, 1);
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

    // Create initial tab
    createTab("Untitled");
    updateLeftPanelTheme();
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

// Helper: current tab
MainWindow::Tab* MainWindow::currentTab() const {
    if (!tabs) return nullptr;
    int idx = tabs->currentIndex();
    if (idx < 0 || idx >= tabList.size()) return nullptr;
    return tabList.at(idx);
}

QGraphicsView* MainWindow::currentView() const {
    Tab* t = currentTab();
    return t ? static_cast<QGraphicsView*>(t->view) : nullptr;
}

Scene* MainWindow::currentScene() const {
    Tab* t = currentTab();
    return t ? t->scene : nullptr;
}

CanvasScene* MainWindow::currentGraphicsScene() const {
    Tab* t = currentTab();
    return t ? t->grScene : nullptr;
}

int MainWindow::createTab(const QString& title) {
    // Build per-tab components
    auto* t = new Tab();

    t->scene = new Scene();
    t->grScene = t->scene->grScene; // nodescene constructs CanvasScene inside initUI

    // Ensure CanvasScene exists; Scene::initUI sets it, but in this codebase Scene() likely does it itself.
    // If not set yet, create CanvasScene explicitly and bind.
    if (!t->grScene) {
        t->grScene = new CanvasScene(t->scene);
        t->scene->grScene = t->grScene;
    }

    t->view = new CanvasView(t->grScene);
    t->view->setScene(t->grScene);

    t->editor = new EditorWindow(this);
    t->scene->setEditorWindow(t->editor);
    t->editor->setScene(t->scene);

    t->nodeList = new NodeRegistery(t->scene);

    t->splitter = new QSplitter(config.splitterOrientation, this);
    t->splitter->addWidget(t->nodeList);
    t->splitter->addWidget(t->view);
    t->splitter->addWidget(t->editor);
    t->splitter->setHandleWidth(1);
    t->splitter->setStyleSheet("QSplitter::handle{background:#222;}");

    t->container = new QWidget(this);
    auto* contLayout = new QHBoxLayout();
    contLayout->setContentsMargins(0,0,0,0);
    contLayout->setSpacing(0);
    t->container->setLayout(contLayout);
    contLayout->addWidget(t->splitter);

    // Add to tabs; insert before trailing '+' tab if present
    int idx = tabs->addTab(t->container, title.isEmpty() ? QString("Untitled") : title);
    tabs->setCurrentIndex(idx);
    if (idx >= tabList.size()) {
        tabList.append(t);
    } else {
        tabList.insert(idx, t);
    }

    // Default both side panels visible
    t->nodeList->setVisible(true);
    t->editor->setVisible(true);

    // Install custom close button on this tab
    installTabCloseButton(idx);

    return idx;
}

void MainWindow::installTabCloseButton(int index) {
    if (!tabs) return;
    if (index < 0 || index >= tabs->count()) return;
    QTabBar* bar = tabs->tabBar();
    if (!bar) return;

    auto* btn = new QPushButton(bar);
    btn->setFlat(true);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setIcon(QIcon(":/icons/close.svg"));
    btn->setIconSize(QSize(12,12));
    btn->setFixedSize(QSize(18,18));
    btn->setStyleSheet(
        "QPushButton { border: none; border-radius: 9px; background: rgba(255,255,255,0.08); }"
        "QPushButton:hover { background: rgba(255,255,255,0.16); }"
        "QPushButton:pressed { background: rgba(255,255,255,0.24); }"
    );

    QObject::connect(btn, &QPushButton::clicked, this, [this, btn]() {
        if (!tabs) return;
        QTabBar* b = tabs->tabBar();
        if (!b) return;
        for (int i = 0; i < tabs->count(); ++i) {
            if (b->tabButton(i, QTabBar::RightSide) == btn) {
                onTabCloseRequested(i);
                break;
            }
        }
    });

    bar->setTabButton(index, QTabBar::RightSide, btn);
}

void MainWindow::setTabTitle(int index, const QString& title) {
    if (!tabs) return;
    if (index >= 0 && index < tabs->count()) tabs->setTabText(index, title);
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

    // New tab button
    mkBtn(newTabButton, tip("New Tab", "Cmd+T"), style()->standardIcon(QStyle::SP_FileDialogNewFolder));

    // Connections
    connect(runButton, &QToolButton::clicked, this, &MainWindow::onRun);
    connect(refreshButton, &QToolButton::clicked, this, &MainWindow::onRefresh);

    connect(toggleCodeButton, &QToolButton::clicked, this, &MainWindow::onToggleCodeEditor);
    connect(toggleNodesButton, &QToolButton::clicked, this, &MainWindow::onToggleNodes);
    connect(openButton, &QToolButton::clicked, this, &MainWindow::onFileOpen);
    connect(saveButton, &QToolButton::clicked, this, &MainWindow::onFileSave);
    connect(saveAsButton, &QToolButton::clicked, this, &MainWindow::onFileSaveAs);

    connect(newTabButton, &QToolButton::clicked, this, &MainWindow::onNewTab);

    // Shortcut for new tab
    new QShortcut(QKeySequence("Meta+T"), this, SLOT(onNewTab()));

    updateLeftPanelTheme();
}

void MainWindow::updateLeftPanelTheme() {
    Tab* t = currentTab();
    const bool leftOn = t && t->nodeList ? t->nodeList->isVisible() : false;
    const bool rightOn = t && t->editor ? t->editor->isVisible() : false;
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

    if (leftPanel) leftPanel->setStyleSheet(style);
}

void MainWindow::onFileOpen() {
    Scene* sc = currentScene();
    if (!sc) return;
    QString filename = FileIO::loadScene(sc, this);
    if (!filename.isEmpty()) {
        // Set tab title and window title
        int idx = tabs->currentIndex();
        QFileInfo fi(filename);
        setTabTitle(idx, fi.fileName());
        setWindowTitleWithFilename(fi.fileName());
        Tab* t = currentTab();
        if (t) t->currentFilePath = filename;
    }
}

void MainWindow::onFileSave() {
    Scene* sc = currentScene();
    if (!sc) return;
    FileIO::saveScene(sc, this, false);
}

void MainWindow::onFileSaveAs() {
    Scene* sc = currentScene();
    if (!sc) return;
    FileIO::saveScene(sc, this, true);
}

void MainWindow::onToggleNodes() {
    Tab* t = currentTab();
    if (!t || !t->nodeList || !t->splitter) return;
    bool makeVisible = !t->nodeList->isVisible();
    t->nodeList->setVisible(makeVisible);

    // When hiding, collapse the first pane; when showing, restore a reasonable width
    if (makeVisible) {
        QList<int> sizes = t->splitter->sizes();
        if (sizes.size() >= 2) {
            sizes[0] = qMax(200, sizes[0]); // ensure visible width
            sizes[1] = qMax(1, sizes[1]);
            t->splitter->setSizes(sizes);
        }
    } else {
        QList<int> sizes = t->splitter->sizes();
        if (sizes.size() >= 2) {
            sizes[1] += sizes[0];
            sizes[0] = 0; // collapse left pane
            t->splitter->setSizes(sizes);
        }
    }

    if (toggleNodesButton) toggleNodesButton->setChecked(t->nodeList->isVisible());
    updateLeftPanelTheme();
}

void MainWindow::onToggleCodeEditor() {
    Tab* t = currentTab();
    if (!t || !t->editor) return;
    t->editor->setVisible(!t->editor->isVisible());
    if (toggleCodeButton) toggleCodeButton->setChecked(t->editor->isVisible());
    updateLeftPanelTheme();
}

void MainWindow::onRun() {
    // Update generated code from the graph for current tab
    if (Scene* sc = currentScene()) {
        sc->updateEditorCode();
    }
}

void MainWindow::onRefresh() {
    if (Scene* sc = currentScene()) {
        sc->resetNodes();
    }
}

void MainWindow::onNewTab() {
    int idx = createTab("Untitled");
    tabs->setCurrentIndex(idx);
    updateLeftPanelTheme();
}

void MainWindow::onTabChanged(int index) {
    Q_UNUSED(index);
    updateLeftPanelTheme();
    if (tabs) setWindowTitleWithFilename(tabs->tabText(tabs->currentIndex()));
    if (tabs) {
        for (int i = 0; i < tabs->count(); ++i) {
            installTabCloseButton(i);
        }
    }
}

void MainWindow::onTabCloseRequested(int index) {
    if (!tabs) return;
    if (index < 0 || index >= tabs->count()) return;
    bool willBeEmpty = (tabs->count() == 1);
    Tab* t = tabList.takeAt(index);
    QWidget* w = tabs->widget(index);
    tabs->removeTab(index);
    if (w) w->deleteLater();
    if (t) delete t;
    if (willBeEmpty) {
        createTab("Untitled");
    }
    if (tabs->currentIndex() < 0 && tabs->count() > 0) tabs->setCurrentIndex(0);
    updateLeftPanelTheme();
}
