//
// Created by Ruhaan on 02/10/25.
//

#include "BitOp.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"

AndNode::AndNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, true) {
    vals = 0;
}

OrNode::OrNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, true) {
    vals = 0;
}

NotNode::NotNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

XorNode::XorNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, true) {
    vals = 0;
}

void AndNode::execute() {
    int ans = 0;
    int connectedCount = 0;
    bool first = true;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        int v = static_cast<int>(getNodeValue(prev, startSock));
        if (first) {
            ans = v;
            first = false;
        } else {
            ans &= v;
        }
        connectedCount++;
    }
    if (connectedCount >= 2) {
        vals = static_cast<int>(ans);
    }
}

void OrNode::execute() {
    int ans = 0;
    int connectedCount = 0;
    bool first = true;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        int v = static_cast<int>(getNodeValue(prev, startSock));
        if (first) {
            ans = v;
            first = false;
        } else {
            ans |= v;
        }
        connectedCount++;
    }
    if (connectedCount >= 2) {
        vals = static_cast<int>(ans);
    }
}

void NotNode::execute() {
    auto *s = inputs[0];
    if (!s) return;
    if (s->socket_type == "addsocket") return;
    auto *prev = getPrevNode(0);
    if (!prev) return;
    auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
    int v = static_cast<int>(getNodeValue(prev, startSock));
    vals = static_cast<int>(~v);
}
void XorNode::execute() {
    int ans = 0;
    int connectedCount = 0;
    bool first = true;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        int v = static_cast<int>(getNodeValue(prev, startSock));
        if (first) {
            ans = v;
            first = false;
        } else {
            ans ^= v;
        }
        connectedCount++;
    }
    if (connectedCount >= 2) {
        vals = static_cast<int>(ans);
    }
}

QJsonObject AndNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "AndNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject OrNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "OrNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject NotNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "NotNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject XorNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "XorNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool AndNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool OrNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool NotNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool XorNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}
