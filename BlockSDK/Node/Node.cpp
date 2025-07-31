//
// Created by Ruhaan on 29/07/25.
//

#include "Node.h"

#include "../../Canvas/Scene/CanvasScene.h"
#include "../Scene/NodeScene.h"
#include "Graphics/Graphics.h"
#include "Widget/Widget.h"
#include "Socket/Socket.h"


Node::Node(Scene *scene_, const std::string &title, int input_size, int output_size) : scene(scene_) {
    content = new WidgetNode();

    grNode = new GraphicsNode(this);
    grNode->setTitle(title);
    scene->addNode(this);
    scene->grScene->addItem(grNode);

    //create the socket for input
    int counter = 0;
    for (int i = 0; i < input_size; i++) {
        auto *s = new SocketNode(this, counter, POSITION::LEFT_BOTTOM);
        counter++;
        inputs.emplace_back(s);
    }
    //create the socket for output
    counter = 0;
    for (int i = 0; i < output_size; i++) {
        auto *s = new SocketNode(this, counter, POSITION::RIGHT_BOTTOM);
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

