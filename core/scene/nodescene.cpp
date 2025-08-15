//
// Created by Ruhaan on 12/08/25.
//

#include "nodescene.h"
#include "../nodes/node.h"
#include "../serialization/serializator.h"
#include "../../ui/canvas/canvasScene.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>

#include "../nodes/edge.h"

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

void Scene::addEdge(NodeEdges* edge) {
    edges.emplace_back(edge);
}

void Scene::removeNode(Node* node) {
    nodes.erase(std::find(nodes.begin(), nodes.end(), node));
}

void Scene::removeEdge(NodeEdges* edge) {
    edges.erase(std::find(edges.begin(), edges.end(), edge));
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


bool Scene::deserialize(const QJsonObject & data, unordered_map<string, int> hashmap) {
    return false;
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
    // Documents/DiverseXEngine_Saves
    QString docsPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir dir(docsPath + "/DiverseXEngine_Saves");

    QString fullPath = dir.filePath(QString::fromStdString(filename));

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << fullPath << "Error:" << file.errorString();
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format in file:" << fullPath;
        return;
    }

    QJsonObject json = doc.object();
    if (json.contains("id"))        id = json["id"].toString().toULongLong();
    if (json.contains("scene_width"))  scene_width = json["scene_width"].toInt();
    if (json.contains("scene_height")) scene_height = json["scene_height"].toInt();

    qDebug() << "File loaded from location" << fullPath;
}