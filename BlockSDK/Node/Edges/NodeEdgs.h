//
// Created by Ruhaan on 03/08/25.
//

#ifndef NODEEGDS_H
#define NODEEGDS_H

class Scene;
class SocketNode;
class EdgeGraphics;

#include "../../../Common.h"
using namespace std;

class NodeEdges {
public:
    NodeEdges(Scene *scene, SocketNode* start_socket, SocketNode* end_socket, EDGETYPES type = EDGE_TYPE_DIRECT);

    void updatePos() const;
    void remove_from_sockets();
    void remove();
    string str();

    Scene* scene;
    SocketNode* startSocket;
    SocketNode* endSocket;

    EdgeGraphics* grEdge;

    void setDestination(int x, int y);

};

#endif
