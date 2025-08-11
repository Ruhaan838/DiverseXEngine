//
// Created by Ruhaan on 29/07/25.
//


#include <iostream>

#include "Graphics.h"
#include "Socket.h"
#include <sstream>
#include "../Node.h"

SocketNode::SocketNode(Node* node_, int index, POSITION position, SOCKETTYPES item) : node(node_), index(index), position(position) {

    grSocket = new SocketGraphics(this, node->grNode, item);

    auto ans = node->getSocketPos(index, position);
    grSocket->setPos(ans.first, ans.second);
    edge = nullptr;

}

std::pair<int, int> SocketNode::getSocketPos() const {
    return {node->getSocketPos(index, position)};
}

void SocketNode::setConnectedEdge(NodeEdges *edge) {
    this->edge = edge;
}

bool SocketNode::hasEdge() const {
    return edge != nullptr;
}

void SocketNode::setEdge(NodeEdges *edge_) {
    edge = edge_;
}

string SocketNode::str() {
    ostringstream oss;
    oss << "\t <Socket " <<  hex << reinterpret_cast< uintptr_t>(this) << ">";
    return oss.str();
}
