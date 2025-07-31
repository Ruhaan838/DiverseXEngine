//
// Created by Ruhaan on 29/07/25.
//

#ifndef SOCKET_H
#define SOCKET_H

#include "../../Common.h"

class SocketGraphics;
class Node;

class SocketNode {
public:
    SocketNode(Node* node_, int index = 0, POSITION position = LEFT_TOP);

    SocketGraphics* grSocket{};
    int index;
    Node* node;
    POSITION position;

};

#endif //SOCKET_H
