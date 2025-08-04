//
// Created by Ruhaan on 29/07/25.
//

#include "Node.h"

#include <iostream>

#include "../../Canvas/Scene/CanvasScene.h"
#include "../Scene/NodeScene.h"
#include "Edges/NodeEdgs.h"
#include "Graphics/Graphics.h"
#include "Widget/Widget.h"
#include "Socket/Socket.h"

Node::Node(Scene *scene_, const std::string &title, std::vector<SOCKETTYPES> input_size, std::vector<SOCKETTYPES> output_size) : scene(scene_) {

    content = new WidgetNode();

    grNode = new GraphicsNode(this);
    grNode->setTitle(title);

    scene->addNode(this);
    scene->grScene->addItem(grNode);

    //create the socket for input
    int counter = 0;
    for (auto item : input_size) {
        auto *s = new SocketNode(this, counter, POSITION::LEFT_BOTTOM, item);
        counter++;
        inputs.emplace_back(s);
    }

    //create the socket for output
    counter = 0;
    for (auto item : output_size) {
        auto *s = new SocketNode(this, counter, POSITION::RIGHT_TOP, item);
        counter++;
        outputs.emplace_back(s);
    }
}

std::pair<int, int> Node::getSocketPos(int index, POSITION position) {
    int x = 0, y = 0;

    if (position == POSITION::LEFT_TOP || position == POSITION::LEFT_BOTTOM) {x = 0;}
    else {x = grNode->width;}
    if (position == POSITION::LEFT_BOTTOM || position == RIGHT_BOTTOM)
        y = grNode->height - grNode->edge_size - grNode->_padding - index * socket_spacing;
    else
        y = grNode->title_height + grNode->_padding + grNode->edge_size + index * socket_spacing;

    return {x, y};
}

void Node::setPos(int x, int y) {
    x_pos = x; y_pos = y;
    grNode->setPos(x, y);
}

std::pair<int, int> Node::Pos() {
    return {x_pos, y_pos};
}

QPointF Node::pos() const {
    return grNode->pos();
}

void Node::updateConnectedEdges() const {


    size_t max_size = std::max(inputs.size(), outputs.size());
    
    for (size_t i = 0; i < max_size; i++) {
        if (i < inputs.size() && inputs[i]->hasEdge()) {
            inputs[i]->edge->updatePos();
        }
        
        if (i < outputs.size() && outputs[i]->hasEdge()) {
            outputs[i]->edge->updatePos();
        }
    }
}
