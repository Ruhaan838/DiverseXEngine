//
// Created by Ruhaan on 29/07/25.
//

#ifndef NODESCENE_H
#define NODESCENE_H
#include <vector>

class CanvasScene;

class Scene {
public:
    Scene();

    // TODO: change the type
    std::vector<int> nodes;
    std::vector<int> edges;

    int scene_width = 64000;
    int scene_height = 64000;

    // TODO: change the type
    void addNode(int Node);
    void addEdge(int Edge);
    void removeNode(int Node);
    void removeEdge(int Edge);

    CanvasScene *grScene{};

    void initUI();
};

#endif //NODESCENE_H
