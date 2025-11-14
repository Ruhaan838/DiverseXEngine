//
// Created by Ruhaan on 24/08/25.
//

#include "inoutNode.h"
#include "../function/functionNode.h"
#include "../../scene/nodescene.h"
#include "../../widgets/widgets.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include "../../nodes/node.h"
#include <QString>
#include <cmath>

inline const string out_node_stylesheet = R"(
    QTextEdit {
        background-color: #2b2b2b;
        color: #ffffff;
        border: 2px solid #3d3d3d;
        border-radius: 5px;
        padding: 2px 8px;
    }
)";

OutputNode::OutputNode(Scene *scene_, const string &title, vector<QString> input_size)
: Node(scene_, title, input_size, {}) {
    auto *widget = new WidgetNode(this);
    textEdit = new TextEdit(QString::number(vals));

    QFont font("Arial", 20, QFont::Normal);
    textEdit->setFont(font);
    textEdit->setMinimumWidth(120);
    textEdit->setMinimumHeight(30);
    textEdit->setAlignment(Qt::AlignCenter);

    textEdit->setStyleSheet(
        out_node_stylesheet.c_str()
    );

    textEdit->setReadOnly(true);
    widget->addContent({textEdit});
    setContent(widget);
}

void OutputNode::execute() {
    if (!inputs.empty() && inputs[0] && inputs[0]->hasEdge() && inputs[0]->getFirstEdge() && inputs[0]->getFirstEdge()->startSocket) {
        Node* inputNode = inputs[0]->getFirstEdge()->startSocket->node;
        SocketNode* startSock = inputs[0]->getFirstEdge()->startSocket;
        if (auto n = dynamic_cast<FunctionNode*>(inputNode)) {
            vals = n->getNodeValue(n, startSock);
        } else if (auto in = dynamic_cast<InputNode*>(inputNode)) {
            vals = in->vals;
        } else {
            vals = 0;
        }
        if (textEdit) {
            textEdit->setText(std::to_string(vals).c_str());
        }
    }
}

QJsonObject OutputNode::serialize() {
    auto old_obj = Node::serialize();
    old_obj["value"] = vals;
    QJsonArray in;
    for (auto i : inputs) {
        in.append(i->serialize());
    }
    old_obj["inputs"] = in;
    old_obj["node_type"] = "OutputNode";
    auto hw = this->getHeightAndWidth();
    old_obj["h"] = hw.first;
    old_obj["w"] = hw.second;
    return old_obj;
}

bool OutputNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    auto h = data.value("h").toInt();
    auto w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();

    auto old_obj = Node::deserialize(data, hashmap);

    auto in = data.value("inputs").toArray();
    auto val = data.value("value").toDouble();
    vals = val;
    if (textEdit) {
        textEdit->setText(std::to_string(vals).c_str());
    }
    for (auto s : inputs) { delete s; }
    inputs.clear();
    for (auto i: in) {
        auto new_i = i.toObject();
        const auto pos = getPosition(new_i.value("position").toInt());
        const QString socketType = new_i.value("socket_type").toString();
        auto *sok = new SocketNode(this, new_i.value("index").toInt(), pos, socketType);
        sok->deserialize(new_i, hashmap);
        inputs.push_back(sok);
    }

    return true && old_obj;
}

Node* OutputNode::getPrevNode(int inputIndex) {

    if (inputIndex < 0 || inputIndex >= inputs.size()) {
        return nullptr;
    }
    if (!inputs[inputIndex]) {
        return nullptr;
    }
    if (inputs[inputIndex]->hasEdge() && inputs[inputIndex]->getFirstEdge() && inputs[inputIndex]->getFirstEdge()->startSocket) {
        Node* prevNode = inputs[inputIndex]->getFirstEdge()->startSocket->node;
        return prevNode;
    }
    return nullptr;
}

// BoolOutputNode: prints True/False based on input value
BoolOutputNode::BoolOutputNode(Scene *scene_, const string &title, vector<QString> input_size)
: Node(scene_, title, input_size, {}) {
    auto *widget = new WidgetNode(this);
    textEdit = new TextEdit(bval ? "True" : "False", widget);

    QFont font("Arial", 20, QFont::Normal);
    textEdit->setFont(font);
    textEdit->setMinimumWidth(120);
    textEdit->setMinimumHeight(30);
    textEdit->setAlignment(Qt::AlignCenter);

    textEdit->setStyleSheet(
        out_node_stylesheet.c_str()
    );

    textEdit->setReadOnly(true);
    widget->addContent({textEdit});
    setContent(widget);
}

static long double extractValue(Node* n, SocketNode* startSock) {
    if (!n) return 0.0L;
    if (auto fn = dynamic_cast<FunctionNode*>(n)) {
        return static_cast<long double>(fn->getNodeValue(fn, startSock));
    }
    if (auto in = dynamic_cast<InputNode*>(n)) {
        return static_cast<long double>(in->vals);
    }
    if (auto out = dynamic_cast<OutputNode*>(n)) {
        return static_cast<long double>(out->vals);
    }
    if (auto bout = dynamic_cast<BoolOutputNode*>(n)) {
        return bout->bval ? 1.0L : 0.0L;
    }
    return 0.0L;
}

void BoolOutputNode::execute() {
    bool value = false;
    long double raw = 0.0L;
    if (!inputs.empty() && inputs[0] && inputs[0]->hasEdge() && inputs[0]->getFirstEdge() && inputs[0]->getFirstEdge()->startSocket) {
        Node* inputNode = inputs[0]->getFirstEdge()->startSocket->node;
        SocketNode* startSock = inputs[0]->getFirstEdge()->startSocket;
        raw = extractValue(inputNode, startSock);
    }
    const double eps = 1e-9;
    auto v = static_cast<double>(raw);
    value = std::fabs(v) > eps; // non-zero => true
    bval = value;
    if (textEdit) {
        textEdit->setText(bval ? "True" : "False");
    }
}

QJsonObject BoolOutputNode::serialize() {
    auto obj = Node::serialize();
    obj["node_type"] = "BoolOutputNode";
    obj["value"] = bval;
    QJsonArray in;
    for (auto i : inputs) {
        in.append(i->serialize());
    }
    obj["inputs"] = in;
    auto hw = this->getHeightAndWidth();
    obj["h"] = hw.first;
    obj["w"] = hw.second;
    return obj;
}

bool BoolOutputNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    auto h = data.value("h").toInt();
    auto w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();

    auto old_obj = Node::deserialize(data, hashmap);

    auto inArr = data.value("inputs").toArray();
    bval = data.value("value").toBool();
    if (textEdit) {
        textEdit->setText(bval ? "True" : "False");
    }

    for (auto s : inputs) { delete s; }
    inputs.clear();
    for (auto i: inArr) {
        auto new_i = i.toObject();
        const auto pos = getPosition(new_i.value("position").toInt());
        const QString socketType = new_i.value("socket_type").toString();
        auto *sok = new SocketNode(this, new_i.value("index").toInt(), pos, socketType);
        sok->deserialize(new_i, hashmap);
        inputs.push_back(sok);
    }

    return true && old_obj;
}

Node* BoolOutputNode::getPrevNode(int inputIndex) {
    if (inputIndex < 0 || inputIndex >= inputs.size()) {
        return nullptr;
    }
    if (!inputs[inputIndex]) {
        return nullptr;
    }
    if (inputs[inputIndex]->hasEdge() && inputs[inputIndex]->getFirstEdge() && inputs[inputIndex]->getFirstEdge()->startSocket) {
        Node* prevNode = inputs[inputIndex]->getFirstEdge()->startSocket->node;
        return prevNode;
    }
    return nullptr;
}

// Note: Registration is handled in functions.cpp under the Outputs category.
