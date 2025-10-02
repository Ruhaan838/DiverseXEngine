//
// Created by Ruhaan on 12/08/25.
//

#include "socket.h"

#include <QJsonObject>
#include <sstream>
#include <QDebug>
#include <QGraphicsScene>
#include <QTimer>

#include "edge.h"
#include "node.h"
#include "../serialization/serializator.h"
#include "../../ui/graphics/socketGraphics.h"
#include "../../ui/graphics/nodeGraphics.h"

SocketNode::SocketNode(Node* node_, int index, POSITION position, const QString& socketType) : node(node_), index(index), position(position), Serializable() {

    this->socket_type = socketType;

    auto ans = node ? node->getSocketPos(index, position) : std::pair<int,int>{0,0};
    if (node && node->grNode) {
        grSocket = new SocketGraphics(this, node->grNode, socketType);
        grSocket->setPos(static_cast<qreal>(ans.first), static_cast<qreal>(ans.second));
        grSocket->setZValue(1);
    } else {
        grSocket = new SocketGraphics(this, nullptr, socketType);
        grSocket->setPos(static_cast<qreal>(ans.first), static_cast<qreal>(ans.second));
        grSocket->setZValue(1);
        if (node) {
            Node* n = node;
            int idx = index;
            POSITION pos = position;
            SocketGraphics* sock = grSocket;
            QTimer::singleShot(0, [n, idx, pos, sock]() {
                if (!n || !sock) return;
                if (n->grNode) {

                    if (n->grNode->scene() && sock->scene() != n->grNode->scene()) n->grNode->scene()->addItem(sock);
                    sock->setParentItem(n->grNode);
                    auto a = n->getSocketPos(idx, pos);
                    sock->setPos(static_cast<qreal>(a.first), static_cast<qreal>(a.second));
                }
            });
        }
    }

    grSocket->setZValue(1);

    if (node) {
        node->refreshSocketsAndEdges();
    }

}

SocketNode::~SocketNode() {
    if (grSocket) {
        delete grSocket;
        grSocket = nullptr;
    }
}

std::pair<int, int> SocketNode::getSocketPos() const {
    return {node->getSocketPos(index, position)};
}

void SocketNode::setConnectedEdge(EdgesNode *edge) {
    this->edge = edge;
}

bool SocketNode::hasEdge() const {
    return edge != nullptr;
}

void SocketNode::setEdge(EdgesNode* edge_) {
    edge = edge_;
    if (edge_) {
        QTimer::singleShot(0, [edge_]() {
            if (edge_) edge_->updatePos();
        });
    }
}

string SocketNode::str() {
    ostringstream oss;
    oss << "\t <Socket " <<  hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}

QJsonObject SocketNode::serialize() {
    auto arr = QJsonObject{
        {"id", static_cast<int>(id)},
        {"index", index},
        {"position", position},
        {"socket_type", socket_type}
    };
    return arr;
}

bool SocketNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) {
    auto i = data.value("id");
    id = i.toInt();

    hashmap[std::to_string(i.toInt())] = reinterpret_cast<uintptr_t>(this);

    return true;
}
