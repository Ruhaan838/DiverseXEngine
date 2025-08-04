//
// Created by Ruhaan on 03/08/25.
//

#include "NodeEdgs.h"

#include <iostream>


#include "../../Scene/NodeScene.h"
#include "Graphics.h"
#include "../Node.h"
#include "../../../Canvas/Scene/CanvasScene.h"
#include "../Socket/Graphics.h"
#include "../Socket/Socket.h"


NodeEdges::NodeEdges(Scene *scene, SocketNode *start_socket, SocketNode *end_socket, EDGETYPES type) : scene(scene), startSocket(start_socket), endSocket(end_socket) {

    if (type == EDGE_TYPE_DIRECT){
        grEdge = new EdgeGraphicsDirect(this);
    } else if (type == EDGE_TYPE_BEZIER) {
        grEdge = new EdgeGraphicsBezier(this);
    }
    this->scene->grScene->addItem(grEdge);

    startSocket->setEdge(this);
    endSocket->setEdge(this);

    updatePos();

}

void NodeEdges::updatePos() const {
    auto xy = startSocket->getSocketPos();
    xy.first += startSocket->node->pos().x();
    xy.second += startSocket->node->pos().y();

    grEdge->setSource(xy.first, xy.second);
    if (endSocket != nullptr) {
        xy = endSocket->getSocketPos();
        xy.first += endSocket->node->pos().x();
        xy.second += endSocket->node->pos().y();

        grEdge->setDestination(xy.first, xy.second);
    }
    grEdge->update();
}


void NodeEdges::remove_from_sockets() {
    if (startSocket != nullptr) {
        startSocket->edge = nullptr;
    }
    if (endSocket != nullptr) {
        endSocket->edge = nullptr;
    }
    endSocket = nullptr;
    startSocket = nullptr;
}

void NodeEdges::remove() {
    remove_from_sockets();
    scene->grScene->removeItem(grEdge);
    grEdge = nullptr;
    scene->removeEdge(this);
}
