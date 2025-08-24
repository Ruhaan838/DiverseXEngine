//
// Created by Ruhaan on 19/08/25.
//

#include "functionNode.h"

#include "inoutNode.h"
#include "../nodes/edge.h"
#include "../nodes/socket.h"


FunctionNode::FunctionNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
: Node(scene_, title, input_size, output_size){}

double FunctionNode::getNodeValue(Node* node) {
    if (!node) return 0;
    if (auto* fn = dynamic_cast<FunctionNode*>(node)) {
        fn->execute();
        return fn->getValues();
    }
    if (auto* in = dynamic_cast<InputNode*>(node)) {
        return in->vals;
    }
    return 0;
}

Node* FunctionNode::getPrevNode(const int idx) const {
    if (idx < 0 || idx >= static_cast<int>(this->inputs.size())) return nullptr;
    auto *input = this->inputs[idx];
    if (input && input->hasEdge() && input->edge && input->edge->startSocket) {
        return input->edge->startSocket->node;
    }
    return nullptr;
}

QJsonObject FunctionNode::serialize() {
    auto old_obj = Node::serialize();
    QJsonArray in;
    for (auto i : inputs) {
        in.append(i->serialize());
    }
    QJsonArray out;
    for (auto o : outputs) {
        out.append(o->serialize());
    }
    old_obj["inputs"] = in;
    old_obj["outputs"] = out;
    auto hw = this->getHeightAndWidth();
    old_obj["h"] = hw.first;
    old_obj["w"] = hw.second;
    return old_obj;
}

bool FunctionNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    auto h = data.value("h").toInt();
    auto w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();
    auto old_obj = Node::deserialize(data, hashmap);

    QJsonArray in = data.value("inputs").toArray();
    for (auto s : inputs) { delete s; }
    inputs.clear();
    for (auto i : in) {
        auto new_i = i.toObject();
        const auto pos = getPosition(new_i.value("position").toInt());
        const auto item = getSocketType(new_i.value("socket_type").toInt());
        auto *sok = new SocketNode(this, new_i.value("index").toInt(), pos, item);
        sok->deserialize(new_i, hashmap);
        inputs.push_back(sok);
    }

    QJsonArray out = data.value("outputs").toArray();
    for (auto s : outputs) { delete s; }
    outputs.clear();
    for (auto o : out) {
        auto new_o = o.toObject();
        const auto pos = getPosition(new_o.value("position").toInt());
        const auto item = getSocketType(new_o.value("socket_type").toInt());
        auto *sok = new SocketNode(this, new_o.value("index").toInt(), pos, item);
        sok->deserialize(new_o, hashmap);
        outputs.push_back(sok);
    }

    return true && old_obj;
}

/* ==========================================================
 This function Nodes must follow the rules:
 1. It must have the condition checking for the number of inputs in execute function and input_size and the
    input vector must be greater than or equal to the number of inputs required.
 2. It must get the node first and then convert it into the input node be using function getPrevNode(idx) at bound index.
 3. make serializing and deserializing methods for function node.
   ==========================================================
*/

// AddNode add the two numbers
AddNode::AddNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
: FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void AddNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        vals = v1 + v2;
    }
}

QJsonObject AddNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "AddNode";
    return old_obj;
}

bool AddNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    return FunctionNode::deserialize(data, hashmap);
}


// SubNode subtracts the two numbers
SubNode::SubNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
    :FunctionNode(scene_, title, input_size, output_size){
    vals = 0;
}

void SubNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        vals = v1 - v2;
    }
}

QJsonObject SubNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "SubNode";
    return old_obj;
}

bool SubNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    return FunctionNode::deserialize(data, hashmap);
}

// MulNode multiplies the two numbers
MulNode::MulNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
    :FunctionNode(scene_, title, input_size, output_size){
    vals = 0;
}

void MulNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        vals = v1 * v2;
    }
}

QJsonObject MulNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "MulNode";
    return old_obj;
}

bool MulNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    return FunctionNode::deserialize(data, hashmap);
}

// DivNode divides the two numbers
DivNode::DivNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
    :FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void DivNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        if (v2 != 0) {
            vals = v1 / v2;
        } else {
            vals = INFINITY;
        }
    }
}

QJsonObject DivNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "DivNode";
    return old_obj;
}

bool DivNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    return FunctionNode::deserialize(data, hashmap);
}

void registerFunctionNodeType() {
    Node::registerType("AddNode", [](Scene* scene) { return new AddNode(scene); });
    Node::registerType("SubNode", [](Scene* scene) { return new SubNode(scene); });
    Node::registerType("MulNode", [](Scene* scene) { return new MulNode(scene); });
    Node::registerType("DivNode", [](Scene* scene) { return new DivNode(scene); });
}