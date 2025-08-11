//
// Created by Ruhaan on 11/08/25.
//

#include "CanvasView.h"
#include "../../BlockSDK/Node/Node.h"
#include "../../BlockSDK/Node/NodeGraphics.h"
#include "../Scene/CanvasScene.h"
#include "../../BlockSDK/Node/Edges/Graphics.h"
#include "../../BlockSDK/Node/Edges/NodeEdgs.h"


void CanvasView::deleteSelected() const {
    for (auto crr_item : grScene->selectedItems()) {
        if (auto edg = dynamic_cast<EdgeGraphics*>(crr_item)) {
            edg->edge->remove();
        }
        else if (auto nd = dynamic_cast<GraphicsNode*>(crr_item)) {
            nd->node->remove();
        }
    }

}

