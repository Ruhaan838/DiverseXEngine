//
// Created by Ruhaan on 29/07/25.
//

#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>

class CanvasScene;

class CanvasView : public QGraphicsView {
public:
    explicit CanvasView(CanvasScene *scene_, QWidget *parent = nullptr);
    CanvasScene *scene;

    void initUI();
};

#endif //CANVASVIEW_H
