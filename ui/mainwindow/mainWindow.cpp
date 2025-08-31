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
#include "../../core/scene/nodescene.h"
#include "../../core/noderegistry/noderegister.h"

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

    auto* nodeList = new NodeRegistery(scene);

    splitter = new QSplitter(config.splitterOrientation, this);
    splitter->addWidget(nodeList);
    splitter->addWidget(view);
    splitter->addWidget(editWin);

    layout->addWidget(splitter);

    setWindowTitle(config.windowTitle);
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
