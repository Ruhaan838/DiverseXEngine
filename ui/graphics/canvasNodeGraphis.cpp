//
// Created by Ruhaan on Canvas Node Graphics
//

#include "canvasNodeGraphis.h"
#include "../../core/widgets/widgets.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QCursor>
#include <QDebug>
#include <QGraphicsScene>
#include <QTimer>

#include "socketGraphics.h"
#include "../../core/noderegistry/canvasNode.h"
#include "../../core/nodes/socket.h"
#include "../../core/nodes/edge.h"
#include "../../core/scene/nodescene.h"

CanvasNodeGraphics::CanvasNodeGraphics(Node *node, QGraphicsItem *parent)
    : NodeGraphics(node, parent), resizeHandle(nullptr), isResizing(false) {

    setTitle("Canvas");

    _resize_handle_pen = QPen(QColor(255, 250, 55), 2);
    _resize_handle_brush = QBrush(QColor(255, 250, 55, 128));

    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsScenePositionChanges);

    _bg_brush = QBrush(QColor(0,0,0,0));
}


QRectF CanvasNodeGraphics::getResizeHandleRect() const {

    return {static_cast<qreal>(width - RESIZE_HANDLE_SIZE),
            static_cast<qreal>(height - RESIZE_HANDLE_SIZE),
            static_cast<qreal>(RESIZE_HANDLE_SIZE),
            static_cast<qreal>(RESIZE_HANDLE_SIZE)};
}

bool CanvasNodeGraphics::isMouseOnResizeHandle(const QPointF& pos) const {
    return getResizeHandleRect().contains(pos);
}

QRectF CanvasNodeGraphics::boundingRect() const {

    return {0, 0, static_cast<qreal>(width), static_cast<qreal>(height)};
}

void CanvasNodeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    NodeGraphics::paint(painter, option, widget);

    QRectF handleRect = getResizeHandleRect();

    painter->setPen(_resize_handle_pen);
    painter->setBrush(_resize_handle_brush);
    painter->drawRect(handleRect);

    painter->setPen(QPen(QColor(255, 255, 255), 1));
    for (int i = 0; i < 3; ++i) {
        painter->drawLine(
            QPointF(handleRect.left() + 2.0 + static_cast<qreal>(i) * 2.0,
                    handleRect.bottom() - 2.0),
            QPointF(handleRect.right() - 2.0,
                    handleRect.top() + 2.0 + static_cast<qreal>(i) * 2.0)
        );
    }
}

void CanvasNodeGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton && isMouseOnResizeHandle(event->pos())) {
        isResizing = true;
        lastMousePos = event->pos();
        setCursor(Qt::SizeFDiagCursor);
        event->accept();
    } else {
        if (scene()) {
            prev_selected_items = scene()->selectedItems();
            prev_selected_items.removeAll(this);
        }

        NodeGraphics::mousePressEvent(event);
    }
}

void CanvasNodeGraphics::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isResizing) {
        QPointF delta = event->pos() - lastMousePos;
        auto hw = getHeightAndWidth();

        int newWidth = std::max(100, static_cast<int>(hw.second + delta.x()));
        int newHeight = std::max(80, static_cast<int>(hw.first + delta.y()));

        if (node) {
            node->setHeightWidth(newHeight, newWidth);
        } else {
            setHeightWidth(newHeight, newWidth);
        }

        if (grContent && content) {

            int curW = (node && node->grNode) ? node->grNode->width : newWidth;
            int curH = (node && node->grNode) ? node->grNode->height : newHeight;
            content->setGeometry(static_cast<int>(edge_size),
                               title_height + static_cast<int>(edge_size),
                               curW - 2 * static_cast<int>(edge_size),
                               curH - 2 * static_cast<int>(edge_size) - title_height);
        }

        lastMousePos = event->pos();
        update();
        event->accept();
    } else {

        if (isMouseOnResizeHandle(event->pos())) {
            setCursor(Qt::SizeFDiagCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }

        NodeGraphics::mouseMoveEvent(event);
    }
}

void CanvasNodeGraphics::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (isResizing && event->button() == Qt::LeftButton) {
        isResizing = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        NodeGraphics::mouseReleaseEvent(event);

        if (isSelected()) {
            if (auto *canvas = dynamic_cast<CanvasNode*>(node)) {
                canvas->tryEmbedSelectedNodes();
                // auto-select inner nodes
                for (auto *n : canvas->inner_nodes) {
                    if (n && n->grNode) n->grNode->setSelected(true);
                }
            }
        }
    }
}

QVariant CanvasNodeGraphics::itemChange(GraphicsItemChange change, const QVariant &value) {

    if (change == QGraphicsItem::ItemPositionHasChanged) {
        if (auto *canvas = dynamic_cast<CanvasNode*>(node)) {
            for (auto *n : canvas->inner_nodes) {
                if (!n) continue;

                for (auto *s : n->inputs) {
                    if (!s || !s->grSocket) continue;
                    if (s->grSocket->parentItem() != n->grNode) s->grSocket->setParentItem(n->grNode);
                    auto p = n->getSocketPos(s->index, s->position);
                    s->grSocket->setPos(static_cast<qreal>(p.first), static_cast<qreal>(p.second));
                    s->grSocket->setZValue(1);
                    if (s->hasEdge() && !s->edges.empty()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                }

                for (auto *s : n->outputs) {
                    if (!s || !s->grSocket) continue;
                    if (s->grSocket->parentItem() != n->grNode) s->grSocket->setParentItem(n->grNode);
                    auto p = n->getSocketPos(s->index, s->position);
                    s->grSocket->setPos(static_cast<qreal>(p.first), static_cast<qreal>(p.second));
                    s->grSocket->setZValue(1);
                    if (s->hasEdge() && !s->edges.empty()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                }

                QTimer::singleShot(0, [n]() { if (n) n->refreshSocketsAndEdges(); });

                QTimer::singleShot(0, [n]() {
                    if (!n) return;
                    for (auto *s : n->inputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                    for (auto *s : n->outputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                });

                QTimer::singleShot(20, [n]() {
                    if (!n) return;
                    for (auto *s : n->inputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                    for (auto *s : n->outputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                });

                QTimer::singleShot(60, [n]() {
                    if (!n) return;
                    for (auto *s : n->inputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                    for (auto *s : n->outputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                });

                QTimer::singleShot(5, [n]() {
                    if (!n || !n->grNode) return;
                    n->grNode->moveBy(0.001, 0.001);
                });

                QTimer::singleShot(15, [n]() {
                    if (!n || !n->grNode) return;
                    n->grNode->moveBy(-0.001, -0.001);
                });

            }

            for (auto *n : canvas->inner_nodes) {
                if (!n) continue;
                n->refreshSocketsAndEdges();
                for (auto *s : n->inputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
                for (auto *s : n->outputs) if (s && s->hasEdge()) { for (auto *e : s->edges) if (e) e->updatePos(); }
            }
            Scene* scImmediate = canvas->scene;
            if (scImmediate) {
                for (auto *e : scImmediate->edges) if (e) e->updatePos();
            }

            if (scImmediate) {
                QTimer::singleShot(10, [scImmediate]() { for (auto *e : scImmediate->edges) if (e) e->updatePos(); });
                QTimer::singleShot(30, [scImmediate]() { for (auto *e : scImmediate->edges) if (e) e->updatePos(); });
                QTimer::singleShot(120, [scImmediate]() { for (auto *e : scImmediate->edges) if (e) e->updatePos(); });
            }
            for (auto *n : canvas->inner_nodes) {
                QTimer::singleShot(10, [n]() { if (n) n->refreshSocketsAndEdges(); });
                QTimer::singleShot(30, [n]() { if (n) n->refreshSocketsAndEdges(); });
                QTimer::singleShot(120, [n]() { if (n) n->refreshSocketsAndEdges(); });
            }

             Scene* sc = canvas->scene;
             if (sc) {
                 QTimer::singleShot(0, [sc]() {
                     for (auto *e : sc->edges) if (e) e->updatePos();
                 });
                 QTimer::singleShot(20, [sc]() {
                     for (auto *e : sc->edges) if (e) e->updatePos();
                 });
                 QTimer::singleShot(60, [sc]() {
                     for (auto *e : sc->edges) if (e) e->updatePos();
                 });
             }
             if (scene()) scene()->update();
         }
     }

    if (change == QGraphicsItem::ItemSelectedChange) {
        bool willBeSelected = value.toBool();
        if (willBeSelected && scene()) {

            prev_selected_items = scene()->selectedItems();
            prev_selected_items.removeAll(this);
        }
    }

    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        if (isSelected()) {
            if (auto *canvas = dynamic_cast<CanvasNode*>(node)) {

                QRectF canvasSceneRect = mapToScene(boundingRect()).boundingRect();

                for (auto *it : prev_selected_items) {
                    if (!it) continue;
                    if (it == this) continue;
                    auto *ng = dynamic_cast<NodeGraphics*>(it);
                    if (!ng) continue;

                    if (ng->parentItem() != nullptr) continue;

                    QRectF nodeSceneRect = ng->mapToScene(ng->boundingRect()).boundingRect();
                    if (canvasSceneRect.contains(nodeSceneRect)) {
                        if (ng->node) {
                            canvas->addNode(ng->node);

                            if (ng->node->grNode) ng->node->grNode->setSelected(true);
                        }
                    }
                }

                canvas->updateSizeToFitChildren();
            }
        }

        prev_selected_items.clear();
    }

    return QGraphicsItem::itemChange(change, value);
}
