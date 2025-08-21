//
// Created by Ruhaan on 12/08/25.
//

#include "nodescene.h"
#include "../nodes/node.h"
#include "../nodes/edge.h"
#include "../serialization/serializator.h"
#include "../../ui/canvas/canvasScene.h"
#include "../noderegistry/inoutNode.h"
#include "../noderegistry/functionNode.h"
#include "../nodes/socket.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>


Scene::Scene() : Serializable() {

    initUI();
}

void Scene::initUI() {
    grScene = new CanvasScene(this);
    grScene->setScene(scene_width, scene_height);
}

void Scene::addNode(Node* node) {
    nodes.emplace_back(node);
}

void Scene::addEdge(EdgesNode* edge) {
    edges.emplace_back(edge);
}

void Scene::removeNode(Node* node) {
    auto it = std::find(nodes.begin(), nodes.end(), node);
    if (it != nodes.end()) {
        nodes.erase(it);
    }
}

void Scene::removeEdge(EdgesNode* edge) {
    auto it = std::find(edges.begin(), edges.end(), edge);
    if (it != edges.end()) {
        edges.erase(it);
    }
}

void Scene::clear() {

    while (!nodes.empty()) {
        nodes.at(0)->remove();
    }
}


QJsonObject Scene::serialize() {
    QJsonArray nodesArray;
    for (auto n : nodes) {
        nodesArray.append(n->serialize());
    }

    QJsonArray edgesArray;
    for (auto e : edges) {
        edgesArray.append(e->serialize());
    }

    QJsonObject sceneObject{
            {"id", static_cast<int>(id)},
            {"scene_width", scene_width},
            {"scene_height", scene_height},
            {"nodes", nodesArray},
            {"edges", edgesArray}
    };

    return sceneObject;
}


bool Scene::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) {

    clear();
    hashmap = {};

    //create nodes
    std::vector<Node*> temp_nodes;
    for (auto node_data : data.value("nodes").toArray()) {
        auto *node = new Node(this);
        temp_nodes.push_back(node);
        addNode(node);
    }

    int idx = 0;
    for (auto node_data : data.value("nodes").toArray()) {
        temp_nodes[idx++]->deserialize(node_data.toObject(), hashmap);
    }

    // create edges
    for (auto edge_data : data.value("edges").toArray()) {
        auto *edge = new EdgesNode(this);
        edge->deserialize(edge_data.toObject(), hashmap);
        addEdge(edge);
        // edge->updatePos();
    }

    return true;
}

void Scene::saveToFile(const std::string &filename) {
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir dir(docsPath + "/DiverseXEngine_Saves");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString fullPath = dir.filePath(QString::fromStdString(filename));

    QJsonDocument doc(serialize());
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file:" << fullPath << "Error:" << file.errorString();
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    if (file.isOpen()) {
        qDebug() << "File saved at location" << fullPath;
    }
    file.close();
}

void Scene::loadFromFile(const std::string &filename) {
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir dir(docsPath + "/DiverseXEngine_Saves");

    QString fullPath = dir.filePath(QString::fromStdString(filename));

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << fullPath << "Error:" << file.errorString();
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse JSON:" << parseError.errorString();
        return;
    }

    QJsonObject jsonObj = doc.object();
    unordered_map<string, uintptr_t> dummy_hashmap;
    deserialize(jsonObj, dummy_hashmap);
}

void Scene::executeGraph() {
    for (Node* node : nodes) {
        if (auto n = dynamic_cast<FunctionNode*>(node)) {
            n->execute();
        }
    }
    for (Node* node : nodes) {
        if (auto outputNode = dynamic_cast<OutputNode*>(node)) {
            outputNode->execute();
        }
    }
}