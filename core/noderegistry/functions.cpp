//
// Created by Ruhaan on 18/08/25.
//

#include "noderegister.h"
#include "../nodes/node.h"
#include "inoutNode.h"
#include "functionNode.h"
// #include "../scene/nodescene.h"
#include "../widgets/widgets.h"

QHash<QString, Node*> NodeRegistery::getfunctionalNode() const {
    QHash<QString, Node*> nodes;
    nodes["Add"] = new AddNode(scene, "Add", {COLOR_1, COLOR_3}, {COLOR_2});
    nodes["Subtract"] = new SubNode(scene, "Subtract", {COLOR_1, COLOR_3}, {COLOR_2});
    nodes["Multiply"] = new MulNode(scene, "Multiply", {COLOR_1, COLOR_3}, {COLOR_2});
    nodes["Divide"] = new DivNode(scene, "Divide", {COLOR_1, COLOR_3}, {COLOR_2});

    for (auto n : nodes) {
        n->setHeightWidth(90, 180);
        n->setPosition(LEFT_TOP, RIGHT_BOTTOM);
    }

    return nodes;
}

vector<Node*> NodeRegistery::getInputNode() const {
    auto *node = new InputNode(scene, "Input", {COLOR_2});

    node->setHeightWidth(140, 200);
    node->setPosition(LEFT_TOP, RIGHT_TOP);

    return {node};
}

vector<Node*> NodeRegistery::getOutputNode() const {
    auto *node = new OutputNode(scene, "Output", {COLOR_1});

    node->setHeightWidth(140, 200);
    node->setPosition(LEFT_TOP, RIGHT_BOTTOM);

    return {node};
}

void registerAllNodeTypes() {
    registerInputNodeType();
    registerOutputNodeType();
    registerFunctionNodeType();
}

static const bool nodeTypesRegistered = (registerAllNodeTypes(), true);
