//
// Created by Ruhaan on 03/08/25.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QGraphicsPathItem>
#include <QPen>
#include <vector>

class NodeEdges;

class EdgeGraphics : public QGraphicsPathItem {
public:
    EdgeGraphics(NodeEdges* edge, QGraphicsPathItem* parent=nullptr);

    NodeEdges* edge;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath path() const;
    void setSource(int x, int y);
    void setDestination(int x, int y);

    QPen _pen;
    QPen _selectedPen;
    QColor _color;
    QColor _selectedColor;

    std::vector<int> posSource = {0, 0};
    std::vector<int> posDestination = {200, 100};

    virtual void updatePath() {};
};

class EdgeGraphicsDirect : public EdgeGraphics {
public:
    EdgeGraphicsDirect(NodeEdges* edge, QGraphicsPathItem *parent=nullptr);
    void updatePath() override;
};

class EdgeGraphicsBezier : public EdgeGraphics {
public:
    EdgeGraphicsBezier(NodeEdges* edge, QGraphicsPathItem *parent=nullptr);
    void updatePath() override;
};

#endif //GRAPHICS_H
