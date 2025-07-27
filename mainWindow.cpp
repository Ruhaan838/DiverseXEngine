//
// Created by Ruhaan on 27/07/25.
//

#include "mainWindow.h"


MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    initUI();
}

void MainWindow::initUI() {

    int mid_h = this->height() / 2;
    int mid_w = this->width() / 2;
    setGeometry(mid_w, mid_h, 800, 600);

    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // create Graphics scene
    grScene = new CanvasScene();


    //graphics View
    view = new QGraphicsView(this);
    view->setScene(grScene);
    layout->addWidget(view);

    setWindowTitle("DiversXEngine");
    show();
}

