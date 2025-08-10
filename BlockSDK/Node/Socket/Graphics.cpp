//
// Created by Ruhaan on 31/07/25.
//

#include "Graphics.h"
#include "Socket.h"
#include "../Graphics.h"

#include <QPainter>
#include <qtextstream.h>
#include <sstream>

SocketGraphics::SocketGraphics(SocketNode *socket, GraphicsNode *parent, SOCKETTYPES item): QGraphicsItem(parent), socket(socket) {

    _color_background = getSocketColor(item);

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
        3 * (radius - outline_width), 3 * (radius - outline_width)};
}


std::string SocketGraphics::str() {
    std::ostringstream oss;
    oss << "\t <SocketGraphics " <<  hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}
