//
// Created by Ruhaan on 12/08/25.
//

#ifndef NODESCENE_H
#define NODESCENE_H

#include <vector>

class CanvasScene;
class Node;
class NodeEdges;

class Scene {
public:
    Scene();

    std::vector<Node*> nodes;
    std::vector<NodeEdges*> edges;

    int scene_width = 64000;
    int scene_height = 64000;

    void addNode(Node* node);
    void addEdge(NodeEdges* edge);
    void removeNode(Node* node);
    void removeEdge(NodeEdges* edge);

    CanvasScene *grScene;

    void initUI();
};

#endif //NODESCENE_H
