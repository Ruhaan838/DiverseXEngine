#include "conditionNode.h"
#include "functionNode.h"
#include "inoutNode.h"
#include "../nodes/socket.h"
#include "../nodes/edge.h"
#include <QJsonObject>
#include <cmath>

ConditionNode::ConditionNode(Scene *scene_, const string &title)
: FunctionNode(scene_, title, {"Number 1", "Number 2"}, {"Ans"}) {
    auto *widget = new WidgetNode(this);
    bool_list = new QComboBox();
    bool_list->addItem("A > B");
    bool_list->addItem("A < B");
    bool_list->addItem("A == B");
    bool_list->addItem("A != B");
    bool_list->addItem("A <= B");
    bool_list->addItem("A >= B");
    bool_list->setFixedHeight(28);
    widget->addContent({bool_list});
    setContentHeight(70);
    setContent(widget);
}

long double ConditionNode::fetchInputValue(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(inputs.size())) return 0.0;
    auto *sock = inputs[idx];
    if (!sock || !sock->hasEdge() || !sock->edge || !sock->edge->startSocket) return 0.0;
    Node* prev = sock->edge->startSocket->node;
    if (!prev) return 0.0;
    return static_cast<long double>(const_cast<ConditionNode*>(this)->getNodeValue(prev));
}

bool ConditionNode::evaluateCondition() {
    long double a = fetchInputValue(0);
    long double b = fetchInputValue(1);
    const long double eps = 1e-9L;
    int op = bool_list ? bool_list->currentIndex() : 0;
    switch (op) {
        case 0: result = (a > b + eps); break;
        case 1: result = (a < b - eps); break;
        case 2: result = (fabsl(a - b) <= eps); break;
        case 3: result = (fabsl(a - b) > eps); break;
        case 4: result = (a <= b + eps); break;
        case 5: result = (a >= b - eps); break;
        default: result = false; break;
    }
    activeValue = result ? 1.0 : 0.0;
    return result;
}

void ConditionNode::execute() { evaluateCondition(); }

QJsonObject ConditionNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "ConditionNode";
    if (bool_list) obj["op_index"] = bool_list->currentIndex();
    obj["result"] = result;
    return obj;
}


bool ConditionNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    bool ok = FunctionNode::deserialize(data, hashmap);
    if (bool_list) bool_list->setCurrentIndex(data.value("op_index").toInt());
    result = data.value("result").toBool();
    activeValue = result ? 1.0 : 0.0;
    return ok;
}

double ConditionNode::getValues() {
    return activeValue;
}

//the only reason why these methods are empty is that we don't want to show and hide the value inside the conditional node.
void ConditionNode::showContent() {}
void ConditionNode::hideContent() {}

void registerConditionNodeType() {
    Node::registerType("ConditionNode", [](Scene* scene){ return new ConditionNode(scene); });
}