//
// Created by Ruhaan on 27/07/25.
//

#include "mainWindow.h"

#include <QColor>
#include <QPushButton>
#include <QTextEdit>
#include <QApplication>

#include "BlockSDK/Node/Graphics/Graphics.h"
#include "BlockSDK/Node/Node.h"
#include "BlockSDK/Node/Socket/Socket.h"
#include "BlockSDK/Scene/NodeScene.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    //load the style file like CSS
    initUI();
    // addDebugContent();
}

void MainWindow::initUI() {

    const int mid_h = this->height() / 2;
    const int mid_w = this->width() / 2;
    setGeometry(mid_w, mid_h, 800, 600);

    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // create Graphics scene
    scene = new Scene();
    grScene = scene->grScene;


    //graphics View
    view = new CanvasView(grScene);
    view->setScene(grScene);
    layout->addWidget(view);

    const auto *node = new Node(scene, "Awesome Node", 2, 1);
    node->grNode->setPos(-250, -150);

    setWindowTitle("DiversXEngine");
    show();
}

void MainWindow::addDebugContent() {
    auto greenBrush = QBrush(Qt::darkGreen);
    auto outlinePen = QPen(Qt::black);
    outlinePen.setWidth(2);

    auto *rect = grScene->addRect(-100, -100, 80, 100, outlinePen, greenBrush);
    rect->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    rect->setPos(-200, -100);

    auto *text = grScene->addText("This is the New Awesome Text", QFont("Ubuntu"));
    text->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    text->setDefaultTextColor(QColor::fromRgbF(1.0, .7, .1));
    text->setPos(0, 150);

    auto *widget1 = new QPushButton("Hello World");
    auto proxy1 = grScene->addWidget(widget1);
    proxy1->setPos(-50, 100);
    proxy1->setFlags(QGraphicsItem::ItemIsSelectable);

    auto *line = grScene->addLine(-200, -100, -110, 200, outlinePen);
    line->setPos(-100, 50);
    line->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}