//
// Created by Ruhaan on 27/07/25.
//

#ifndef CANVASSCENE_H
#define CANVASSCENE_H

#include <QGraphicsScene>

class Scene;

class CanvasScene : public QGraphicsScene {

public:
    CanvasScene(Scene* scene_, QGraphicsScene *parent = nullptr);
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void setScene(int width, int height);

    QColor _color_background = QColor("#393939");
    QColor _color_light = QColor("#2f2f2f");
    QColor _color_dark = QColor("#292929");

    QPen _pen_light = QPen(_color_light);
    QPen _pen_dark = QPen(_color_dark);

    int grid_size = 20;
    int grid_gap = 5;

    Scene *scene{};


};

#endif //CANVASSCENE_H
