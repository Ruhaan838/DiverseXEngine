//
// Created by Ruhaan on 12/08/25.
//

#ifndef NODE_H
#define NODE_H

#include <qpoint.h>
#include <string>
#include <vector>
#include "../../Common.h"
#include "../serialization/serializator.h"

class Scene;
class WidgetNode;
class SocketNode;
class NodeGraphics;

using namespace std;

class Node : public Serializable{
public:
    Node(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {COLOR_1}, vector<SOCKETTYPES> output_size = {COLOR_2});

    pair<int, int> getSocketPos(int index, POSITION position);
    void setPos(int x, int y);
    pair<int, int> Pos();
    QPointF pos() const;
    void updateConnectedEdges() const;
    string str();
    void remove();
    void setEditingFlag(bool flag);
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, int> hashmap) override;


    vector<SocketNode*> inputs;
    vector<SocketNode*> outputs;


    Scene* scene;

    NodeGraphics* grNode;
    WidgetNode *content;

    int socket_spacing = 22;
    int x_pos, y_pos;



};

#endif //NODE_H
