//
// Created by Ruhaan on 18/08/25.
//

#include "noderegister.h"
#include "../nodes/node.h"
#include "inoutNode.h"
#include "functionNode.h"
#include "conditionNode.h"
#include "canvasNode.h"


static const bool Add_registered = (NodeRegistery::registerNode(
    "Add", NodeCategory::FUNCTION,
    [](Scene* scene) -> Node* {
        return new AddNode(scene, "Add",
                          std::vector<QString>{"Number 1", "Number 2"},
                          std::vector<QString>{"Output"});
    }), true);

static const bool Subtract_registered = (NodeRegistery::registerNode(
    "Subtract", NodeCategory::FUNCTION,
    [](Scene* scene) -> Node* {
        return new SubNode(scene, "Subtract",
                          std::vector<QString>{"Number 1", "Number 2"},
                          std::vector<QString>{"Output"});
    }), true);

static const bool Multiply_registered = (NodeRegistery::registerNode(
    "Multiply", NodeCategory::FUNCTION,
    [](Scene* scene) -> Node* {
        return new MulNode(scene, "Multiply",
                          std::vector<QString>{"Number 1", "Number 2"},
                          std::vector<QString>{"Output"});
    }), true);

static const bool Divide_registered = (NodeRegistery::registerNode(
    "Divide", NodeCategory::FUNCTION,
    [](Scene* scene) -> Node* {
        return new DivNode(scene, "Divide",
                          std::vector<QString>{"Number 1", "Number 2"},
                          std::vector<QString>{"Output"});
    }), true);

static const bool Condition_registered = (NodeRegistery::registerNode(
    "Condition", NodeCategory::FUNCTION,
    [](Scene* scene) -> Node* { return new ConditionNode(scene, "Condition"); },
    200, 150), true);

static const bool Input_registered = (NodeRegistery::registerNode(
    "Input", NodeCategory::INPUT,
    [](Scene* scene) -> Node* {
        return new InputNode(scene, "Input", std::vector<QString>{""});
    }), true);

static const bool Output_registered = (NodeRegistery::registerNode(
    "Output", NodeCategory::OUTPUT,
    [](Scene* scene) -> Node* {
        return new OutputNode(scene, "Output", std::vector<QString>{""});
    }), true);

// static const bool Canvas_registered = (NodeRegistery::registerNode(
//     "Canvas", NodeCategory::CONTAINER,
//     [](Scene* scene) -> Node* { return new CanvasNode(scene); }), true);

void registerAllNodeTypes() {}
