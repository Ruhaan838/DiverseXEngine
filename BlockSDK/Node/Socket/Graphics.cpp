//
// Created by Ruhaan on 31/07/25.
//

#include "Graphics.h"
#include "../Graphics/Graphics.h"
#include <QPainter>

SocketGraphics::SocketGraphics(GraphicsNode *parent): QGraphicsItem(parent) {
    _pen = QPen(_color_outline);
    _pen.setWidthF(outline_width);
    _brush = QBrush(_color_background);
}

void SocketGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    //paint circle
    painter->setBrush(_brush);
    painter->setPen(_pen);
    painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);

}

QRectF SocketGraphics::boundingRect() const {
    return {-radius - outline_width, -radius - outline_width,
        2 * (radius - outline_width), 2 * (radius - outline_width)};
}

