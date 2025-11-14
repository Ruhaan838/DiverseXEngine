//
// Created by Ruhaan on 12/08/25.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <utility>
#include <QString>
#include <vector>

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
    // Allows EdgesNode to attach; for outputs this appends, for inputs this replaces
    void setEdge(EdgesNode* edge = nullptr);

    // Multi-edge helpers
    void addEdge(EdgesNode* e);
    void removeEdge(EdgesNode* e);
    const std::vector<EdgesNode*>& getEdges() const { return edges; }
    EdgesNode* getFirstEdge() const;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) override;

    std::string str();

    SocketGraphics* grSocket{};
    int index;
    Node *node;
    POSITION position;
    QString socket_type;

    // Legacy single-edge pointer (remains valid for inputs; for outputs points to first edge if any)
    EdgesNode* edge = nullptr;

    // Multiple edges storage (primarily used for outputs for fan-out)
    std::vector<EdgesNode*> edges;

    // var_name is assigned during code generation to show which variable this socket maps to
    QString var_name;

};

#endif //SOCKET_H
