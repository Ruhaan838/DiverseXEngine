//
// Created by Ruhaan on 12/08/25.
//

#ifndef SOCKETGRAPHICS_H
#define SOCKETGRAPHICS_H

#include <QGraphicsItem>
#include <QColor>
#include <QPen>
#include <QBrush>

#include "../../Common.h"

class NodeGraphics;
class SocketNode;

class SocketGraphics : public QGraphicsItem{
public:
    SocketGraphics(SocketNode *socket, NodeGraphics* parent = nullptr, SOCKETTYPES item = COLOR_1);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    std::string str();

    float radius = 6.0;
    float outline_width = 1.0;
    QColor _color_background;
    QColor _color_outline = QColor("#FF000000");

    QPen _pen;
    QBrush _brush;
    SocketNode *socket;


};

#endif //SOCKETGRAPHICS_H

