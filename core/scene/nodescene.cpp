//
// Created by Ruhaan on 12/08/25.
//

#include "nodescene.h"
#include "../nodes/node.h"
#include "../../ui/canvas/canvasScene.h"

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