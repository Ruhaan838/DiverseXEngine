//
// Created by Ruhaan on 24/08/25.
//

#include "inoutNode.h"
#include "functionNode.h"
#include "../scene/nodescene.h"
#include "../widgets/widgets.h"
#include "../nodes/socket.h"
#include "../nodes/edge.h"
#include "../nodes/node.h"
#include <QString>

inline const string out_node_stylesheet = R"(
    QTextEdit {
        background-color: #2b2b2b;
        color: #ffffff;
        border: 2px solid #3d3d3d;
        border-radius: 5px;
        padding: 2px 8px;
    }
)";

OutputNode::OutputNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size)
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
    if (!inputs.empty() && inputs[0] && inputs[0]->hasEdge() && inputs[0]->edge && inputs[0]->edge->startSocket) {
        Node* inputNode = inputs[0]->edge->startSocket->node;
        if (auto n = dynamic_cast<FunctionNode*>(inputNode)) {
            vals = n->getNodeValue(n);
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
        const auto item = getSocketType(new_i.value("socket_type").toInt());
        auto *sok = new SocketNode(this, new_i.value("index").toInt(), pos, item);
        sok->deserialize(new_i, hashmap);
        inputs.push_back(sok);
    }

    return true && old_obj;
}

void registerOutputNodeType() {
    Node::registerType("OutputNode", [](Scene* scene) { return new OutputNode(scene); });
}
