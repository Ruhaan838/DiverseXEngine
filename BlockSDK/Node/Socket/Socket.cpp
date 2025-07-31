//
// Created by Ruhaan on 29/07/25.
//

#include "Socket.h"
#include "Graphics.h"
#include "../Graphics/Graphics.h"
#include "../Node.h"


SocketNode::SocketNode(Node* node_, int index, POSITION position) : node(node_), index(index), position(position) {
    grSocket = new SocketGraphics(this->node->grNode);
    auto ans = node->getSocketPos(index, position);
    grSocket->setPos(ans.first, ans.second);
}
