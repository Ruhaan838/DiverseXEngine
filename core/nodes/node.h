//
// Created by Ruhaan on 12/08/25.
//

#ifndef NODE_H
#define NODE_H

#include <qpoint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <QString>
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
    void initNode(string title, vector<SOCKETTYPES> in, vector<SOCKETTYPES> out);
    pair<int, int> getSocketPos(int index, POSITION position);
    void setPos(int x, int y);
    QPointF pos() const;
    void show();

    void updateConnectedEdges() const;

    string str();

    void remove();
    void setEditingFlag(bool flag);

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) override;

    void setHeightWidth(int h, int w);
    std::pair<int, int> getHeightAndWidth() const;

    vector<SocketNode*> inputs, outputs;

    void setContent(WidgetNode* content);
    void setPosition(POSITION in_pos, POSITION out_pos);

    Scene* scene;

    NodeGraphics* grNode = nullptr;
    WidgetNode *content = nullptr;

    int socket_spacing = 22;

    static void registerType(const QString& type, std::function<Node*(Scene*)> creator);
    static Node* createNode(const QString& type, Scene* scene);
private:
    static unordered_map<QString, function<Node*(Scene*)>>& registry();

    vector<SOCKETTYPES> in_socket_type;
    vector<SOCKETTYPES> out_socket_type;
    string title;
    int pending_w = -1, pending_h = -1;
    POSITION in_pos = LEFT_BOTTOM, out_pos = RIGHT_TOP;

};

#endif //NODE_H
