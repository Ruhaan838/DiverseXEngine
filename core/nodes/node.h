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
    Node(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {}, vector<SOCKETTYPES> output_size = {});

    void initDefault(vector<SOCKETTYPES> input, vector<SOCKETTYPES> output);

    pair<int, int> getSocketPos(int index, POSITION position);
    void setPos(int x, int y);
    pair<int, int> Pos();
    QPointF pos() const;
    void updateConnectedEdges() const;
    string str();
    void remove();
    void setEditingFlag(bool flag);
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) override;


    vector<SocketNode*> inputs;
    vector<SocketNode*> outputs;


    Scene* scene;

    NodeGraphics* grNode;
    WidgetNode *content;

    int socket_spacing = 22;



};

#endif //NODE_H
