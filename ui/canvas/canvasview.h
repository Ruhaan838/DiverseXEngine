//
// Created by Ruhaan on 13/08/25.
//

#ifndef CANVASVIEW_H
#define CANVASVIEW_H

#include <QGraphicsView>
#include <vector>
#include "../../Common.h"


class CanvasScene;
class EdgesNode;
class SocketGraphics;
class SocketNode;
class CutLineGraphics;

class CanvasView : public QGraphicsView {
public:
    explicit CanvasView(CanvasScene *scene_, QWidget *parent = nullptr);
    CanvasScene *grScene;
    float zoomInFactor = 0.9f;
    float zoom = 1.0f;
    float zoomStep = 0.05f;
    bool zoomClamp = true;
    std::vector<float> zoomRange = {0.0f, 0.2f};

    EDGEDRAGMODS mode = MODE_NO_OP;
    QPointF last_lmb_click_scene_pos;

    EdgesNode* dragEdge{};
    EdgesNode* prevEdge{};
    SocketNode* lastStartSocket{};
    bool editingFlag = false;

    //cutline
    CutLineGraphics* cutline;


    void initUI();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    //helper methods
    void middleMouseButtonPress(QMouseEvent *event);
    void middleMouseButtonRelease(QMouseEvent *event);

    void leftMouseButtonPress(QMouseEvent *event);
    void leftMouseButtonRelease(QMouseEvent *event);

    void rightMouseButtonPress(QMouseEvent *event);
    void rightMouseButtonRelease(QMouseEvent *event);

    QGraphicsItem* getItemAtClick(QMouseEvent* event);
    bool edgeDragStart(SocketGraphics * item);
    bool edgeDragEnd(QGraphicsItem* item);
    bool distanceBetween(QMouseEvent* event);

    void deleteSelected() const;
    void cutIntersectingEdges() const;

protected:
    bool event(QEvent *ev) override;

    // DnD from node palette
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    void applyZoom(float zoomFactor);
};

#endif //CANVASVIEW_H

