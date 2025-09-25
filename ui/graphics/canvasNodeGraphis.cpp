//
// Created by Ruhaan on Canvas Node Graphics
//

#include "canvasNodeGraphis.h"
#include "../../core/widgets/widgets.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QCursor>
#include <QDebug>

CanvasNodeGraphics::CanvasNodeGraphics(Node *node, QGraphicsItem *parent)
    : NodeGraphics(node, parent), resizeHandle(nullptr), isResizing(false) {
    // Set canvas-specific properties
    setTitle("Canvas");

    // Initialize resize handle styling with RGB values instead of hex strings
    _resize_handle_pen = QPen(QColor(255, 250, 55), 2);  // #FFFFA637
    _resize_handle_brush = QBrush(QColor(255, 250, 55, 128));  // #80FFA637 with alpha

    // Make the canvas resizable
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsMovable);

    initResizeHandle();
}

void CanvasNodeGraphics::initResizeHandle() {
    // The resize handle will be drawn directly in paint() method
    // No need for a separate QGraphicsRectItem
}

QRectF CanvasNodeGraphics::getResizeHandleRect() const {
    // Use the public member variables directly since getHeightAndWidth() is not const
    return {static_cast<qreal>(width - RESIZE_HANDLE_SIZE),
            static_cast<qreal>(height - RESIZE_HANDLE_SIZE),
            static_cast<qreal>(RESIZE_HANDLE_SIZE),
            static_cast<qreal>(RESIZE_HANDLE_SIZE)};
}

bool CanvasNodeGraphics::isMouseOnResizeHandle(const QPointF& pos) const {
    return getResizeHandleRect().contains(pos);
}

QRectF CanvasNodeGraphics::boundingRect() const {
    // Use the public member variables directly
    return {0, 0, static_cast<qreal>(width), static_cast<qreal>(height)};
}

void CanvasNodeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // Call parent paint method to draw the basic node
    NodeGraphics::paint(painter, option, widget);

    // Draw the resize handle in the bottom-right corner
    QRectF handleRect = getResizeHandleRect();

    painter->setPen(_resize_handle_pen);
    painter->setBrush(_resize_handle_brush);
    painter->drawRect(handleRect);

    // Draw resize handle grip lines
    painter->setPen(QPen(QColor(255, 255, 255), 1));  // White color
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
        NodeGraphics::mousePressEvent(event);
    }
}

void CanvasNodeGraphics::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (isResizing) {
        QPointF delta = event->pos() - lastMousePos;
        auto hw = getHeightAndWidth();

        // Calculate new dimensions
        int newWidth = std::max(100, static_cast<int>(hw.second + delta.x()));
        int newHeight = std::max(80, static_cast<int>(hw.first + delta.y()));

        // Update the node size
        setHeightWidth(newHeight, newWidth);

        // Update content geometry if it exists
        if (grContent && content) {
            content->setGeometry(static_cast<int>(edge_size),
                               title_height + static_cast<int>(edge_size),
                               newWidth - 2 * static_cast<int>(edge_size),
                               newHeight - 2 * static_cast<int>(edge_size) - title_height);
        }

        lastMousePos = event->pos();
        update();
        event->accept();
    } else {
        // Check if mouse is over resize handle to change cursor
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
    }
}
