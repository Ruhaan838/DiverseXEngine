//
// Created by Ruhaan on 12/08/25.
//

#ifndef SOCKETGRAPHICS_H
#define SOCKETGRAPHICS_H

#include <QGraphicsItem>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QFont>

#include "../../Common.h"

class NodeGraphics;
class SocketNode;

class SocketGraphics : public QGraphicsItem{
public:
    SocketGraphics(SocketNode *socket, NodeGraphics* parent = nullptr, const QString& socketType = "");

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    std::string str();

    float radius = 6.0;
    float outline_width = 1.0;
    QColor _color_background;
    QColor _color_outline = QColor(0, 0, 0, 255);

    QPen _pen;
    QBrush _brush;
    SocketNode *socket;

    QString socketTypeText;
    QFont textFont;
    QColor textColor;
    float textOffset = 3.0f;
};

#endif //SOCKETGRAPHICS_H
