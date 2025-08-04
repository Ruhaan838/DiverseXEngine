//
// Created by Ruhaan on 03/08/25.
//

#include "Graphics.h"

#include <QPainter>
#include <Qt>

EdgeGraphics::EdgeGraphics(NodeEdges* edge, QGraphicsPathItem *parent): QGraphicsPathItem(parent), edge(edge) {
    _color = QColor("#001000");
    _selectedColor = QColor("#00ff00");

    _pen = QPen(_color);
    _selectedPen = QPen(_selectedColor);
    _pen.setWidthF(2.0);
    _selectedPen.setWidthF(2.0);
    setFlags(QGraphicsPathItem::ItemIsSelectable);
    setZValue(-1);
}

void EdgeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    updatePath();
    if (!isSelected()) {
        painter->setPen(_pen);
    }else {
        painter->setPen(_selectedColor);
    }
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());
}

QPainterPath EdgeGraphics::path() const {
    return QGraphicsPathItem::path();
}

void EdgeGraphics::setSource(int x, int y) {
    posSource = {x, y};
}

void EdgeGraphics::setDestination(int x, int y) {
    posDestination = {x, y};
}


EdgeGraphicsDirect::EdgeGraphicsDirect(NodeEdges *edge, QGraphicsPathItem *parent): EdgeGraphics(edge, parent) {}

void EdgeGraphicsDirect::updatePath() {
    auto path = QPainterPath(QPointF(posSource.at(0), posSource.at(1)));
    path.lineTo(posDestination.at(0), posDestination.at(1));
    setPath(path);
}

EdgeGraphicsBezier::EdgeGraphicsBezier(NodeEdges *edge, QGraphicsPathItem *parent): EdgeGraphics(edge, parent) {}

void EdgeGraphicsBezier::updatePath() {
    auto path = QPainterPath(QPointF(posSource.at(0), posSource.at(1)));
    
    double dx = posDestination.at(0) - posSource.at(0);
    // double dy = posDestination.at(1) - posSource.at(1);
    
    QPointF control1(posSource.at(0) + dx * 0.5, posSource.at(1));
    QPointF control2(posDestination.at(0) - dx * 0.5, posDestination.at(1));
    
    path.cubicTo(control1, control2, QPointF(posDestination.at(0), posDestination.at(1)));
    setPath(path);
}

