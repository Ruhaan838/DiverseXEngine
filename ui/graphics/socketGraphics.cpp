//
// Created by Ruhaan on 12/08/25.
//

#include "socketGraphics.h"
#include "../../core/nodes/socket.h"
#include "../../core/nodes/node.h"
#include "nodeGraphics.h"

#include <QPainter>
#include <qtextstream.h>
#include <sstream>
#include <QGraphicsSceneMouseEvent>

SocketGraphics::SocketGraphics(SocketNode *socket, NodeGraphics *parent, const QString& socketType): QGraphicsItem(parent), socket(socket) {

    _color_background = getSocketColor(socketType);

    _pen = QPen(_color_outline);
    _pen.setWidthF(outline_width);
    _brush = QBrush(_color_background);

    socketTypeText = socketType;
    textFont = QFont("Arial", 10);
    textColor = QColor(QColor::fromRgb(255, 255, 255));

    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}


void SocketGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    painter->setBrush(_brush);
    painter->setPen(_pen);
    painter->drawEllipse(-radius, -radius, 2 * radius, 2 * radius);

    if (!socketTypeText.isEmpty() && socket && socket->node) {
        if (socketTypeText == "addsocket") {
            QFont plusFont = textFont;
            plusFont.setPointSize(12);
            plusFont.setBold(true);
            painter->setFont(plusFont);
            painter->setPen(QPen(textColor));
            QFontMetrics fm(plusFont);
            int textWidth = fm.horizontalAdvance("+");
            int textHeight = fm.height();
            qreal baselineY = static_cast<qreal>(fm.ascent()) - static_cast<qreal>(textHeight) / 2.0;
            QPointF textPos(-static_cast<qreal>(textWidth) / 2.0, baselineY);
            painter->drawText(textPos, "+");
            return;
        }

        painter->setFont(textFont);
        painter->setPen(QPen(textColor));

        POSITION socketPosition = socket->position;
        QFontMetrics fm(textFont);
        int textWidth = fm.horizontalAdvance(socketTypeText);
        int textHeight = fm.height();

        QPointF textPos;

        if (socketPosition == LEFT_TOP || socketPosition == LEFT_BOTTOM) {
            textPos = QPointF(radius + textOffset, textHeight / 4);
        } else {
            textPos = QPointF(-radius - textOffset - textWidth, textHeight / 4);
        }

        painter->drawText(textPos, socketTypeText);
    }
}

QRectF SocketGraphics::boundingRect() const {
    if (!socketTypeText.isEmpty() && socket && socket->node) {
        QFontMetrics fm(textFont);
        int textWidth = fm.horizontalAdvance(socketTypeText);
        int textHeight = fm.height();

        POSITION socketPosition = socket->position;

        if (socketPosition == LEFT_TOP || socketPosition == LEFT_BOTTOM) {
            return QRectF(-radius - outline_width, -radius - outline_width,
                         radius + textOffset + textWidth + outline_width,
                         qMax(2 * radius + 2 * outline_width, (float)textHeight));
        } else {
            return QRectF(-radius - textOffset - textWidth - outline_width, -radius - outline_width,
                         radius + textOffset + textWidth + outline_width,
                         qMax(2 * radius + 2 * outline_width, (float)textHeight));
        }
    }

    return QRectF(-radius - outline_width, -radius - outline_width,
                  2 * radius + 2 * outline_width, 2 * radius + 2 * outline_width);
}


void SocketGraphics::mousePressEvent(QGraphicsSceneMouseEvent *event) {


    if (!socket || !socket->node) {
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    if (socket->socket_type == "addsocket") {
        Node* node = socket->node;

        if (event->button() == Qt::RightButton) {
            node->removeLastInputSocket();
            event->accept();
            return;
        }


        if (event->button() == Qt::LeftButton) {
            int addsockIndex = socket->index;
            int insertIndex = addsockIndex;
            node->addInputSocket(insertIndex, "");
            event->accept();
            return;
        }
    }

    QGraphicsItem::mousePressEvent(event);
}


std::string SocketGraphics::str() {
    std::ostringstream oss;
    oss << "\t <SocketGraphics " <<  std::hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}
