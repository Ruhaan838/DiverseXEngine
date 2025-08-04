//
// Created by Ruhaan on 29/07/25.
//

#ifndef NODE_H
#define NODE_H

#include <qpoint.h>
#include <string>
#include <vector>
#include "../../Common.h"

class Scene;
class WidgetNode;
class SocketNode;
class GraphicsNode;


class Node {
public:
    Node(Scene* scene_, const std::string &title = "Undefine Node", std::vector<SOCKETTYPES> input_size = {COLOR_1}, std::vector<SOCKETTYPES> output_size = {COLOR_2});

    std::pair<int, int> getSocketPos(int index, POSITION position);
    void setPos(int x, int y);
    std::pair<int, int> Pos();
    QPointF pos() const;
    void updateConnectedEdges() const;


    std::vector<SocketNode*> inputs;
    std::vector<SocketNode*> outputs;


    Scene* scene;
    std::string _title;

    GraphicsNode* grNode;
    WidgetNode *content;

    int socket_spacing = 22;
    int x_pos, y_pos;



};

#endif //NODE_H
