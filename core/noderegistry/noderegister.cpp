//
// Created by Ruhaan on 18/08/25.
//

#include "noderegister.h"

#include "../nodes/node.h"
#include "../scene/nodescene.h"

void registerAllNodeTypes();

NodeRegistery::NodeRegistery(Scene* parent) : scene(parent), functions(nullptr), arithmetic(nullptr), math(nullptr), permutation(nullptr), bitoperations(nullptr), inputs(nullptr), outputs(nullptr), containers(nullptr), matrix(nullptr) {
    setHeaderHidden(true);
    registerAllNodeTypes();
    buildUI();
    setupDoubleClickHandler();
}

QHash<QString, NodeInfo>& NodeRegistery::getRegistry() {
    static QHash<QString, NodeInfo> registry;
    return registry;
}

void NodeRegistery::registerNode(const QString& name, NodeCategory category,
                                const std::function<Node*(Scene*)>& creator,
                                int width, int height) {
    NodeInfo info;
    info.name = name;
    info.category = category;
    info.creator = creator;
    info.width = width;
    info.height = height;

    getRegistry()[name] = info;

    Node::registerType(name, creator);
}

QList<NodeInfo> NodeRegistery::getNodesByCategory(NodeCategory category) {
    QList<NodeInfo> result;
    for (auto it = getRegistry().begin(); it != getRegistry().end(); ++it) {
        if (it.value().category == category) {
            result.append(it.value());
        }
    }
    return result;
}

Node* NodeRegistery::createNode(const QString& name, Scene* scene) {
    auto& registry = getRegistry();
    if (registry.contains(name)) {
        const NodeInfo& info = registry[name];
        Node* node = info.creator(scene);
        if (node) {
            node->setHeightWidth(info.height, info.width);
            // Special-case placement for Input Matrix
            if (name == "Input Matrix") {
                node->setPosition(LEFT_BOTTOM, RIGHT_TOP);
            } else {
                switch (info.category) {
                    case NodeCategory::INPUT:
                        node->setPosition(LEFT_TOP, RIGHT_TOP);
                        break;
                    case NodeCategory::CONTAINER:
                        node->setPosition(LEFT_TOP, RIGHT_TOP);
                        break;
                    default:
                        node->setPosition(LEFT_TOP, RIGHT_BOTTOM);
                        break;
                }
            }
        }
        return node;
    }
    return nullptr;
}

void NodeRegistery::buildUI() {

    functions = new QTreeWidgetItem(this);
    functions->setText(0, "Functions");

    arithmetic = new QTreeWidgetItem(this);
    arithmetic->setText(0, "Arithmetic");

    math = new QTreeWidgetItem(this);
    math->setText(0, "Math");

    permutation = new QTreeWidgetItem(this);
    permutation->setText(0, "Permutation");

    bitoperations = new QTreeWidgetItem(this);
    bitoperations->setText(0, "Bit Operations");

    inputs = new QTreeWidgetItem(this);
    inputs->setText(0, "Inputs");

    outputs = new QTreeWidgetItem(this);
    outputs->setText(0, "Outputs");

    containers = new QTreeWidgetItem(this);
    containers->setText(0, "Containers");

    matrix = new QTreeWidgetItem(this);
    matrix->setText(0, "Matrix");


    auto& registry = getRegistry();
    for (auto it = registry.begin(); it != registry.end(); ++it) {
        const NodeInfo& info = it.value();
        QTreeWidgetItem* parent = nullptr;

        switch (info.category) {
            case NodeCategory::FUNCTION:
                parent = functions;
                break;
            case NodeCategory::ARITHMETIC:
                parent = arithmetic;
                break;
            case NodeCategory::MATH:
                parent = math;
                break;
            case NodeCategory::PERMUTATION:
                parent = permutation;
                break;
            case NodeCategory::BITOPERATIONS:
                parent = bitoperations;
                break;
            case NodeCategory::INPUT:
                parent = inputs;
                break;
            case NodeCategory::OUTPUT:
                parent = outputs;
                break;
            case NodeCategory::MATRIX:
                parent = matrix;
                break;
            case NodeCategory::CONTAINER:
                parent = containers;
                break;
        }

        if (parent) {
            auto* item = new QTreeWidgetItem(parent);
            item->setText(0, info.name);
        }
    }
}

void NodeRegistery::setupDoubleClickHandler() {
    connect(this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int column) {
        QString nodeName = item->text(0);

        if (nodeName == "Functions" || nodeName == "Inputs" ||
            nodeName == "Outputs" || nodeName == "Arithmetic" || nodeName == "Math" ||
            nodeName == "Permutation" || nodeName == "Bit Operations" || nodeName == "Matrix" || nodeName == "Containers") {
            return;
        }

        Node* node = createNode(nodeName, scene);
        if (node) {
             scene->addNode(node);
             node->show();
            QPointF pending;
            if (scene && scene->takePendingNodePos(pending)) {
                node->setPos(static_cast<int>(pending.x()), static_cast<int>(pending.y()));
            }
         }
     });
}

QHash<QString, Node*> NodeRegistery::getfunctionalNode() const {
    QHash<QString, Node*> nodes;
    QList<NodeInfo> combined;
    auto fn = getNodesByCategory(NodeCategory::FUNCTION);
    auto ar = getNodesByCategory(NodeCategory::ARITHMETIC);
    auto ma = getNodesByCategory(NodeCategory::MATH);

    combined.append(fn);
    combined.append(ar);
    combined.append(ma);

    for (const NodeInfo& info : combined) {
        Node* node = info.creator(scene);
        if (node) {
            node->setHeightWidth(info.height, info.width);
            node->setPosition(LEFT_TOP, RIGHT_BOTTOM);
        }
        nodes[info.name] = node;
    }

    return nodes;
}

vector<Node*> NodeRegistery::getInputNode() const {
    vector<Node*> result;
    auto inputNodes = getNodesByCategory(NodeCategory::INPUT);

    for (const NodeInfo& info : inputNodes) {
        Node* node = info.creator(scene);
        if (node) {
            node->setHeightWidth(info.height, info.width);
            node->setPosition(LEFT_TOP, RIGHT_TOP);
            result.push_back(node);
        }
    }

    return result;
}

vector<Node*> NodeRegistery::getOutputNode() const {
    vector<Node*> result;
    auto outputNodes = getNodesByCategory(NodeCategory::OUTPUT);

    for (const NodeInfo& info : outputNodes) {
        Node* node = info.creator(scene);
        if (node) {
            node->setHeightWidth(info.height, info.width);
            node->setPosition(LEFT_TOP, RIGHT_BOTTOM);
            result.push_back(node);
        }
    }

    return result;
}
