//
// Created by Ruhaan on 12/08/25.
//

#ifndef NODESCENE_H
#define NODESCENE_H

#include <qjsonobject.h>
#include <vector>

#include "../serialization/serializator.h"

class CanvasScene;
class Node;
class EdgesNode;
class Serializable;

class Scene : public Serializable{
public:
    Scene();

    std::vector<Node*> nodes;
    std::vector<EdgesNode*> edges;

    int scene_width = 64000;
    int scene_height = 64000;

    void addNode(Node* node);
    void addEdge(EdgesNode* edge);
    void removeNode(Node* node);
    void removeEdge(EdgesNode* edge);
    void saveToFile(const std::string &filename);
    void clear();



    void loadFromFile(const std::string &filename);

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) override;


    CanvasScene *grScene{};

    void initUI();
    void executeGraph();
};

#endif //NODESCENE_H
