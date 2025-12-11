//
// Created by Ruhaan on 12/08/25.
//

#include "canvasScene.h"
#include "../../core/scene/nodescene.h"
#include <QPainter>

using namespace std;

CanvasScene::CanvasScene(Scene* scene_ , QGraphicsScene *parent) : QGraphicsScene(parent) {
    scene = scene_;
    setBackgroundBrush(_color_background);

    _pen_light.setWidth(1);
    _pen_dark.setWidth(2);
    // Soften grid colors slightly
    QColor light = _pen_light.color(); light.setAlpha(70); _pen_light.setColor(light);
    QColor dark = _pen_dark.color(); dark.setAlpha(110); _pen_dark.setColor(dark);
}

void CanvasScene::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsScene::drawBackground(painter, rect);

    // Inset the grid a bit so it doesn't touch the view edges
    const int margin = 8;
    QRectF r = rect.adjusted(margin, margin, -margin, -margin);

    // create the grid of Canvas
    int left = floor(r.left());
    int right = ceil(r.right());
    int top = floor(r.top());
    int bottom = ceil(r.bottom());

    auto first_left = left - (left % grid_size);
    auto first_top = top - (top % grid_size);

    // compute all lines to be drawn
    vector<QLine> lines_light, lines_dark;

    for (int x = first_left; x <= right; x += grid_size) {
        if (x % (grid_size * grid_gap) != 0) {
            lines_light.emplace_back(x, top, x, bottom);
        } else {
            lines_dark.emplace_back(x, top, x, bottom);
        }
    }

    for (int y = first_top; y <= bottom; y += grid_size) {
        if (y % (grid_size * grid_gap) != 0) {
            lines_light.emplace_back(left, y, right, y);
        } else {
            lines_dark.emplace_back(left, y, right, y);
        }
    }

    // draw the lines
    painter->setPen(_pen_light);
    painter->drawLines(lines_light.data(), static_cast<uint8_t>(lines_light.size()));

    painter->setPen(_pen_dark);
    painter->drawLines(lines_dark.data(), static_cast<uint8_t>(lines_dark.size()));

}

void CanvasScene::setScene(int width, int height) {
    setSceneRect(-width / 2, -height / 2, width, height);
}
