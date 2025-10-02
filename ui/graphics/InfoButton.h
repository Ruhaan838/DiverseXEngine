#ifndef INFOBUTTON_H
#define INFOBUTTON_H

#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>

class NodeGraphics;

class InfoButton : public QGraphicsEllipseItem {
public:
    InfoButton(NodeGraphics* parent, int diameter = 18);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    NodeGraphics* _parent;
    QGraphicsSimpleTextItem* text = nullptr;
};

#endif // INFOBUTTON_H

