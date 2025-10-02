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
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>

class Node;
class WidgetNode;
class InfoButton;

class NodeGraphics : public QGraphicsItem {
public:
    NodeGraphics(Node *node, QGraphicsItem *parent = nullptr);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void initUI();
    void initTitle();
    void initContent();

    void setTitle(const std::string&  title);

    std::string getTitle();
    std::string getTitle() const;

    void setInfoText(const std::string &text);
    void toggleInfoPopup();
    void updateInfoButtonPos();

    Node* node;
    WidgetNode* content = nullptr;
    QGraphicsProxyWidget* grContent = nullptr;


    std::string _title;
    QGraphicsTextItem *title_item = nullptr;
    Qt::GlobalColor _title_color;
    QFont _title_font;
    int title_height = 24;

    int width = 240;
    int height = 180;

    void setHeightWidth(int h, int w);
    std::pair<int, int> getHeightAndWidth();

    float edge_size = 10.0;
    int _padding = 4.0;

    QPen _pen_default;
    QPen _pen_selected;

    QBrush _title_brush;
    QBrush _bg_brush;

    InfoButton* infoButton = nullptr;
    QGraphicsTextItem* infoPopup = nullptr;
    QGraphicsRectItem* infoPopupBg = nullptr;

};


#endif //NODEGRAPHICS_H
