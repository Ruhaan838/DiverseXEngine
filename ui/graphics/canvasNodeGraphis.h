//
// Created by Ruhaan on Canvas Node Graphics
//

#ifndef CANVASNODEGRAPHICS_H
#define CANVASNODEGRAPHICS_H

#include "nodeGraphics.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class CanvasNodeGraphics : public NodeGraphics {
public:
    CanvasNodeGraphics(Node *node, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    [[nodiscard]] QRectF boundingRect() const override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void initResizeHandle();
    [[nodiscard]] QRectF getResizeHandleRect() const;
    [[nodiscard]] bool isMouseOnResizeHandle(const QPointF& pos) const;

    QGraphicsRectItem* resizeHandle;
    bool isResizing;
    QPointF lastMousePos;

    // Resize handle properties
    static constexpr int RESIZE_HANDLE_SIZE = 10;
    QPen _resize_handle_pen;
    QBrush _resize_handle_brush;
};

#endif // CANVASNODEGRAPHICS_H
