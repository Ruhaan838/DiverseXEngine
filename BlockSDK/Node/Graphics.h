
#ifndef GRAPHICS_H
#define GRAPHICS_H
//
// Created by Ruhaan on 29/07/25.
//


#include <QGraphicsItem>
#include <Qt>
#include <QGraphicsProxyWidget>
#include <QRectF>
#include <QPen>
#include <QWidget>

class Node;
class WidgetNode;

class GraphicsNode : public QGraphicsItem {
public:
    GraphicsNode(Node *node, QGraphicsItem *parent = nullptr);
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


#endif //GRAPHICS_H