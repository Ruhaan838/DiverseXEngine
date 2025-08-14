//
// Created by Ruhaan on 14/08/25.
//

#include "cutlineGraphics.h"

#include <QPainter>

CutLineGraphics::CutLineGraphics(QGraphicsItem *parent) : QGraphicsItem(parent) {

    _pen = QPen(Qt::white);
    _pen.setWidthF(2.0);
    _pen.setDashPattern({3, 3});

    setZValue(2);
}

QRectF CutLineGraphics::boundingRect() const {
    return {0, 0, 1, 1};
}

void CutLineGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(_pen);

    auto poly = QPolygonF(line_points);
    painter->drawPolyline(poly);
}
