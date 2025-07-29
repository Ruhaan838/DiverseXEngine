//
// Created by Ruhaan on 29/07/25.
//

#ifndef NODESCENE_H
#define NODESCENE_H


#include <vector>

class CanvasScene;
class Node;

class Scene {
public:
    Scene();

    std::vector<Node*> nodes;
    std::vector<Node*> edges;

    int scene_width = 64000;
    int scene_height = 64000;

    void addNode(Node* node);
    void addEdge(Node* edge);
    void removeNode(Node* node);
    void removeEdge(Node* edge);

    CanvasScene *grScene;

    void initUI();
};

#endif //NODESCENE_H
