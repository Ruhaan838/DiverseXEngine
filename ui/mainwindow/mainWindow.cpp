//
// Created by Ruhaan on 12/08/25.
//


#include "mainWindow.h"

#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QSplitter>

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
    layout->setContentsMargins(
        0, 0, 0, 0
    );
    setLayout(layout);

    scene = new Scene();
    grScene = scene->grScene;
    view = new CanvasView(grScene);
    view->setScene(grScene);

    editWin = new EditorWindow(this);

    scene->setEditorWindow(editWin);
    editWin->setScene(scene);

    auto* nodeList = new NodeRegistery(scene);

    splitter = new QSplitter(config.splitterOrientation, this);
    splitter->addWidget(nodeList);
    splitter->addWidget(view);
    splitter->addWidget(editWin);

    layout->addWidget(splitter);

    setupMenuBar();
    setupFileActions();

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

void MainWindow::setupMenuBar() {
    menuBar = new QMenuBar(this);
    layout->insertWidget(0, menuBar);

    QMenu* fileMenu = menuBar->addMenu("File");

    openAction = new QAction("Open", this);
    openAction->setShortcut(QKeySequence::Open);
    fileMenu->addAction(openAction);

    saveAction = new QAction("Save", this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);

    saveAsAction = new QAction("Save As...", this);
    saveAsAction->setShortcut(QKeySequence());
    fileMenu->addAction(saveAsAction);
}

void MainWindow::setupFileActions() {
    connect(openAction, &QAction::triggered, this, &MainWindow::onFileOpen);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onFileSave);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::onFileSaveAs);
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
