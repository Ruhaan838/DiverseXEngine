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
#include <QDebug>

inline bool DEBUG = false;

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
    if (DEBUG) qDebug() << "we have this st socket and end socket" << startSocket << " " << endSocket;
    if (startSocket != nullptr) {
        if (DEBUG) qDebug() << "\t \t Start Socket:" << startSocket;
        startSocket->setEdge(nullptr);
    }
    if (endSocket != nullptr) {
        if (DEBUG) qDebug() << "\t \t End Socket:" << endSocket;
        endSocket->setEdge(nullptr);
    }
    endSocket = nullptr;
    startSocket = nullptr;
}

void NodeEdges::remove() {
    if (DEBUG) qDebug() << "NodeEdges:remove ~ " << this->str().c_str() << "\nwith (Sockets, Edges):";
    if (DEBUG) qDebug() << "\t remove edge from all sockets";
    remove_from_sockets();

    if (DEBUG) qDebug() << "\t removed the grEdge";
    if (grEdge && scene && scene->grScene) {
        scene->grScene->removeItem(grEdge);
        grEdge = nullptr;
    }
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

NodeEdges::~NodeEdges() {
    if (DEBUG) qDebug() << "NodeEdges destructor called for" << this;
}