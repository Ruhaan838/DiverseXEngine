//
// Created by Ruhaan on 29/07/25.
//

#include "Graphics.h"

#include <iostream>
#include <QDebug>
#include <QPainter>

#include "../Node.h"
#include "../Widget/Widget.h"

GraphicsNode::GraphicsNode(Node *node, QGraphicsItem *parent) : QGraphicsItem(parent), node(node) {

        _title_color = Qt::white;
        _title_font = QFont("Ubuntu", 12);
        _pen_default =  QPen(QColor("#7F000000"));
        _pen_selected =  QPen(QColor("#FFFFA637"));

        _title_brush = QBrush(QColor("#FF313131"));
        _bg_brush = QBrush(QColor("#E3212121"));
        content = node->content;

        initUI();
        initTitle();
        setTitle("Undefine Node");

        // init sockets

        //init content
        initContent();

}

void GraphicsNode::initUI() {
    setFlags(QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable);
}

void GraphicsNode::initTitle() {
    title_item = new QGraphicsTextItem(this);
    title_item->setDefaultTextColor(_title_color);
    // qDebug() << "Title: " << QString::fromStdString(_title);
    // std::cout << "The Title is: " << _title << std::endl;
    title_item->setFont(_title_font);
    title_item->setPos(_padding, 0);
    title_item->setTextWidth(width - (2 * _padding));
}

void GraphicsNode::initContent() {
    grContent = new QGraphicsProxyWidget(this);
    content->setGeometry(edge_size, title_height + edge_size,
        width - 2 * edge_size, height - 2 * edge_size - title_height);
    grContent->setWidget(content);
}

void GraphicsNode::setTitle(const std::string& title) {
    _title = title;
    if (title_item) {
        title_item->setPlainText(QString::fromStdString(_title));
    }
}

std::string GraphicsNode::getTitle() {
    return _title;
}

QRectF GraphicsNode::boundingRect() const {
    return QRectF(0, 0, width, height).normalized();
}

void GraphicsNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) {
    //title
    auto path_title = QPainterPath();
    path_title.setFillRule(Qt::WindingFill);
    path_title.addRoundedRect(0, 0, width, title_height, edge_size, edge_size);
    path_title.addRect(0, title_height - edge_size, edge_size, edge_size);
    path_title.addRect(width - edge_size, title_height - edge_size, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_title_brush);
    painter->drawPath(path_title.simplified());

    //content
    auto path_content = QPainterPath();
    path_content.setFillRule(Qt::WindingFill);
    path_content.addRoundedRect(0, title_height, width, height - title_height, edge_size, edge_size);
    path_content.addRect(0, title_height, edge_size, edge_size);
    path_content.addRect(width - edge_size, title_height, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_bg_brush);
    painter->drawPath(path_content.simplified());

    // outline
    auto path_outline = QPainterPath();
    path_outline.addRoundedRect(0, 0, width, height, edge_size, edge_size);
    if (!isSelected())
        painter->setPen(_pen_default);
    else
        painter->setPen(_pen_selected);

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_outline.simplified());
}

void GraphicsNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
    node->updateConnectedEdges();
}
