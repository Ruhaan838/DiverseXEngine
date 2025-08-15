//
// Created by Ruhaan on 12/08/25.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <utility>

#include "../../Common.h"
#include "../serialization/serializator.h"

class SocketGraphics;
class Node;
class NodeEdges;

class SocketNode : public Serializable {
public:
    SocketNode(Node* node_, int index = 0, POSITION position = LEFT_TOP, SOCKETTYPES item = COLOR_1);
    void setConnectedEdge(NodeEdges* edge = nullptr);
    std::pair<int, int> getSocketPos() const;
    bool hasEdge() const;
    void setEdge(NodeEdges* edge = nullptr);
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, int> hashmap) override;

    std::string str();

    SocketGraphics* grSocket{};
    int index;
    Node *node;
    POSITION position;
    SOCKETTYPES socket_type;

    NodeEdges* edge;

};

#endif //SOCKET_H
