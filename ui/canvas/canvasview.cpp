//
// Created by Ruhaan on 13/08/25.
//

#include "canvasview.h"

#include <QApplication>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>
#include <qevent.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

#include "canvasScene.h"
#include "../../core/noderegistry/function/functionNode.h"
#include "../graphics/socketGraphics.h"
#include "../graphics/cutlineGraphics.h"
#include "../../core/nodes/node.h"
#include "../../core/nodes/edge.h"
#include "../../core/nodes/socket.h"
#include "../../core/scene/nodescene.h"
#include "../graphics/nodeGraphics.h"
#include "../graphics/canvasNodeGraphis.h"
#include "../../core/noderegistry/canvasNode.h"
#include "../../core/noderegistry/noderegister.h"

inline bool DEBUG = false;

CanvasView::CanvasView(CanvasScene *scene_, QWidget *parent)
    : QGraphicsView(scene_, parent), grScene(scene_) {

    initUI();
    setScene(grScene);
    setDragMode(QGraphicsView::RubberBandDrag);

    cutline = new CutLineGraphics();
    grScene->addItem(cutline);

}

void CanvasView::initUI() {
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // accept drags from node palette
    setAcceptDrops(true);
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

    auto releaseEvent = QMouseEvent(
        QEvent::MouseButtonRelease,
        event->pos(),
        event->globalPos(),
        Qt::LeftButton,
        Qt::NoButton,
        event->modifiers()
    );
    QGraphicsView::mouseReleaseEvent(&releaseEvent);

    auto fakeEvent = QMouseEvent(
        event->type(),
        event->pos(),
        event->globalPos(),
        Qt::LeftButton,
        event->buttons() | Qt::LeftButton,
        event->modifiers()
    );
    QGraphicsView::mousePressEvent(&fakeEvent);
}

void CanvasView::middleMouseButtonRelease(QMouseEvent *event) {
    auto fakeEvent = QMouseEvent(
        event->type(),
        event->pos(),
        event->globalPos(),
        Qt::LeftButton,
        event->buttons() & Qt::LeftButton,
        event->modifiers()
    );
    QGraphicsView::mouseReleaseEvent(&fakeEvent);
    setDragMode(QGraphicsView::NoDrag);
}

void CanvasView::applyZoom(float zoomFactor) {
    float zoomInFactor = 1.25f;
    float zoomOutFactor = 1.0f / zoomInFactor;

    if (zoomFactor > 1.0f) {
        zoom += zoomStep;
        zoomFactor = zoomInFactor;
    } else {
        zoom -= zoomStep;
        zoomFactor = zoomOutFactor;
    }

    bool clamped = false;
    if (zoom < zoomRange[0]) {
        zoom = zoomRange[0];
        clamped = true;
    }
    if (zoom > zoomRange[1]) {
        zoom = zoomRange[1];
        clamped = true;
    }

    if (!clamped || zoomClamp == false) {
        scale(zoomFactor, zoomFactor);
    }
}

bool CanvasView::event(QEvent *ev) {
    if (ev->type() == QEvent::NativeGesture) {
        auto *ng = static_cast<QNativeGestureEvent*>(ev);

        switch (ng->gestureType()) {
            case Qt::ZoomNativeGesture: {
                // pinch zoom
                float zoomFactor = 1.0f + ng->value();
                applyZoom(zoomFactor);
                return true;
            }
            case Qt::PanNativeGesture: {
                qreal panValue = ng->value();
                if (qAbs(panValue) > 0) {
                    verticalScrollBar()->setValue(verticalScrollBar()->value() - panValue);
                }
                return true;
            }
            default:
                break;
        }
    }
    return QGraphicsView::event(ev);
}

void CanvasView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData() && event->mimeData()->hasFormat(kNodeMimeType)) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CanvasView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData() && event->mimeData()->hasFormat(kNodeMimeType)) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void CanvasView::dropEvent(QDropEvent *event) {
    if (!(event->mimeData() && event->mimeData()->hasFormat(kNodeMimeType))) {
        event->ignore();
        return;
    }

    QString nodeName = QString::fromUtf8(event->mimeData()->data(kNodeMimeType));
    if (nodeName.isEmpty() || !grScene || !grScene->scene) {
        event->ignore();
        return;
    }

    // map drop point to scene coordinates
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QPointF viewPt = event->position();
#else
    QPointF viewPt = event->pos();
#endif
    QPointF scenePt = mapToScene(viewPt.toPoint());

    Node* node = NodeRegistery::createNode(nodeName, grScene->scene);
    if (!node) {
        event->ignore();
        return;
    }

    grScene->scene->addNode(node);
    node->show();
    node->setPos(static_cast<int>(scenePt.x()), static_cast<int>(scenePt.y()));

    event->acceptProposedAction();
}

void CanvasView::wheelEvent(QWheelEvent *event) {
    bool isTrackpad = (event->source() == Qt::MouseEventSynthesizedBySystem);

    if (isTrackpad && event->pixelDelta() != QPoint(0, 0) &&
        !(event->modifiers() & Qt::ControlModifier)) {

        QPoint pixelDelta = event->pixelDelta();

        QScrollBar *hBar = horizontalScrollBar();
        QScrollBar *vBar = verticalScrollBar();

        hBar->setValue(hBar->value() - pixelDelta.x());
        vBar->setValue(vBar->value() - pixelDelta.y());

        event->accept();
        return;
    }

    const int numDegrees = event->angleDelta().y() / 8;
    const int numSteps = numDegrees / 15;

    if (numSteps == 0) {
        event->ignore();
        return;
    }

    float zoomOutFactor = 1.0f / zoomInFactor;
    float zoomFactor;

    if (numSteps > 0) {
        zoomFactor = zoomInFactor;
        zoom += zoomStep;
    } else {
        zoomFactor = zoomOutFactor;
        zoom -= zoomStep;
    }

    bool clamped = false;
    if (zoom < zoomRange[0]) {
        zoom = zoomRange[0];
        clamped = true;
    } else if (zoom > zoomRange[1]) {
        zoom = zoomRange[1];
        clamped = true;
    }

    if (!clamped || zoomClamp == false) {
        scale(zoomFactor, zoomFactor);
        event->accept();
    }
}

void CanvasView::keyPressEvent(QKeyEvent *event) {
    if ((event->key() == Qt::Key_R) && ((event->modifiers() & Qt::ControlModifier) || (event->modifiers() & Qt::MetaModifier))) {
        // Ctrl/Cmd + R: reset all node input/display values
        if (grScene && grScene->scene) {
            grScene->scene->resetNodes();
            return; // consume shortcut
        }
    }
    if (event->key() == Qt::Key_Delete) {
        if (!editingFlag)
            this->deleteSelected();
        else
            QGraphicsView::keyPressEvent(event);
    }
    else if (event->key() == Qt::Key_S) {
        const auto mods = event->modifiers();
        const bool cmdOrCtrl = (mods & Qt::ControlModifier) || (mods & Qt::MetaModifier);
        const bool hasShift = (mods & Qt::ShiftModifier);
        if (cmdOrCtrl && !hasShift) {
            grScene->scene->saveToFile("Graph.json");
            return;
        }
        QGraphicsView::keyPressEvent(event);
    }
    else if ((event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && (event->modifiers() & Qt::ControlModifier)) {
        if (grScene && grScene->scene) {
            grScene->scene->executeGraph();
        }
    }
    else if ((event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) && (event->modifiers() & Qt::ControlModifier)) {
        // Ctrl + "+"
        applyZoom(1 + zoomStep);
    }
    else if (event->key() == Qt::Key_Minus && (event->modifiers() & Qt::ControlModifier)) {
        // Ctrl + "-"
        applyZoom(1 - zoomStep);
    }
    else if (event->key() == Qt::Key_G) {
        if (grScene && grScene->scene) {
            grScene->scene->updateEditorCode();
        }
    }
    else if (event->key() == Qt::Key_R) {
        // Detach selected nodes from their Canvas parent (unchanged behavior for plain R)
        bool anyDetached = false;
        auto items = scene() ? scene()->selectedItems() : QList<QGraphicsItem*>();
        for (auto *it : items) {
            auto *ng = dynamic_cast<NodeGraphics*>(it);
            if (!ng || !ng->node) continue;
            QGraphicsItem* parent = ng->parentItem();
            while (parent) {
                if (auto *cng = dynamic_cast<CanvasNodeGraphics*>(parent)) {
                    auto *canvas = dynamic_cast<CanvasNode*>(cng->node);
                    if (canvas) {
                        canvas->removeNode(ng->node);
                        anyDetached = true;
                    }
                    break;
                }
                parent = parent->parentItem();
            }
        }
        if (!anyDetached) {
            QGraphicsView::keyPressEvent(event);
        }
    }
    else {
        QGraphicsView::keyPressEvent(event);
    }
}

//helper methods
void CanvasView::leftMouseButtonPress(QMouseEvent *event) {
    last_lmb_click_scene_pos = mapToScene(event->pos());
    auto item = getItemAtClick(event);

    static FunctionNode* lastActiveNode = nullptr;

    FunctionNode* clickedFn = nullptr;
    if (auto n = dynamic_cast<NodeGraphics*>(item)) {
        clickedFn = dynamic_cast<FunctionNode*>(n->node);
    }

    if (lastActiveNode && lastActiveNode != clickedFn) {
        lastActiveNode->hideContent();
        lastActiveNode = nullptr;
    }

    if (auto sk = dynamic_cast<SocketGraphics*>(item)) {
        // if this is the special addsocket, create a new input instead of starting an edge drag
        if (sk->socket && sk->socket->socket_type == "addsocket") {
            Node* node = sk->socket->node;
            if (node) {
                int addsockIndex = sk->socket->index;
                int insertIndex = addsockIndex; // insert before addsocket
                //QString label = "Number";
                //node->addInputSocket(insertIndex, label);
                // pass empty label so addInputSocket will auto-number (Number N)
                node->addInputSocket(insertIndex, QString());
             }
             return;
         }

        if (mode == MODE_NO_OP) {
            mode = MODE_EDGE_DRAG;
            edgeDragStart(sk);
            return;
        }
    }

    if (clickedFn) {
        clickedFn->showContent();
        lastActiveNode = clickedFn;
    }

    if (mode == MODE_EDGE_DRAG) {
        if (edgeDragEnd(item)) {
            return;
        }
    }

    if (!item) {
        if (event->modifiers() & Qt::ShiftModifier) {
            mode = MODE_EDGE_CUT;
            auto fake = QMouseEvent(
                QEvent::MouseButtonRelease,
                event->pos(),
                event->globalPos(),
                Qt::LeftButton,
                Qt::NoButton,
                event->modifiers()
            );
            QGraphicsView::mouseReleaseEvent(&fake);
            QApplication::setOverrideCursor(Qt::CrossCursor);
            return;
        }
        else {
            if (grScene && grScene->scene) {
                grScene->scene->setPendingNodePos(last_lmb_click_scene_pos);
            }
        }

    }

    QGraphicsView::mousePressEvent(event);
}

void CanvasView::leftMouseButtonRelease(QMouseEvent *event) {

    auto item = getItemAtClick(event);

    if (mode == MODE_EDGE_DRAG) {
        if (!distanceBetween(event)) {
            if (edgeDragEnd(item)) {
                return;
            }
        }
    }

    if (mode == MODE_EDGE_CUT) {
        cutIntersectingEdges();
        cutline->line_points = {};
        cutline->update();
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        mode = MODE_NO_OP;
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::rightMouseButtonPress(QMouseEvent *event) {

    if (auto item = getItemAtClick(event)) {
        if (auto sok = dynamic_cast<SocketGraphics*>(item)) {
            if (sok->socket && sok->socket->socket_type == "addsocket") {
                if (DEBUG) qDebug() << "CanvasView::rightMouseButtonPress - addsocket right-click, removing last input";
                sok->socket->node->removeLastInputSocket();
                return;
            }
        }

    } else {
        qDebug("Item is nullptr - printing debug info:");
        qDebug() << "Scene:"  << scene();
        qDebug() << "Nodes:";

        for (const auto n : grScene->scene->nodes) {
            qDebug("%s", n->str().c_str());
        }
        qDebug() << "Edges:";
        for (const auto n : grScene->scene->edges) {
            qDebug("%s", n->str().c_str());
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void CanvasView::rightMouseButtonRelease(QMouseEvent *event) {
    QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent *event) {
    if (mode == MODE_EDGE_DRAG) {
        auto pos = mapToScene(event->pos());
        dragEdge->setDestination(pos.x(), pos.y());
    }
    if (mode == MODE_EDGE_CUT) {
        auto pos = mapToScene(event->pos());
        cutline->line_points.append(pos);
        cutline->update();
    }

    QGraphicsView::mouseMoveEvent(event);
}


QGraphicsItem* CanvasView::getItemAtClick(QMouseEvent *event) {
    auto pos = event->pos();
    return itemAt(pos);
}

bool CanvasView::edgeDragStart(SocketGraphics *item) {
    if (DEBUG) {
        qDebug() << "View::edgeDragStart ~ Start Dragging Edge";
        qDebug() << "View::edgeDragStart ~ assign Start Socket" << item->socket;
    }
    prevEdge = nullptr; // no longer track previous for deletion
    lastStartSocket = item->socket;

    dragEdge = new EdgesNode(grScene->scene, item->socket, nullptr, EDGE_TYPE_BEZIER);

    if (DEBUG) {qDebug() << item << item->socket;};
    if (DEBUG){qDebug() << "View::edgeDragStart ~ dragEdge" << dragEdge;}

    return true;

}

bool CanvasView::edgeDragEnd(QGraphicsItem *item) {
    mode = MODE_NO_OP;

    if (auto sok = dynamic_cast<SocketGraphics*>(item)) {
        if (sok->socket != lastStartSocket){
            if (DEBUG)
                qDebug() << "View::edgeDragEnd ~ assign end Socket" << sok->socket;
            // Do not remove existing edges; SocketNode::setEdge will enforce single-edge for inputs
            dragEdge->startSocket = lastStartSocket;
            dragEdge->endSocket = sok->socket;
            dragEdge->startSocket->setConnectedEdge(dragEdge);
            dragEdge->endSocket->setConnectedEdge(dragEdge);
            if (DEBUG)
                qDebug() << "View::edgeDragEnd ~ assigned start and end to drag edge";

            dragEdge->updatePos();
            return true;
        }
    }

    if (DEBUG)
        qDebug() << "View::edgeDragEnd ~ End Dragging Edge";

    if (dragEdge) {
        dragEdge->remove();
    }
    dragEdge = nullptr;


    if (DEBUG)
        qDebug() << "View: edgeDragEnd ~ everything done.";

    return false;
}

bool CanvasView::distanceBetween(QMouseEvent *event) {
    auto new_lmb_release_scene_pos = mapToScene(event->pos());
    auto dist_scene_pos = new_lmb_release_scene_pos - last_lmb_click_scene_pos;

    double val = (dist_scene_pos.x() * dist_scene_pos.x()) + (dist_scene_pos.y() * dist_scene_pos.y());
    int edge_threshold = EDGE_DRAG_START_THRESHOLD * EDGE_DRAG_START_THRESHOLD;
    return val < edge_threshold;
}