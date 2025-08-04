//
// Created by Ruhaan on 31/07/25.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QGraphicsItem>
#include <QColor>
#include <QPen>
#include <QBrush>

#include "../../../Common.h"

class GraphicsNode :public QGraphicsItem{};

class SocketGraphics : public QGraphicsItem{
public:
    SocketGraphics(GraphicsNode* parent = nullptr, SOCKETTYPES item = COLOR_1);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;

    float radius = 6.0;
    float outline_width = 1.0;
    QColor _color_background;
    QColor _color_outline = QColor("#FF000000");

    QPen _pen;
    QBrush _brush;


};

#endif //GRAPHICS_H
