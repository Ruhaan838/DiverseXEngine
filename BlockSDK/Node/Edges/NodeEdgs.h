//
// Created by Ruhaan on 03/08/25.
//

#ifndef NODEEGDS_H
#define NODEEGDS_H

class Scene;
class SocketNode;
class EdgeGraphics;

#include "../../../Common.h"

class NodeEdges {
public:
    NodeEdges(Scene *scene, SocketNode* start_socket, SocketNode* end_socket, EDGETYPES type = EDGE_TYPE_DIRECT);

    void updatePos() const;
    void remove_from_sockets();
    void remove();

    Scene* scene;
    SocketNode* startSocket;
    SocketNode* endSocket;

    EdgeGraphics* grEdge;

};

#endif
