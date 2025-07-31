//
// Created by Ruhaan on 29/07/25.
//

#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>
#include "../Common.h"

class Scene;
class GraphicsNode;
class WidgetNode;
class SocketNode;


class Node {
public:
    Node(Scene* scene_, const std::string &title = "Undefine Node", int input_size=0, int output_size=0);

    std::pair<int, int> getSocketPos(int index, POSITION position);

    std::vector<SocketNode*> inputs;
    std::vector<SocketNode*> outputs;


    Scene* scene;
    std::string _title;

    GraphicsNode* grNode;
    WidgetNode *content;

    int socket_spacing = 22;



};

#endif //NODE_H
