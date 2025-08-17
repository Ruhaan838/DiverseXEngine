//
// Created by Ruhaan on 12/08/25.
//

#include "socket.h"

#include <QJsonObject>
#include <sstream>

#include "node.h"
#include "../serialization/serializator.h"
#include "../../ui/graphics/socketGraphics.h"

SocketNode::SocketNode(Node* node_, int index, POSITION position, SOCKETTYPES item) : node(node_), index(index), position(position), Serializable() {

    grSocket = new SocketGraphics(this, node->grNode, item);

    auto ans = node->getSocketPos(index, position);
    grSocket->setPos(ans.first, ans.second);
    edge = nullptr;

    socket_type = item;

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

void SocketNode::setEdge(EdgesNode *edge_) {
    edge = edge_;
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
