//
// Created by Ruhaan on 29/07/25.
//

#include "NodeScene.h"
#include "../../Canvas/Scene/CanvasScene.h"

Scene::Scene() {
    initUI();
}

void Scene::initUI() {
    grScene = new CanvasScene(this);
    grScene->setScene(scene_width, scene_height);
}

//TODO: change the type
void Scene::addNode(int Node) {
    nodes.emplace_back(Node);
}

// TODO: change the type
void Scene::addEdge(int Edge) {
    nodes.emplace_back(Edge);
}

// TODO: change the type or chack is this method works?
void Scene::removeNode(int Node) {
    nodes.erase(std::find(nodes.begin(), nodes.end(), Node));
}

// TODO: change the type or chack is this method works?
void Scene::removeEdge(int Edge) {
    nodes.erase(std::find(nodes.begin(), nodes.end(), Edge));
}

