//
// Created by Ruhaan on 18/08/25.
//

#include "noderegister.h"
#include "../nodes/node.h"
#include "inoutNode.h"
#include "functionNode.h"
#include "conditionNode.h"

QHash<QString, Node*> NodeRegistery::getfunctionalNode() const {
    QHash<QString, Node*> nodes;
    nodes["Add"] = new AddNode(scene, "Add", {"Number 1", "Number 2"}, {"Output"});
    nodes["Subtract"] = new SubNode(scene, "Subtract", {"Number 1", "Number 2"}, {"Output"});
    nodes["Multiply"] = new MulNode(scene, "Multiply", {"Number 1", "Number 2"}, {"Output"});
    nodes["Divide"] = new DivNode(scene, "Divide", {"Number 1", "Number 2"}, {"Output"});
    nodes["Condition"] = new ConditionNode(scene, "Condition");

    for (auto n : nodes) {
        n->setHeightWidth(90, 180);
        n->setPosition(LEFT_TOP, RIGHT_BOTTOM);
    }
    nodes["Condition"]->setHeightWidth(150, 200);

    return nodes;
}

vector<Node*> NodeRegistery::getInputNode() const {
    auto *node = new InputNode(scene, "Input", {""});

    node->setHeightWidth(90, 180);
    node->setPosition(LEFT_TOP, RIGHT_TOP);

    return {node};
}

vector<Node*> NodeRegistery::getOutputNode() const {
    auto *node = new OutputNode(scene, "Output", {""});

    node->setHeightWidth(90, 180);
    node->setPosition(LEFT_TOP, RIGHT_BOTTOM);

    return {node};
}


void registerAllNodeTypes() {
    registerInputNodeType();
    registerOutputNodeType();
    registerFunctionNodeType();
    registerConditionNodeType();
}

static const bool nodeTypesRegistered = (registerAllNodeTypes(), true);
