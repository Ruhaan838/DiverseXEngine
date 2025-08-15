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

class NodeEdges : public Serializable {
public:
    NodeEdges(Scene *scene, SocketNode* start_socket, SocketNode* end_socket, EDGETYPES type = EDGE_TYPE_DIRECT);

    void updatePos() const;
    void remove_from_sockets();
    void remove();
    string str();
    void setDestination(int x, int y);
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, int> hashmap) override;

    Scene* scene;
    SocketNode* startSocket;
    SocketNode* endSocket;
    EDGETYPES edge_type;

    EdgeGraphics* grEdge;


};

#endif //EDGE_H
