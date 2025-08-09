//
// Created by Ruhaan on 29/07/25.
//


#include <iostream>

#include "Graphics.h"
#include "Socket.h"
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
