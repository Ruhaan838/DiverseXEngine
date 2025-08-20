//
// Created by Ruhaan on 12/08/25.
//

#include "nodeGraphics.h"

#include <iostream>
#include <QDebug>
#include <QPainter>
#include <QVariant>

#include "../../core/nodes/node.h"
#include "../../core/nodes/edge.h"
#include "../../core/widgets/widgets.h"
#include "../canvas/canvasScene.h"
#include "../../core/scene/nodescene.h"
#include "../../core/nodes/socket.h"

inline bool DEBUG = false;

NodeGraphics::NodeGraphics(Node *node, QGraphicsItem *parent) : QGraphicsItem(parent), node(node) {

        _title_color = Qt::white;
        _title_font = QFont("Ubuntu", 12);
        _pen_default =  QPen(QColor("#7F000000"));
        _pen_selected =  QPen(QColor("#FFFFA637"));

        _title_brush = QBrush(QColor("#FF313131"));
        _bg_brush = QBrush(QColor("#E3212121"));

        initUI();
        initTitle();
        setTitle("Undefine Node");

}

void NodeGraphics::initUI() {
    setFlags(QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable);
}

void NodeGraphics::initTitle() {
    title_item = new QGraphicsTextItem(this);
    title_item->setDefaultTextColor(_title_color);
    // qDebug() << "Title: " << QString::fromStdString(_title);
    // std::cout << "The Title is: " << _title << std::endl;
    title_item->setFont(_title_font);
    title_item->setPos(_padding, 0);
    title_item->setTextWidth(width - (2 * _padding));
}

void NodeGraphics::initContent() {
    content = node->content;
    grContent = new QGraphicsProxyWidget(this);
    content->setGeometry(edge_size, title_height + edge_size,
        width - 2 * edge_size, height - 2 * edge_size - title_height);
    grContent->setWidget(content);
}

void NodeGraphics::setTitle(const std::string& title) {
    _title = title;
    if (title_item) {
        title_item->setPlainText(QString::fromStdString(_title));
    }
}

std::string NodeGraphics::getTitle() {
    return _title;
}

QRectF NodeGraphics::boundingRect() const {
    return QRectF(0, 0, width, height).normalized();
}

void NodeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) {
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
    auto hw = getHeightAndWidth();
    path_content.addRoundedRect(0, title_height, hw.second, hw.first - title_height, edge_size, edge_size);
    path_content.addRect(0, title_height, edge_size, edge_size);
    path_content.addRect(hw.second - edge_size, title_height, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_bg_brush);
    painter->drawPath(path_content.simplified());

    // outline
    auto path_outline = QPainterPath();
    path_outline.addRoundedRect(0, 0, hw.second, hw.first, edge_size, edge_size);
    if (!isSelected())
        painter->setPen(_pen_default);
    else
        painter->setPen(_pen_selected);

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_outline.simplified());
}

void NodeGraphics::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);

    if (scene()) {
        auto* nodeScene = dynamic_cast<CanvasScene*>(scene());
        if (nodeScene && nodeScene->scene) {
            for (auto* sceneNode : nodeScene->scene->nodes) {
                if (sceneNode->grNode->isSelected()) {
                    sceneNode->updateConnectedEdges();
                }
            }
        }
    }
}


std::pair<int, int> NodeGraphics::getHeightAndWidth() {
    return {height, width};
}

void NodeGraphics::setHeightWidth(int h, int w) {
    height = h; width = w;
}

