#include "InfoButton.h"
#include "nodeGraphics.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QBrush>
#include <QColor>
#include <QFont>

InfoButton::InfoButton(NodeGraphics* parent, int diameter)
    : QGraphicsEllipseItem(parent), _parent(parent) {
    setRect(0, 0, diameter, diameter);
    setBrush(QBrush(QColor(220, 220, 220)));
    setPen(Qt::NoPen);
    setAcceptHoverEvents(true);

    text = new QGraphicsSimpleTextItem("!", this);
    QFont f = text->font();
    f.setBold(true);
    f.setPointSize(std::max(8, diameter * 2 / 3));
    text->setFont(f);

    QRectF tr = text->boundingRect();
    qreal tx = (rect().width() - tr.width()) / 2.0;
    qreal ty = (rect().height() - tr.height()) / 2.0 - 1;
    text->setPos(tx, ty);
}

void InfoButton::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (_parent) _parent->toggleInfoPopup();
    QGraphicsEllipseItem::mousePressEvent(event);
}

void InfoButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    setBrush(QBrush(QColor(255, 255, 255)));
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void InfoButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    setBrush(QBrush(QColor(220, 220, 220)));
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}

