//
// Created by Ruhaan on 12/08/25.
//

#include "canvasview.h"
#include "canvasScene.h"
#include "../graphics/nodeGraphics.h"
#include "../graphics/edgeGraphics.h"
#include "../../core/nodes/node.h"
#include "../../core/nodes/edge.h"
#include "../../core/scene/nodescene.h"
#include "../graphics/cutlineGraphics.h"

void CanvasView::deleteSelected() const {
    for (auto crr_item : grScene->selectedItems()) {
        if (auto edg = dynamic_cast<EdgeGraphics*>(crr_item)) {
            edg->edge->remove();
        }
        else if (auto nd = dynamic_cast<NodeGraphics*>(crr_item)) {
            nd->node->remove();
        }
    }

}

void CanvasView::cutIntersectingEdges() const {

    for (int idx = 0; idx < cutline->line_points.size() - 1; idx++) {
        auto p1 = cutline->line_points.at(idx);
        auto p2 = cutline->line_points.at(idx + 1);

        QList<NodeEdges*> toRemove;
        for (auto edge : grScene->scene->edges) {
            if (edge->grEdge && edge->grEdge->intersectsWith(p1, p2)) {
                toRemove.append(edge);
            }
        }
        for (auto edge : toRemove) {
            edge->remove();
        }
    }
}
