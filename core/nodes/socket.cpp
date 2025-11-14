//
// Created by Ruhaan on 12/08/25.
//

#include "socket.h"

#include <QJsonObject>
#include <sstream>
#include <QDebug>
#include <QGraphicsScene>
#include <QTimer>
#include <algorithm>

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
    this->setEdge(edge);
}

bool SocketNode::hasEdge() const {
    return edge != nullptr || !edges.empty();
}

void SocketNode::setEdge(EdgesNode* edge_) {
    // Fan-out only on output sockets (RIGHT_*). Inputs (LEFT_*) keep single-edge.
    if (!edge_) return;

    bool isOutput = (position == RIGHT_TOP || position == RIGHT_BOTTOM);

    if (isOutput) {
        // Avoid duplicates
        for (auto *e : edges) if (e == edge_) return;
        edges.push_back(edge_);
        if (!edge) edge = edge_;
    } else {
        // Input: replace existing connection
        if (edge && edge != edge_) {
            // Let the old edge detach itself safely; call remove which will call remove_from_sockets
            auto *old = edge;
            edge = nullptr; // prevent recursion mishaps
            // Defer removal to event loop to avoid potential re-entrancy
            QTimer::singleShot(0, [old]() { if (old) old->remove(); });
        }
        edge = edge_;
        // keep edges vector in sync for inputs as size 1
        edges.clear();
        edges.push_back(edge_);
    }

    if (edge_) {
        QTimer::singleShot(0, [edge_]() {
            if (edge_) edge_->updatePos();
        });
    }
}

void SocketNode::addEdge(EdgesNode *e) {
    if (!e) return;
    bool isOutput = (position == RIGHT_TOP || position == RIGHT_BOTTOM);
    if (isOutput) {
        for (auto *x : edges) if (x == e) return;
        edges.push_back(e);
        if (!edge) edge = e;
    } else {
        setEdge(e);
    }
}

void SocketNode::removeEdge(EdgesNode *e) {
    if (!e) return;
    // Remove from vector
    if (!edges.empty()) {
        edges.erase(std::remove(edges.begin(), edges.end(), e), edges.end());
    }
    // Clear legacy pointer if it pointed to this
    if (edge == e) edge = edges.empty() ? nullptr : edges.front();
}

EdgesNode* SocketNode::getFirstEdge() const {
    if (edge) return edge;
    if (!edges.empty()) return edges.front();
    return nullptr;
}

string SocketNode::str() {
    ostringstream oss;
    oss << "\t <Socket " <<  hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}

QJsonObject SocketNode::serialize() {
    auto arr = QJsonObject{
        {"id", QString::fromStdString(std::to_string(id))},
        {"index", index},
        {"position", position},
        {"socket_type", socket_type}
    };
    return arr;
}

bool SocketNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) {
    auto v = data.value("id");
    if (v.isString()) {
        auto s = v.toString();
        id = static_cast<uintptr_t>(s.toULongLong());
        hashmap[s.toStdString()] = reinterpret_cast<uintptr_t>(this);
    } else {
        auto i = v.toInt();
        id = static_cast<uintptr_t>(static_cast<unsigned int>(i));
        hashmap[std::to_string(i)] = reinterpret_cast<uintptr_t>(this);
    }

    return true;
}
