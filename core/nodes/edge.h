//
// Created by Ruhaan on 12/08/25.
//

#ifndef EDGE_H
#define EDGE_H


class Scene;
class SocketNode;
class EdgeGraphics;

#include <QJsonObject>
#include "../../Common.h"
#include "../serialization/serializator.h"

using namespace std;

class EdgesNode : public Serializable {
public:
    EdgesNode(Scene *scene, SocketNode* start_socket = nullptr, SocketNode* end_socket = nullptr, EDGETYPES type = EDGE_TYPE_BEZIER);

    void updatePos() const;
    void remove_from_sockets();
    void remove();
    string str();
    void setDestination(int x, int y) const;

    void setStartSocket(SocketNode* socket);
    SocketNode* getStartSocket() const;

    void setEndSocket(SocketNode* socket);
    SocketNode* getEndSocket() const;

    EDGETYPES getEdgeType() const;
    void setEdgeType(EDGETYPES type);


    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) override;

    Scene* scene;
    SocketNode* startSocket = nullptr;
    SocketNode* endSocket = nullptr;
    EDGETYPES edge_type = EDGE_TYPE_DIRECT;

    EdgeGraphics* grEdge = nullptr;


};

#endif //EDGE_H
