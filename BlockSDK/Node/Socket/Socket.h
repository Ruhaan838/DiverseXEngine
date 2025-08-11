//
// Created by Ruhaan on 29/07/25.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <utility>

#include "../../../Common.h"

class SocketGraphics;
class Node;
class NodeEdges;

class SocketNode {
public:
    SocketNode(Node* node_, int index = 0, POSITION position = LEFT_TOP, SOCKETTYPES item = COLOR_1);
    void setConnectedEdge(NodeEdges* edge = nullptr);
    std::pair<int, int> getSocketPos() const;
    bool hasEdge() const;
    void setEdge(NodeEdges* edge = nullptr);

    std::string str();

    SocketGraphics* grSocket{};
    int index;
    Node *node;
    POSITION position;

    NodeEdges* edge;

};

#endif //SOCKET_H
