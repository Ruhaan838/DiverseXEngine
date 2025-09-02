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
#include "../../ui/canvas/editorWindow.h"
#include "../codegeneration/codeTemplateManager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QSet>


Scene::Scene() : Serializable() {

    initUI();
}

void Scene::initUI() {
    grScene = new CanvasScene(this);
    grScene->setScene(scene_width, scene_height);
}

void Scene::addNode(Node* node) {
    if (std::find(nodes.begin(), nodes.end(), node) == nodes.end()) {
        nodes.emplace_back(node);
    }
}

void Scene::addEdge(EdgesNode* edge) {
    edges.emplace_back(edge);
}

void Scene::removeNode(Node* node) {
    auto it = std::find(nodes.begin(), nodes.end(), node);
    if (it != nodes.end()) {
        clearVarName(*it);
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
    vector<Node*> temp_nodes;
    for (auto node_data : data.value("nodes").toArray()) {
        auto obj = node_data.toObject();
        QString type = obj.value("node_type").toString();
        Node* node = Node::createNode(type, this);
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

void Scene::setEditorWindow(EditorWindow* editorWindow) {
    this->editorWindow = editorWindow;
}

void Scene::updateEditorCode() {
    if (!editorWindow) return;

    varNameMap.clear();
    inCounter = outCounter = fnCounter = 1;

    static bool templatesLoaded = false;
    if (!templatesLoaded) {
        templatesLoaded = CodeTemplateManager::getInstance().loadTemplatesFromFile(":/codebase/python.json");
        if (!templatesLoaded) {
            QString directPath = QDir::currentPath() + "/codebase/python.json";
            templatesLoaded = CodeTemplateManager::getInstance().loadTemplatesFromFile(directPath);
            qDebug() << "Trying to load from:" << directPath;

            if (!templatesLoaded) {

                QString absolutePath = "/Users/ruhaan/Documents/DiverseXEngine/codebase/python.json";
                templatesLoaded = CodeTemplateManager::getInstance().loadTemplatesFromFile(absolutePath);
                qDebug() << "Trying to load from absolute path:" << absolutePath;

                if (!templatesLoaded) {
                    qWarning() << "Failed to load code templates from JSON file";
                    return;
                }
            }
        }
        qDebug() << "Templates loaded successfully";
    }

    QString importsCode = "";
    QString functionsCode = "";
    QString mainCode = "";

    QSet<QString> addedFunctionDefs;

    for (Node* node : nodes) {
        if (auto* inputNode = dynamic_cast<InputNode*>(node)) {
            QString inputTemplate = CodeTemplateManager::getInstance().getInputTemplate();
            if (!inputTemplate.isEmpty()) {
                QString varName = getOrCreateVarName(inputNode);
                QString inputCode = inputTemplate;
                mainCode += inputCode.replace("{}", varName) + "\n";
            }
        }
    }

    auto functionNameForNode = [](FunctionNode* fn)->QString {
        if (dynamic_cast<AddNode*>(fn)) return "add";
        if (dynamic_cast<SubNode*>(fn)) return "sub";
        if (dynamic_cast<MulNode*>(fn)) return "mul";
        if (dynamic_cast<DivNode*>(fn)) return "div";
        return "";
    };


    QSet<FunctionNode*> visitedFns;
    QStringList functionCallLines;

    std::function<void(FunctionNode*)> genFn = [&](FunctionNode* fn){
        if (!fn || visitedFns.contains(fn)) return;

        for (size_t i=0;i<fn->inputs.size();++i) {
            auto *s = fn->inputs[i];
            if (s && s->hasEdge() && s->edge && s->edge->startSocket) {
                if (auto *prevFn = dynamic_cast<FunctionNode*>(s->edge->startSocket->node)) {
                    genFn(prevFn);
                }
            }
        }

        QString fname = functionNameForNode(fn);
        if (fname.isEmpty()) return;
        QString arg1 = "0"; QString arg2 = "0";
        if (fn->inputs.size() >= 1) {
            auto *s = fn->inputs[0];
            if (s && s->hasEdge() && s->edge && s->edge->startSocket) {
                Node* nn = s->edge->startSocket->node;
                if (auto *inN = dynamic_cast<InputNode*>(nn)) arg1 = getOrCreateVarName(inN);
                else if (auto *fnN = dynamic_cast<FunctionNode*>(nn)) arg1 = getOrCreateVarName(fnN);
            }
        }
        if (fn->inputs.size() >= 2) {
            auto *s = fn->inputs[1];
            if (s && s->hasEdge() && s->edge && s->edge->startSocket) {
                Node* nn = s->edge->startSocket->node;
                if (auto *inN = dynamic_cast<InputNode*>(nn)) arg2 = getOrCreateVarName(inN);
                else if (auto *fnN = dynamic_cast<FunctionNode*>(nn)) arg2 = getOrCreateVarName(fnN);
            }
        }
        QString resultVar = getOrCreateVarName(fn);
        QString callTemplate = CodeTemplateManager::getInstance().getFunctionCallTemplate(fname);
        QString callLine;
        if (!callTemplate.isEmpty()) {
            callLine = callTemplate;
            int idx = callLine.indexOf("{}"); if (idx>=0) callLine.replace(idx,2,resultVar);
            idx = callLine.indexOf("{}"); if (idx>=0) callLine.replace(idx,2,arg1);
            idx = callLine.indexOf("{}"); if (idx>=0) callLine.replace(idx,2,arg2);
        } else {
            callLine = resultVar + " = " + fname + "(" + arg1 + ", " + arg2 + ")";
        }
        functionCallLines.append(callLine);
        visitedFns.insert(fn);
    };

    for (Node* node : nodes) {
        if (auto *out = dynamic_cast<OutputNode*>(node)) {
            if (auto *prev = out->getPrevNode(0)) {
                if (auto *fnPrev = dynamic_cast<FunctionNode*>(prev)) {
                    genFn(fnPrev);
                }
            }
        }
    }

    for (const QString &l : functionCallLines) {
        if (!mainCode.contains(l + "\n")) mainCode += l + "\n";
    }

    for (auto *fn : visitedFns) {
        QString fname = functionNameForNode(fn);
        if (!fname.isEmpty() && !addedFunctionDefs.contains(fname)) {
            QString defTemplate = CodeTemplateManager::getInstance().getFunctionTemplate(fname);
            if (!defTemplate.isEmpty()) {
                functionsCode += defTemplate + "\n\n";
                addedFunctionDefs.insert(fname);
            }
        }
    }

    for (Node* node : nodes) {
        if (auto* outputNode = dynamic_cast<OutputNode*>(node)) {
            if (auto* prevNode = outputNode->getPrevNode(0)) {

                QString outputTemplate = CodeTemplateManager::getInstance().getOutputTemplate();

                if (outputTemplate.isEmpty()) continue;

                QString targetVar;
                if (auto *fnPrev = dynamic_cast<FunctionNode*>(prevNode)) targetVar = getOrCreateVarName(fnPrev);
                else if (auto *inPrev = dynamic_cast<InputNode*>(prevNode)) targetVar = getOrCreateVarName(inPrev);

                if (!targetVar.isEmpty()) {
                    QString outLine = outputTemplate;
                    int idx = outLine.indexOf("{}"); if (idx>=0) outLine.replace(idx,2,targetVar);
                    if (!mainCode.contains(outLine + "\n")) mainCode += outLine + "\n";
                }

            }
        }
    }

    if (functionsCode.endsWith("\n\n")) functionsCode.chop(2);

    editorWindow->setImportsText(importsCode);
    editorWindow->setFunctionsText(functionsCode);
    editorWindow->setMainText(mainCode);
}

QString Scene::getOrCreateVarName(Node* node) {

    if (!node) return "";

    long long key = reinterpret_cast<long long>(node);
    auto it = varNameMap.find(key);
    if (it != varNameMap.end()) return it->second;
    QString name;

    if (dynamic_cast<InputNode*>(node)) name = "in" + QString::number(inCounter++);
    else if (dynamic_cast<OutputNode*>(node)) name = "out" + QString::number(outCounter++);
    else if (dynamic_cast<FunctionNode*>(node)) name = "n" + QString::number(fnCounter++);
    else name = "v" + QString::number(varNameMap.size()+1);

    varNameMap[key] = name;

    return name;
}

void Scene::clearVarName(Node* node) {
    if (!node) return;
    long long key = reinterpret_cast<long long>(node);
    varNameMap.erase(key);
}
