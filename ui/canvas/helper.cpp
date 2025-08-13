//
// Created by Ruhaan on 12/08/25.
//

#include "canvasview.h"
#include "../../core/nodes/node.h"
#include "../graphics/nodeGraphics.h"
#include "canvasScene.h"
#include "../graphics/edgeGraphics.h"
#include "../../core/nodes/edge.h"

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