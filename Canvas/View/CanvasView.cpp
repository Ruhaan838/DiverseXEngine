//
// Created by Ruhaan on 29/07/25.
//

#include "CanvasView.h"

#include <qevent.h>
#include <QDebug>

#include "../Scene/CanvasScene.h"
#include "../../BlockSDK/Node/Socket/Graphics.h"


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

    last_lmb_click_scene_pos = mapToScene(event->pos());

    if (auto item = getItemAtClick(event)) {
        if (dynamic_cast<SocketGraphics*>(item)) {
            if (mode == MODE_NO_OP) {
                mode = MODE_EDGE_DRAG;
                edgeDragStart(item);
                return;
            }
        }
        if (mode == MODE_EDGE_DRAG) {
            if (edgeDragEnd(item)) {
                return;
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::leftMouseButtonRelease(QMouseEvent *event) {
    if (auto item = getItemAtClick(event)) {
        if (mode == MODE_EDGE_DRAG) {

            if (distanceBetween(event)) {
                {}
            } else {
                if (edgeDragEnd(item)) {
                    return;
                }
            }
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::rightMouseButtonPress(QMouseEvent *event) {
    QGraphicsView::mousePressEvent(event);
}

void CanvasView::rightMouseButtonRelease(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}

QGraphicsItem* CanvasView::getItemAtClick(QMouseEvent *event) {
    auto pos = event->pos();
    return itemAt(pos);;
}

bool CanvasView::edgeDragStart(QGraphicsItem *item) {
    if (DEBUG) {
        qDebug() << "View::edgeDragStart - Start Dragging Edge";
        qDebug() << "View::edgeDragStart - assign Start Socket";
    }
}

bool CanvasView::edgeDragEnd(QGraphicsItem *item) {
    mode = MODE_NO_OP;
    if (DEBUG)
        qDebug() << "View::edgeDragEnd - End Dragging Edge";
    if (dynamic_cast<SocketGraphics*>(item)) {
        if (DEBUG)
            qDebug() << "View::edgeDragEnd - assign end Socket";
        return true;
    }
    return false;
}

//measures if we are too far from the last LMB click scene position
bool CanvasView::distanceBetween(QMouseEvent *event) {
    auto new_lmb_release_scene_pos = mapToScene(event->pos());
    auto dist_scene_pos = new_lmb_release_scene_pos - last_lmb_click_scene_pos;

    double val = (dist_scene_pos.x() * dist_scene_pos.x()) + (dist_scene_pos.y() * dist_scene_pos.y());
    int edge_threshold = EDGE_DRAG_START_THRESHOLD * EDGE_DRAG_START_THRESHOLD;
    return val < edge_threshold;
}


