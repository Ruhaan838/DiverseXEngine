//
// Created by Ruhaan on 29/07/25.
//

#include "NodeScene.h"
#include "../../Canvas/Scene/CanvasScene.h"
#include "../Node/Node.h"
#include <QDebug>

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

void Scene::addEdge(NodeEdges* edge) {
    edges.emplace_back(edge);
}

void Scene::removeNode(Node* node) {
    nodes.erase(std::find(nodes.begin(), nodes.end(), node));
}

void Scene::removeEdge(NodeEdges* edge) {
    edges.erase(std::find(edges.begin(), edges.end(), edge));
}

