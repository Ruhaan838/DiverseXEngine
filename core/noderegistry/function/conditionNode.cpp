#include "conditionNode.h"
#include "functionNode.h"
#include "../inout/inoutNode.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include <QJsonObject>
#include <cmath>

ConditionNode::ConditionNode(Scene *scene_, const string &title)
: FunctionNode(scene_, title, {"Number 1", "Number 2"}, {"True", "False"}, false) {
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
    auto *startSock = sock->edge->startSocket;
    return static_cast<long double>(const_cast<ConditionNode*>(this)->getNodeValue(prev, startSock));
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

double ConditionNode::getValues() {
    return activeValue;
}

// Per-output values: "True" socket yields 1 when condition true else 0; "False" yields inverse
double ConditionNode::getValuesForOutput(SocketNode* startSocket) {
    // Ensure latest result
    evaluateCondition();
    if (!startSocket) return activeValue;
    // Prefer socket_type labels, fallback to index
    QString type = startSocket->socket_type;
    if (type == "True") return result ? 1.0 : 0.0;
    if (type == "False") return result ? 0.0 : 1.0;
    // Fallback by index: assume 0=True, 1=False
    if (startSocket->index == 0) return result ? 1.0 : 0.0;
    if (startSocket->index == 1) return result ? 0.0 : 1.0;
    return activeValue;
}

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

    // Ensure we have exactly two outputs labeled True/False for new behavior
    POSITION pos = this->out_pos;
    if (outputs.size() == 0) {
        auto *t = new SocketNode(this, 0, pos, QString("True"));
        auto *f = new SocketNode(this, 1, pos, QString("False"));
        outputs.push_back(t);
        outputs.push_back(f);
    } else if (outputs.size() == 1) {
        if (outputs[0]) {
            outputs[0]->index = 0;
            outputs[0]->socket_type = "True";
            outputs[0]->position = pos;
        }
        auto *f = new SocketNode(this, 1, pos, QString("False"));
        outputs.push_back(f);
    } else {
        // At least two outputs; normalize first two labels
        if (outputs[0]) { outputs[0]->index = 0; outputs[0]->socket_type = "True"; outputs[0]->position = pos; }
        if (outputs[1]) { outputs[1]->index = 1; outputs[1]->socket_type = "False"; outputs[1]->position = pos; }
        // Re-index any extras to keep consistent
        for (size_t i = 2; i < outputs.size(); ++i) {
            if (outputs[i]) outputs[i]->index = static_cast<int>(i);
        }
    }

    refreshSocketsAndEdges();
    return ok;
}


//the only reason why these methods are empty is that we don't want to show and hide the value inside the conditional node.
void ConditionNode::showContent() {}
void ConditionNode::hideContent() {}

void registerConditionNodeType() {
    Node::registerType("ConditionNode", [](Scene* scene){ return new ConditionNode(scene); });
}