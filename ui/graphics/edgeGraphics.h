//
// Created by Ruhaan on 03/08/25.
//

#ifndef EDGEGRAPHICS_H
#define EDGEGRAPHICS_H

#include <QGraphicsPathItem>
#include <QPen>
#include <vector>

class EdgesNode;

class EdgeGraphics : public QGraphicsPathItem {
public:
    EdgeGraphics(EdgesNode* edge, QGraphicsPathItem* parent=nullptr);

    EdgesNode* edge;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath path() const;
    bool intersectsWith(QPointF p1, QPointF p2);
    void setSource(int x, int y);
    void setDestination(int x, int y);

    QPen _pen;
    QPen _selectedPen;
    QPen _pen_dragging;
    QColor _color;
    QColor _selectedColor;


    std::vector<int> posSource = {0, 0};
    std::vector<int> posDestination = {200, 100};

    virtual QPainterPath calPath() { return QPainterPath(); };
};

class EdgeGraphicsDirect : public EdgeGraphics {
public:
    EdgeGraphicsDirect(EdgesNode* edge, QGraphicsPathItem *parent=nullptr);
    QPainterPath calPath() override;
};

class EdgeGraphicsBezier : public EdgeGraphics {
public:
    EdgeGraphicsBezier(EdgesNode* edge, QGraphicsPathItem *parent=nullptr);
    QPainterPath calPath() override;
};

#endif //EDGEGRAPHICS_H
