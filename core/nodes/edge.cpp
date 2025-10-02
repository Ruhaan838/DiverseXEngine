//
// Created by Ruhaan on 12/08/25.
//

#include "edge.h"

#include <iostream>
#include <sstream>
#include <QDebug>
#include <QTimer>

#include "node.h"
#include "socket.h"
#include "../scene/nodescene.h"
#include "../serialization/serializator.h"
#include "../../ui/graphics/edgeGraphics.h"
#include "../../ui/canvas/canvasScene.h"
#include "../../ui/graphics/socketGraphics.h"
#include "../../ui/graphics/nodeGraphics.h"

inline bool DEBUG = false;

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

    QPointF sourceScenePos;
    if (startSocket->grSocket) {
        sourceScenePos = startSocket->grSocket->mapToScene(QPointF(0,0));
    } else {
        auto xy = startSocket->getSocketPos();
        QPointF localPos(static_cast<qreal>(xy.first), static_cast<qreal>(xy.second));
        if (startSocket->node && startSocket->node->grNode) {
            sourceScenePos = startSocket->node->grNode->mapToScene(localPos);
        } else {
            sourceScenePos = localPos;
        }
    }

    grEdge->setSource(static_cast<int>(sourceScenePos.x()), static_cast<int>(sourceScenePos.y()));

    if (endSocket != nullptr) {
        QPointF destScenePos;
        if (endSocket->grSocket) {
            destScenePos = endSocket->grSocket->mapToScene(QPointF(0,0));
        } else {
            auto xy = endSocket->getSocketPos();
            QPointF localPos(static_cast<qreal>(xy.first), static_cast<qreal>(xy.second));
            if (endSocket->node && endSocket->node->grNode) {
                destScenePos = endSocket->node->grNode->mapToScene(localPos);
            } else {
                destScenePos = localPos;
            }
        }
        grEdge->setDestination(static_cast<int>(destScenePos.x()), static_cast<int>(destScenePos.y()));
    } else {
        grEdge->setDestination(static_cast<int>(sourceScenePos.x()), static_cast<int>(sourceScenePos.y()));
    }

    grEdge->update();
}


void EdgesNode::remove_from_sockets() {
    if (startSocket != nullptr) {
        startSocket = nullptr;
    }
    if (endSocket != nullptr) {
        endSocket = nullptr;
    }
    endSocket = nullptr;
    startSocket = nullptr;
}

void EdgesNode::remove() {
    remove_from_sockets();

    if (grEdge && scene && scene->grScene && grEdge != nullptr) {
        scene->grScene->removeItem(grEdge);
        grEdge = nullptr;
    }
    if (scene) {
        scene->removeEdge(this);
    }
}

void EdgesNode::setStartSocket(SocketNode* socket) {
    if (socket != nullptr && socket->socket_type == "addsocket") {
        if (DEBUG) qDebug() << "EdgesNode::setStartSocket - attempt to connect to addsocket ignored";
        return;
    }

    startSocket = socket;
    if (startSocket != nullptr) {
        startSocket->setEdge(this);
    }
}

SocketNode *EdgesNode::getStartSocket() const {
    return startSocket;
}

void EdgesNode::setEndSocket(SocketNode* socket) {
    if (socket != nullptr && socket->socket_type == "addsocket") {
        if (DEBUG) qDebug() << "EdgesNode::setEndSocket - attempt to connect to addsocket ignored";
        return;
    }

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

    QTimer::singleShot(0, [this]() {
        this->updatePos();
    });

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
