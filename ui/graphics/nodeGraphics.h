//
// Created by Ruhaan on 12/08/25.
//

#ifndef NODEGRAPHICS_H
#define NODEGRAPHICS_H

#include <QGraphicsItem>
#include <Qt>
#include <QGraphicsProxyWidget>
#include <QRectF>
#include <QPen>
#include <QWidget>

class Node;
class WidgetNode;

class NodeGraphics : public QGraphicsItem {
public:
    NodeGraphics(Node *node, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void initUI();
    void initTitle();
    void initContent();

    void setTitle(const std::string&  title);
    std::string getTitle();

    Node* node;
    WidgetNode* content;
    QGraphicsProxyWidget* grContent;


    std::string _title;
    QGraphicsTextItem *title_item;
    Qt::GlobalColor _title_color;
    QFont _title_font;
    int title_height = 24;

    int width = 180;
    int height = 240;
    float edge_size = 10.0;
    int _padding = 4.0;

    QPen _pen_default;
    QPen _pen_selected;

    QBrush _title_brush;
    QBrush _bg_brush;

};


#endif //NODEGRAPHICS_H
