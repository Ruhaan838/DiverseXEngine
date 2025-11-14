//
// Created by Ruhaan on 12/08/25.
//

#include "nodescene.h"
#include "../nodes/node.h"
#include "../nodes/edge.h"
#include "../serialization/serializator.h"
#include "../../ui/canvas/canvasScene.h"
#include "../noderegistry/inout/inoutNode.h"
#include "../noderegistry/function/functionNode.h"
#include "../noderegistry/function/conditionNode.h"
#include "../nodes/socket.h"
#include "../../ui/canvas/editorWindow.h"
#include "../codegeneration/codeTemplateManager.h"
#include "../../ui/graphics/nodeGraphics.h"
#include "../noderegistry/canvasNode.h"
#include "../noderegistry/matrix/matrixNode.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QSet>
#include <QTimer>

#include "../noderegistry/arithmetic/mathNode.h"
#include "../noderegistry/permutation/Permutation.h"
#include "../noderegistry/bitOps/BitOp.h"
#include "../noderegistry/inout/printNode.h"


Scene::Scene() : Serializable() {

    initUI();
}

void Scene::initUI() {
    grScene = new CanvasScene(this);
    grScene->setScene(scene_width, scene_height);
}

void Scene::setPendingNodePos(const QPointF &pos) {
    pending_node_pos = pos;
    has_pending_node_pos = true;
}

bool Scene::takePendingNodePos(QPointF &out) {
    if (!has_pending_node_pos) return false;
    out = pending_node_pos;
    has_pending_node_pos = false;
    pending_node_pos = QPointF();
    return true;
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
        auto *n = temp_nodes[idx++];
        if (n) n->show();
        temp_nodes[idx-1]->deserialize(node_data.toObject(), hashmap);
    }

    // After all nodes are created and basic deserialized, embed children for Canvas nodes
    for (Node* n : nodes) {
        auto *canvas = dynamic_cast<CanvasNode*>(n);
        if (!canvas) continue;
        if (!canvas->grNode) canvas->show();

        bool anyAdded = false;
        for (const auto &pair : canvas->pending_children) {
            uintptr_t childId = pair.first;
            QPointF localPos = pair.second;
            Node* child = nullptr;
            for (Node* candidate : nodes) {
                if (candidate && candidate->id == childId) { child = candidate; break; }
            }
            if (!child || child == canvas) continue;

            // Make child visible if not already
            if (!child->grNode) child->show();

            // Use addNode to handle parenting and socket parenting, then set exact saved local position
            canvas->addNode(child);
            if (child->grNode) {
                child->grNode->setPos(localPos);
                QTimer::singleShot(0, [child]() { if (child) child->refreshSocketsAndEdges(); });
            }
            anyAdded = true;
        }
        canvas->pending_children.clear();
        if (anyAdded) canvas->updateSizeToFitChildren();
    }

    // create edges
    for (auto edge_data : data.value("edges").toArray()) {
        auto *edge = new EdgesNode(this);
        edge->deserialize(edge_data.toObject(), hashmap);
        addEdge(edge);
    }

    // Final stabilization: refresh all nodes/sockets/edges so positions are correct after load
    for (Node* n : nodes) if (n) n->refreshSocketsAndEdges();
    for (auto *e : edges) if (e) e->updatePos();
    if (grScene) grScene->update();
    QTimer::singleShot(0, [this]() {
        for (Node* n : nodes) if (n) n->refreshSocketsAndEdges();
        for (auto *e : edges) if (e) e->updatePos();
        if (grScene) grScene->update();
    });
    QTimer::singleShot(30, [this]() {
        for (Node* n : nodes) if (n) n->refreshSocketsAndEdges();
        for (auto *e : edges) if (e) e->updatePos();
        if (grScene) grScene->update();
    });

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
            bool blocked = false;
            // Gate by any disabled Canvas containing this node
            for (Node* c : nodes) {
                auto *canvas = dynamic_cast<CanvasNode*>(c);
                if (!canvas) continue;
                if (!canvas->isEnabled()) {
                    if (std::find(canvas->inner_nodes.begin(), canvas->inner_nodes.end(), n) != canvas->inner_nodes.end()) {
                        blocked = true;
                        break;
                    }
                }
            }
            if (!blocked) n->execute();
        }
    }
    // Gate outputs too
    for (Node* node : nodes) {
        bool blocked = false;
        for (Node* c : nodes) {
            auto *canvas = dynamic_cast<CanvasNode*>(c);
            if (!canvas) continue;
            if (!canvas->isEnabled()) {
                if (std::find(canvas->inner_nodes.begin(), canvas->inner_nodes.end(), node) != canvas->inner_nodes.end()) {
                    blocked = true; break;
                }
            }
        }
        if (!blocked) {
            if (auto outputNode = dynamic_cast<OutputNode*>(node)) {
                outputNode->execute();
            }
            if (auto boolOut = dynamic_cast<BoolOutputNode*>(node)) {
                boolOut->execute();
            }
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
    QSet<QString> importSet;

    // Helper: check if a node is inside any Canvas
    auto isInsideAnyCanvas = [this](Node* n)->bool {
        if (!n) return false;
        for (Node* c : nodes) {
            if (auto *canvas = dynamic_cast<CanvasNode*>(c)) {
                if (std::find(canvas->inner_nodes.begin(), canvas->inner_nodes.end(), n) != canvas->inner_nodes.end())
                    return true;
            }
        }
        return false;
    };

    // Emit input stubs to mainCode (unchanged)
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
        //arithmatic
        if (dynamic_cast<AddNode*>(fn)) return "add";
        if (dynamic_cast<SubNode*>(fn)) return "sub";
        if (dynamic_cast<MulNode*>(fn)) return "mul";
        if (dynamic_cast<DivNode*>(fn)) return "div";

        //math
        if (dynamic_cast<PowNode*>(fn)) return  "pow";
        if (dynamic_cast<ModNode*>(fn)) return "mod";
        if (dynamic_cast<SqrtNode*>(fn)) return "sqrt";
        if (dynamic_cast<SinNode*>(fn)) return "sin";
        if (dynamic_cast<CosNode*>(fn)) return "cos";
        if (dynamic_cast<TanNode*>(fn)) return "tan";

        //permutations
        if (dynamic_cast<GCDNode*>(fn)) return "gcd";
        if (dynamic_cast<FactorialNode*>(fn)) return "factorial";

        //bitops
        if (dynamic_cast<AndNode*>(fn)) return "bit_and";
        if (dynamic_cast<OrNode*>(fn)) return "bit_or";
        if (dynamic_cast<NotNode*>(fn)) return "bit_not";
        if (dynamic_cast<XorNode*>(fn)) return "bit_xor";

        //condition
        if (dynamic_cast<ConditionNode*>(fn)) return "condition";

        // print-if
        if (dynamic_cast<PrintIfNode*>(fn)) return "print_if";

        // if (dynamic_cast<*>(fn)) return "";

        return "";
    };


    QSet<FunctionNode*> visitedFns;
    QStringList functionCallLines;

    // Helper to generate function calls into any destination list with a local visited set
    std::function<void(FunctionNode*, QStringList&, QSet<FunctionNode*>&)> genFnInto = [&](FunctionNode* fn, QStringList& dest, QSet<FunctionNode*>& visited){
        if (!fn || visited.contains(fn)) return;

        // Recurse into inputs first
        for (auto *s : fn->inputs) {
            if (!s) continue;
            if (s->socket_type == "addsocket") continue;
            if (s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
                if (auto *prevFn = dynamic_cast<FunctionNode*>(s->getFirstEdge()->startSocket->node)) {
                    genFnInto(prevFn, dest, visited);
                }
            }
        }

        QString fname = functionNameForNode(fn);
        if (fname.isEmpty()) { visited.insert(fn); return; }

        // Inline print_if
        if (fname == "print_if") {
            QString condArg = "0";
            if (!fn->inputs.empty()) {
                auto *s0 = fn->inputs[0];
                if (s0 && s0->hasEdge() && s0->getFirstEdge() && s0->getFirstEdge()->startSocket) {
                    Node* nn = s0->getFirstEdge()->startSocket->node;
                    if (auto *inN = dynamic_cast<InputNode*>(nn)) condArg = getOrCreateVarName(inN);
                    else if (auto *fnN = dynamic_cast<FunctionNode*>(nn)) condArg = getOrCreateVarName(fnN);
                }
            }
            QString msg;
            if (auto *pnode = dynamic_cast<PrintIfNode*>(fn)) {
                msg = pnode->getMessage();
                msg.replace("\\", "\\\\");
                msg.replace("\"", "\\\"");
                msg.replace("\n", "\\n");
            }
            dest.append(QString("if bool(%1): print(\"%2\")").arg(condArg, msg));
            visited.insert(fn);
            return;
        }

        // Build args
        QStringList args;
        for (auto *s : fn->inputs) {
            if (!s) continue;
            if (s->socket_type == "addsocket") continue;
            QString arg = "0";
            if (s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
                Node* nn = s->getFirstEdge()->startSocket->node;
                if (auto *inN = dynamic_cast<InputNode*>(nn)) arg = getOrCreateVarName(inN);
                else if (auto *fnN = dynamic_cast<FunctionNode*>(nn)) arg = getOrCreateVarName(fnN);
            }
            args << arg;
        }
        while (!args.isEmpty() && args.last() == "0") args.removeLast();

        QString resultVar = getOrCreateVarName(fn);
        QString callTemplate = CodeTemplateManager::getInstance().getFunctionCallTemplate(fname);
        if (!callTemplate.isEmpty() && args.size() <= 2) {
            QString callLine = callTemplate;
            int idx = callLine.indexOf("{}"); if (idx>=0) callLine.replace(idx,2,resultVar);
            idx = callLine.indexOf("{}"); if (idx>=0) callLine.replace(idx,2,args.size() > 0 ? args[0] : QString("0"));
            idx = callLine.indexOf("{}"); if (idx>=0) callLine.replace(idx,2,args.size() > 1 ? args[1] : QString("0"));
            if (fname == "condition") {
                int opIndex = 0;
                if (auto *cnode = dynamic_cast<ConditionNode*>(fn)) {
                    if (cnode->bool_list) opIndex = cnode->bool_list->currentIndex();
                }
                callLine.replace("OP_INDEX", QString::number(opIndex));
            }
            dest.append(callLine);
        } else {
            if (args.empty()) dest.append(resultVar + " = " + fname + "()");
            else if (args.size() == 1) dest.append(resultVar + " = " + fname + "(" + args[0] + ")");
            else if (args.size() == 2) dest.append(resultVar + " = " + fname + "(" + args[0] + ", " + args[1] + ")");
            else {
                QString nested = args[0];
                nested = QString("%1(%2, %3)").arg(fname, args[0], args[1]);
                for (int i = 2; i < args.size(); ++i) nested = QString("%1(%2, %3)").arg(fname, nested, args[i]);
                dest.append(QString("%1 = %2").arg(resultVar, nested));
            }
        }

        // Track imports/defs needed
        QString imp = CodeTemplateManager::getInstance().getFunctionImport(fname);
        if (!imp.isEmpty()) importSet.insert(imp);
        QString defTemplate = CodeTemplateManager::getInstance().getFunctionTemplate(fname);
        if (!defTemplate.isEmpty() && !addedFunctionDefs.contains(fname)) {
            functionsCode += defTemplate + "\n\n";
            addedFunctionDefs.insert(fname);
        }

        visited.insert(fn);
    };

    // Populate functionCallLines from outputs that are NOT inside any Canvas
    {
        QSet<FunctionNode*> visitedGlobalFns;
        for (Node* node : nodes) {
            if (auto *out = dynamic_cast<OutputNode*>(node)) {
                if (isInsideAnyCanvas(out)) continue; // handled under Canvas blocks
                if (auto *prev = out->getPrevNode(0)) {
                    if (auto *fnPrev = dynamic_cast<FunctionNode*>(prev)) {
                        genFnInto(fnPrev, functionCallLines, visitedGlobalFns);
                    }
                }
            }
            if (auto *bout = dynamic_cast<BoolOutputNode*>(node)) {
                if (isInsideAnyCanvas(bout)) continue; // handled under Canvas blocks
                if (auto *prev = bout->getPrevNode(0)) {
                    if (auto *fnPrev = dynamic_cast<FunctionNode*>(prev)) {
                        genFnInto(fnPrev, functionCallLines, visitedGlobalFns);
                    }
                }
            }
        }
    }

    // Append global function calls
    for (const QString &l : functionCallLines) {
        if (!mainCode.contains(l + "\n")) mainCode += l + "\n";
    }

    // Emit Canvas-scoped code blocks
    auto indentLines = [](const QStringList& lines){
        QString out = "";
        for (const QString &l : lines) out += "  " + l + "\n";
        return out;
    };

    for (Node* n : nodes) {
        auto *canvas = dynamic_cast<CanvasNode*>(n);
        if (!canvas) continue;

        // Determine condition source (first input)
        QString condVar = "";
        QStringList condLines;
        bool invertCond = false; // new: whether we should negate the condition
        if (!canvas->inputs.empty()) {
            auto *s = canvas->inputs[0];
            if (s && s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
                // Capture the upstream start socket and node
                SocketNode* startSock = s->getFirstEdge()->startSocket;
                Node* prev = startSock->node;
                if (auto *inPrev = dynamic_cast<InputNode*>(prev)) condVar = getOrCreateVarName(inPrev);
                else if (auto *fnPrev = dynamic_cast<FunctionNode*>(prev)) {
                    condVar = getOrCreateVarName(fnPrev);
                    QSet<FunctionNode*> visitedCond;
                    genFnInto(fnPrev, condLines, visitedCond); // ensure condition is computed before 'if'

                    // If the upstream is a ConditionNode and the connected output socket is "False",
                    // invert the condition in generated code so the Canvas guards the false branch.
                    if (dynamic_cast<ConditionNode*>(fnPrev)) {
                        // Prefer explicit socket_type label, fallback to index 1
                        if (startSock && (startSock->socket_type == "False" || startSock->index == 1)) {
                            invertCond = true;
                        }
                    }
                }
            }
        }
        if (condVar.isEmpty()) {
            // No condition connected: per spec, do not emit inner code
            continue;
        }

        // Collect inner function calls driven by inner OutputNodes
        QStringList innerFnLines;
        QList<OutputNode*> innerOutputs;
        QList<BoolOutputNode*> innerBoolOutputs;
        for (Node* inner : canvas->inner_nodes) {
            if (auto *out = dynamic_cast<OutputNode*>(inner)) innerOutputs.append(out);
            if (auto *bout = dynamic_cast<BoolOutputNode*>(inner)) innerBoolOutputs.append(bout);
        }
        {
            QSet<FunctionNode*> visitedInner;
            for (auto *out : innerOutputs) {
                if (auto *prev = out->getPrevNode(0)) {
                    if (auto *fnPrev = dynamic_cast<FunctionNode*>(prev)) {
                        genFnInto(fnPrev, innerFnLines, visitedInner);
                    }
                }
            }
            for (auto *bout : innerBoolOutputs) {
                if (auto *prev = bout->getPrevNode(0)) {
                    if (auto *fnPrev = dynamic_cast<FunctionNode*>(prev)) {
                        genFnInto(fnPrev, innerFnLines, visitedInner);
                    }
                }
            }
        }

        // Start block: condition pre-lines then if
        for (const QString &l : condLines) if (!mainCode.contains(l + "\n")) mainCode += l + "\n";
        // Use inversion when connected to Condition.False output
        if (invertCond) mainCode += QString("if not bool(%1):\n").arg(condVar);
        else mainCode += QString("if bool(%1):\n").arg(condVar);
        mainCode += indentLines(innerFnLines);

        // Emit inner outputs inside block
        QString outputTemplate = CodeTemplateManager::getInstance().getOutputTemplate();
        if (!outputTemplate.isEmpty()) {
            for (auto *out : innerOutputs) {
                if (auto* prevNode = out->getPrevNode(0)) {
                    QString targetVar;
                    if (auto *fnPrev = dynamic_cast<FunctionNode*>(prevNode)) targetVar = getOrCreateVarName(fnPrev);
                    else if (auto *inPrev = dynamic_cast<InputNode*>(prevNode)) targetVar = getOrCreateVarName(inPrev);
                    if (!targetVar.isEmpty()) {
                        QString outLine = outputTemplate;
                        int idx = outLine.indexOf("{}"); if (idx>=0) outLine.replace(idx,2,targetVar);
                        mainCode += QString("  %1\n").arg(outLine);
                    }
                }
            }
            for (auto *bout : innerBoolOutputs) {
                if (auto* prevNode = bout->getPrevNode(0)) {
                    QString targetVar;
                    if (auto *fnPrev = dynamic_cast<FunctionNode*>(prevNode)) targetVar = getOrCreateVarName(fnPrev);
                    else if (auto *inPrev = dynamic_cast<InputNode*>(prevNode)) targetVar = getOrCreateVarName(inPrev);
                    if (!targetVar.isEmpty()) {
                        QString outLine = outputTemplate;
                        int idx = outLine.indexOf("{}"); if (idx>=0) outLine.replace(idx,2,QString("bool(%1)").arg(targetVar));
                        mainCode += QString("  %1\n").arg(outLine);
                    }
                }
            }
        }
    }

    // Now emit global outputs (already skipped those inside Canvas)
    for (Node* node : nodes) {
        if (auto* outputNode = dynamic_cast<OutputNode*>(node)) {
            if (isInsideAnyCanvas(outputNode)) continue;
            if (auto* prevNode = outputNode->getPrevNode(0)) {

                QString outputTemplate = CodeTemplateManager::getInstance().getOutputTemplate();
                if (outputTemplate.isEmpty()) continue;
                QString targetVar;
                if (auto *fnPrev = dynamic_cast<FunctionNode*>(prevNode)) targetVar = getOrCreateVarName(fnPrev);
                else if (auto *inPrev = dynamic_cast<InputNode*>(prevNode)) targetVar = getOrCreateVarName(inPrev);
                if (!targetVar.isEmpty()) {
                    QString outLine = outputTemplate;
                    int idx = outLine.indexOf("{}"); if (idx>=0) outLine.replace(idx,2,targetVar);
                    mainCode += outLine + "\n"; // unconditional append, no comment
                }
            }
        }
        if (auto* boolOut = dynamic_cast<BoolOutputNode*>(node)) {
            if (isInsideAnyCanvas(boolOut)) continue;
            if (auto* prevNode = boolOut->getPrevNode(0)) {
                QString outputTemplate = CodeTemplateManager::getInstance().getOutputTemplate();
                if (outputTemplate.isEmpty()) continue;
                QString targetVar;
                if (auto *fnPrev = dynamic_cast<FunctionNode*>(prevNode)) targetVar = getOrCreateVarName(fnPrev);
                else if (auto *inPrev = dynamic_cast<InputNode*>(prevNode)) targetVar = getOrCreateVarName(inPrev);
                if (!targetVar.isEmpty()) {
                    QString outLine = outputTemplate;
                    int idx = outLine.indexOf("{}"); if (idx>=0) outLine.replace(idx,2,QString("bool(%1)").arg(targetVar));
                    mainCode += outLine + "\n"; // unconditional append, no comment
                }
            }
        }
    }

    // Emit imports from collected importSet
    if (!importSet.isEmpty()) {
        for (const QString &im : importSet) {
            if (!importsCode.contains(im + "\n")) importsCode += im + "\n";
        }
        importsCode += "\n";
    }

    if (functionsCode.endsWith("\n\n")) functionsCode.chop(2);

    editorWindow->setImportsText(importsCode);
    editorWindow->setFunctionsText(functionsCode);
    editorWindow->setMainText(mainCode);

    for (Node* node : nodes) {
        if (!node) continue;
        if (auto *inNode = dynamic_cast<InputNode*>(node)) {
            QString v = getOrCreateVarName(inNode);
            if (inNode->grNode) inNode->grNode->setTitle(QString("Input - %1").arg(v).toStdString());
            inNode->var_code = QString("Input - %1").arg(v);
        }
    }

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

void Scene::resetNodes() {
    for (Node* n : nodes) {
        if (!n) continue;
        if (auto *in = dynamic_cast<InputNode*>(n)) {
            in->vals = 0; if (in->lineEdit) in->lineEdit->clear();
        } else if (auto *mat = dynamic_cast<MatrixInputNode*>(n)) {
            mat->resetMatrixValues();
        } else if (auto *out = dynamic_cast<OutputNode*>(n)) {
            out->vals = 0; if (out->textEdit) out->textEdit->clear();
        } else if (auto *bout = dynamic_cast<BoolOutputNode*>(n)) {
            bout->bval = false; if (bout->textEdit) bout->textEdit->clear();
        }
    }

    for (Node* n : nodes) {
        if (auto *in = dynamic_cast<InputNode*>(n)) {
            QString existing = getOrCreateVarName(in); // fetch without creating new
            if (in->grNode) {
                in->grNode->setTitle(std::string("Input - ") + existing.toStdString());
            }
            in->var_code = QString("Input - %1").arg(existing);
        }
    }
    if (grScene) grScene->update();
}
