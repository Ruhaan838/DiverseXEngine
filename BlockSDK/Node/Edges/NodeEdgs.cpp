//
// Created by Ruhaan on 03/08/25.
//

#include "NodeEdgs.h"

#include <iostream>
#include <sstream>

#include "../../Scene/NodeScene.h"
#include "Graphics.h"
#include "../Node.h"
#include "../../../Canvas/Scene/CanvasScene.h"
#include "../Socket/Socket.h"


NodeEdges::NodeEdges(Scene *scene, SocketNode *start_socket, SocketNode *end_socket, EDGETYPES type) : scene(scene), startSocket(start_socket), endSocket(end_socket) {

    if (type == EDGE_TYPE_DIRECT){
        grEdge = new EdgeGraphicsDirect(this);
    } else if (type == EDGE_TYPE_BEZIER) {
        grEdge = new EdgeGraphicsBezier(this);
    }
    this->scene->grScene->addItem(grEdge);
    this->scene->addEdge(this);

    startSocket->setEdge(this);
    if (endSocket != nullptr) {
        endSocket->setEdge(this);
    }

    updatePos();

}

void NodeEdges::updatePos() const {
    if (startSocket == nullptr || grEdge == nullptr) {
        return;
    }
    
    auto xy = startSocket->getSocketPos();
    xy.first += startSocket->node->pos().x();
    xy.second += startSocket->node->pos().y();

    grEdge->setSource(xy.first, xy.second);
    if (endSocket != nullptr) {
        xy = endSocket->getSocketPos();
        xy.first += endSocket->node->pos().x();
        xy.second += endSocket->node->pos().y();
        grEdge->setDestination(xy.first, xy.second);
    } else {
        grEdge->setDestination(xy.first, xy.second);
    }
    grEdge->update();
}


void NodeEdges::remove_from_sockets() {
    if (startSocket != nullptr) {
        startSocket->setEdge(nullptr);
    }
    if (endSocket != nullptr) {
        endSocket->setEdge(nullptr);
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

string NodeEdges::str() {
    ostringstream oss;
    oss << "\t <Edge " <<  hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}

void NodeEdges::setDestination(int x, int y) {
    grEdge->setDestination(x, y);
    grEdge->update();
}

