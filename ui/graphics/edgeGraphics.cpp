//
// Created by Ruhaan on 03/08/25.
//

#include "edgeGraphics.h"

#include <QPainter>
#include <Qt>

#include "../../core/nodes/edge.h"
#include "../../core/nodes/socket.h"
#include "../../Common.h"

EdgeGraphics::EdgeGraphics(NodeEdges* edge, QGraphicsPathItem *parent): QGraphicsPathItem(parent), edge(edge) {
    _color = QColor("#001000");
    _selectedColor = QColor("#00ff00");

    _pen = QPen(_color);
    _selectedPen = QPen(_selectedColor);
    _pen_dragging = QPen(_color);
    _pen.setWidthF(2.0);
    _selectedPen.setWidthF(2.0);
    _pen_dragging.setStyle(Qt::DashLine);
    _pen_dragging.setWidth(2.0);
    setFlags(QGraphicsPathItem::ItemIsSelectable);
    setZValue(-1);
}

void EdgeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    auto p = calPath();
    setPath(p);

    if (edge->endSocket == nullptr) {
        painter->setPen(_pen_dragging);
    } else {
        if (!isSelected())
            painter->setPen(_pen);
        else
            painter->setPen(_selectedPen);
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

bool EdgeGraphics::intersectsWith(QPointF p1, QPointF p2) {
    auto cutpath = QPainterPath(QPointF(p1));
    cutpath.lineTo(p2);
    auto p = calPath();
    return cutpath.intersects(p);
}

EdgeGraphicsDirect::EdgeGraphicsDirect(NodeEdges *edge, QGraphicsPathItem *parent): EdgeGraphics(edge, parent) {}

QPainterPath EdgeGraphicsDirect::calPath() {
    auto path = QPainterPath(QPointF(posSource.at(0), posSource.at(1)));
    path.lineTo(posDestination.at(0), posDestination.at(1));
    setPath(path);
    return path;
}

EdgeGraphicsBezier::EdgeGraphicsBezier(NodeEdges *edge, QGraphicsPathItem *parent): EdgeGraphics(edge, parent) {}

QPainterPath EdgeGraphicsBezier::calPath() {
    auto s = posSource;
    auto d = posDestination;
    auto dist = ((d[0] - s[0]) * 0.5);

    auto cpx_s = +dist;
    auto cpx_d = -dist;
    auto cpy_s = 0;
    auto cpy_d = 0;

    auto sspos = edge->startSocket->position;
    if ((s[0] > d[0] && (sspos == RIGHT_TOP || sspos == RIGHT_BOTTOM)) || (s[0] < d[0] && (sspos == LEFT_BOTTOM || sspos == LEFT_TOP))) {
        cpx_d *= -1;
        cpx_s *= -1;

        auto diff = s[1] - d[1];
        auto div_term = 0.00001;
        if (diff != 0) {
            div_term = fabs(diff);
        }
        cpy_d = (diff / div_term) * EDGE_CP_ROUNDNESS;

        diff = d[1] - s[1];
        div_term = 0.00001;
        if (diff != 0) {
            div_term = fabs(diff);
        }
        cpy_s = (diff / div_term) * EDGE_CP_ROUNDNESS;
    }


    QPainterPath path;
    path.moveTo(QPointF(posSource[0], posSource[1]));
    path.cubicTo(QPointF(s[0] + cpx_s, s[1] + cpy_s),
                 QPointF(d[0] + cpx_d, d[1] + cpy_d),
                 QPointF(posDestination[0], posDestination[1]));

    return path;
}

