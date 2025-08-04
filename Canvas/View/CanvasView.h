//
// Created by Ruhaan on 29/07/25.
//

#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <vector>
#include "../../Common.h"

class CanvasScene;

class CanvasView : public QGraphicsView {
public:
    explicit CanvasView(CanvasScene *scene_, QWidget *parent = nullptr);
    CanvasScene *grScene;
    float zoomInFactor = 1.25;
    float zoom = 10;
    bool zoomClap = true;
    float zoomStep = 1;
    std::vector<float> zoomRange = {0, 20};

    //draging socket
    EDGEDRAGMODS mode = MODE_NO_OP;
    QPointF last_lmb_click_scene_pos;



    void initUI();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // void wheelEvent(QWheelEvent *event) override;


    //helper methods
    void middleMouseButtonPress(QMouseEvent *event);
    void middleMouseButtonRelease(QMouseEvent *event);

    void leftMouseButtonPress(QMouseEvent *event);
    void leftMouseButtonRelease(QMouseEvent *event);

    void rightMouseButtonPress(QMouseEvent *event);
    void rightMouseButtonRelease(QMouseEvent *event);

    QGraphicsItem* getItemAtClick(QMouseEvent* event);
    bool edgeDragStart(QGraphicsItem* item);
    bool edgeDragEnd(QGraphicsItem* item);
    bool distanceBetween(QMouseEvent* event);

};

#endif //CANVASVIEW_H
