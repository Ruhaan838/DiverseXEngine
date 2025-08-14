//
// Created by Ruhaan on 14/08/25.
//

#ifndef CUTLINEGRAPHICS_H
#define CUTLINEGRAPHICS_H

#include <QGraphicsItem>
#include <QPen>
#include <QPointF>
#include <QVector>


class CutLineGraphics : public QGraphicsItem {
public:
    CutLineGraphics(QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    QPen _pen;
    QVector<QPointF> line_points = {};
};

#endif //CUTLINEGRAPHICS_H
