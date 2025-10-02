//
// Created by Ruhaan on 12/08/25.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <utility>
#include <QString>

#include "../../Common.h"
#include "../serialization/serializator.h"

class SocketGraphics;
class Node;
class EdgesNode;

class SocketNode : public Serializable {
public:
    SocketNode(Node* node_, int index = 0, POSITION position = LEFT_TOP, const QString& socketType = "Input");
    ~SocketNode();
    void setConnectedEdge(EdgesNode* edge = nullptr);
    std::pair<int, int> getSocketPos() const;
    bool hasEdge() const;
    void setEdge(EdgesNode* edge = nullptr);
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) override;

    std::string str();

    SocketGraphics* grSocket{};
    int index;
    Node *node;
    POSITION position;
    QString socket_type;

    EdgesNode* edge;

    // var_name is assigned during code generation to show which variable this socket maps to
    QString var_name;

};

#endif //SOCKET_H
