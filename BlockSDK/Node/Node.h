//
// Created by Ruhaan on 29/07/25.
//

#ifndef NODE_H
#define NODE_H
#include <string>

class Scene;
class GraphicsNode;
class WidgetNode;

class Node {
public:
    Node(Scene* scene_, const std::string &title = "Undefine Node");

    Scene* scene;
    std::string _title;

    GraphicsNode *grNode{};

    WidgetNode *content;

};

#endif //NODE_H
