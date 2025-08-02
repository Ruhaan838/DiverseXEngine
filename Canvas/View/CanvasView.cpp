//
// Created by Ruhaan on 29/07/25.
//

#include "CanvasView.h"

#include <qevent.h>

#include "../Scene/CanvasScene.h"
#include <Qt>

CanvasView::CanvasView(CanvasScene *scene_, QWidget *parent)
    : QGraphicsView(scene_, parent), grScene(scene_) {
    initUI();

    setScene(grScene);


}

void CanvasView::initUI() {
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

}

void CanvasView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        middleMouseButtonPress(event);
    } else if (event->button() == Qt::LeftButton) {
        // printf("Left button pressed\n");
        leftMouseButtonPress(event);
    } else if (event->button() == Qt::RightButton) {
        // printf("Right button pressed\n");
        rightMouseButtonPress(event);
    }
    else {
        QGraphicsView::mousePressEvent(event);
    }
}

void CanvasView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        middleMouseButtonRelease(event);
    } else if (event->button() == Qt::LeftButton) {
        leftMouseButtonRelease(event);
    } else if (event->button() == Qt::RightButton) {
        rightMouseButtonRelease(event);
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void CanvasView::middleMouseButtonPress(QMouseEvent *event) {

    setDragMode(QGraphicsView::ScrollHandDrag);
    auto releaseEvent = QMouseEvent(QEvent::MouseButtonRelease, event->localPos(),
        event->screenPos(), Qt::LeftButton, Qt::NoButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(&releaseEvent);
    auto fakeEvent = QMouseEvent(event->type(), event->localPos(), event->screenPos(),
        Qt::LeftButton, event->buttons() | Qt::LeftButton, event->modifiers());
    QGraphicsView::mousePressEvent(&fakeEvent);
}

void CanvasView::middleMouseButtonRelease(QMouseEvent *event) {
    auto fakeEvent = QMouseEvent(event->type(), event->localPos(), event->screenPos(),
        Qt::LeftButton, event->buttons() & Qt::LeftButton, event->modifiers());
    QGraphicsView::mouseReleaseEvent(&fakeEvent);
    setDragMode(QGraphicsView::NoDrag);
}

// void CanvasView::wheelEvent(QWheelEvent *event) {
//     // calculate out zoom Factr
//     float zoomOutFactor = 1 / zoomInFactor;
//
//     //calcualte the zoom
//     float zoomFactor;
//     if (event->angleDelta().y() > 0) {
//         zoomFactor = zoomInFactor;
//         zoom += zoomStep;
//     }
//     else {
//         zoomFactor = zoomOutFactor;
//         zoom -= zoomStep;
//     }
//
//     //clamping
//     bool clamped = false;
//     if (zoom < zoomRange[0]) {
//         zoom = zoomRange[0];
//         clamped = true;
//     } else if (zoom > zoomRange[1]) {
//         zoom = zoomRange[1];
//         clamped = true;
//     }
//
//     //set secene scale
//     if (!clamped | zoomClap == false)
//         scale(zoomFactor, zoomFactor);
// }


//helper methods
void CanvasView::leftMouseButtonPress(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::leftMouseButtonRelease(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::rightMouseButtonPress(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::rightMouseButtonRelease(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}