//
// Created by Ruhaan on 12/08/25.
//

#include "mainWindow.h"

#include <QColor>
#include <QPushButton>
#include <QTextEdit>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QDebug>

#include "../canvas/canvasview.h"
#include "../canvas/canvasScene.h"
#include "../graphics/edgeGraphics.h"
#include "../graphics/nodeGraphics.h"
#include "../graphics/socketGraphics.h"
#include "../../core/nodes/node.h"
#include "../../core/nodes/edge.h"
#include "../../core/scene/nodescene.h"

MainWindow::MainWindow(QWidget *parent) : QWidget(parent) {
    //load the style file like CSS
    initUI();
    // addDebugContent();
}

void MainWindow::initUI() {

    // Set window size first, then center it
    resize(800, 600);

    // Center the window on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

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

    addNodes();

    setWindowTitle("DiversXEngine");
    show();
}

void MainWindow::addNodes() {
    auto *node = new Node(scene, "Awesome Node", {COLOR_1, COLOR_2, COLOR_3}, {COLOR_2});
    node->setPos(-350, -150);

    auto *node1 = new Node(scene, "Awesome Node 1", {COLOR_3, COLOR_4, COLOR_5}, {COLOR_2});
    node1->setPos(200, -250);

    auto *node2 = new Node(scene, "Awesome Node 2", {COLOR_6, COLOR_1, COLOR_3}, {COLOR_2});
    node2->setPos(-50, 50);

    if (!node->outputs.empty() && !node1->inputs.empty()) {
        auto *edge1 = new EdgesNode(scene, node->outputs.at(0), node2->inputs.at(0), EDGE_TYPE_BEZIER);
        auto *edge2 = new EdgesNode(scene, node2->outputs.at(0), node1->inputs.at(1), EDGE_TYPE_BEZIER);
    } else {
        qDebug() << "Warning: Cannot create edge - missing sockets";
    }
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

