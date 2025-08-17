//
// Created by Ruhaan on 12/08/25.
//

#include "edge.h"

#include <iostream>
#include <sstream>
#include <QDebug>

#include "node.h"
#include "socket.h"
#include "../scene/nodescene.h"
#include "../serialization/serializator.h"
#include "../../ui/graphics/edgeGraphics.h"
#include "../../ui/canvas/canvasScene.h"

inline bool DEBUG = true;

EdgesNode::EdgesNode(Scene *scene, SocketNode *start_socket, SocketNode *end_socket, EDGETYPES type) : scene(scene), Serializable() {


    this->scene->addEdge(this);

    setStartSocket(start_socket);
    setEndSocket(end_socket);

    setEdgeType(type);

}

void EdgesNode::updatePos() const {
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


void EdgesNode::remove_from_sockets() {
    if (DEBUG) qDebug() << "we have this st socket and end socket" << startSocket << " " << endSocket;
    if (startSocket != nullptr) {
        if (DEBUG) qDebug() << "\t \t Start Socket:" << startSocket;

        startSocket = nullptr;
    }
    if (endSocket != nullptr) {
        if (DEBUG) qDebug() << "\t \t End Socket:" << endSocket;
        endSocket = nullptr;
    }
    endSocket = nullptr;
    startSocket = nullptr;
}

void EdgesNode::remove() {
    if (DEBUG) qDebug() << "NodeEdges:remove ~ " << this->str().c_str() << "\nwith (Sockets, Edges):";
    if (DEBUG) qDebug() << "\t remove edge from all sockets";
    remove_from_sockets();

    if (DEBUG) qDebug() << "\t removed the grEdge";
    if (grEdge && scene && scene->grScene && grEdge != nullptr) {
        scene->grScene->removeItem(grEdge);
        grEdge = nullptr;
    }
    if (scene) {
        scene->removeEdge(this);
    }
}

void EdgesNode::setStartSocket(SocketNode* socket) {
    startSocket = socket;
    if (startSocket != nullptr) {
        startSocket->setEdge(this);
    }
}

SocketNode *EdgesNode::getStartSocket() const {
    return startSocket;
}

void EdgesNode::setEndSocket(SocketNode* socket) {
    endSocket = socket;
    if (endSocket != nullptr) {
        endSocket->setEdge(this);
    }
}

SocketNode *EdgesNode::getEndSocket() const {
    return endSocket;
}

EDGETYPES EdgesNode::getEdgeType() const {
    return edge_type;
}

void EdgesNode::setEdgeType(EDGETYPES type) {
    if (this->grEdge && grEdge != nullptr) {
        scene->grScene->removeItem(grEdge);
    }
    edge_type = type;
    if (edge_type == EDGE_TYPE_DIRECT) {
        grEdge = new EdgeGraphicsDirect(this);
    } else if (edge_type == EDGE_TYPE_BEZIER) {
        grEdge = new EdgeGraphicsBezier(this);
    } else {
        grEdge = new EdgeGraphicsBezier(this);
    }
    scene->grScene->addItem(grEdge);

    updatePos();

}

string EdgesNode::str() {
    ostringstream oss;
    oss << "\t <Edge " <<  hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}

void EdgesNode::setDestination(int x, int y) const {
    grEdge->setDestination(x, y);
    grEdge->update();
}

QJsonObject EdgesNode::serialize() {
    auto arr = QJsonObject{
        {"id", static_cast<int>(id)},
        {"edge_type", edge_type},
        {"start", static_cast<int>(startSocket->id)},
        {"end", static_cast<int>(endSocket->id)}
    };
    return arr;
}

bool EdgesNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) {
    auto i = data.value("id");
    id = i.toInt();
    hashmap[std::to_string(i.toInt())] = reinterpret_cast<uintptr_t>(this);

    int start_id = data.value("start").toInt();
    int end_id = data.value("end").toInt();

    if (hashmap.count(std::to_string(start_id))) {
        setStartSocket(reinterpret_cast<SocketNode*>(hashmap[std::to_string(start_id)]));
    }
    if (hashmap.count(std::to_string(end_id))) {
        setEndSocket(reinterpret_cast<SocketNode*>(hashmap[std::to_string(end_id)]));
    }

    setEdgeType(getEdgeEnum(data.value("edge_type").toInt()));

    return true;
}
