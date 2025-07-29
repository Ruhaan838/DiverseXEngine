//
// Created by Ruhaan on 29/07/25.
//

#include "NodeScene.h"
#include "../../Canvas/Scene/CanvasScene.h"
#include "../Node/Node.h"

Scene::Scene() {
    initUI();
}

void Scene::initUI() {
    grScene = new CanvasScene(this);
    grScene->setScene(scene_width, scene_height);
}

void Scene::addNode(Node* node) {
    nodes.emplace_back(node);
}

void Scene::addEdge(Node* edge) {
    nodes.emplace_back(edge);
}

// TODO: check is this method works?
void Scene::removeNode(Node* node) {
    nodes.erase(std::find(nodes.begin(), nodes.end(), node));
}

// TODO: check is this method works?
void Scene::removeEdge(Node* edge) {
    nodes.erase(std::find(nodes.begin(), nodes.end(), edge));
}

