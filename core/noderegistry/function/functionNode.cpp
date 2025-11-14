//
// Created by Ruhaan on 19/08/25.
//

#include "functionNode.h"

#include "../inout/inoutNode.h"
#include "../../nodes/edge.h"
#include "../../nodes/socket.h"
#include "../canvasNode.h"
#include "../../scene/nodescene.h"
#include "../inout/constantNode.h"
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

static vector<QString> withAddSocket(vector<QString> v) {
    if (v.empty()) {
        v.emplace_back("Number 1");
        v.emplace_back("Number 2");
        v.emplace_back("addsocket");
        return v;
    }
    bool exists = false;
    for (auto &s : v) if (s == "addsocket") exists = true;
    if (!exists) v.emplace_back("addsocket");
    return v;
}

FunctionNode::FunctionNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size, bool allow_addsocket)
: Node(scene_, title, allow_addsocket ? withAddSocket(input_size) : input_size, output_size), allow_addsocket(allow_addsocket) {}

// Provide context-aware value fetching using the upstream start socket when available
static thread_local bool __dvx_inCanvasCheck = false;

double FunctionNode::getNodeValue(Node* node, SocketNode* startSocket) {
    if (!node) return 0;
    if (auto* fn = dynamic_cast<FunctionNode*>(node)) {
        // Gate by Canvas: if this function node is inside any disabled Canvas, don’t execute
        if (!__dvx_inCanvasCheck) {
            __dvx_inCanvasCheck = true;
            Scene* sc = fn->scene;
            if (sc) {
                for (Node* n : sc->nodes) {
                    auto *canvas = dynamic_cast<CanvasNode*>(n);
                    if (!canvas) continue;
                    // Avoid nested recursion: if calling isEnabled would re-enter getNodeValue, skip gating for now
                    if (!canvas->isEnabled()) {
                        if (std::find(canvas->inner_nodes.begin(), canvas->inner_nodes.end(), fn) != canvas->inner_nodes.end()) {
                            __dvx_inCanvasCheck = false;
                            return 0.0; // blocked
                        }
                    }
                }
            }
            __dvx_inCanvasCheck = false;
        }
        fn->execute();
        // If a startSocket is provided, allow the node to return a value specific to that output
        if (startSocket) return fn->getValuesForOutput(startSocket);
        return fn->getValues();
    }
    if (auto* in = dynamic_cast<InputNode*>(node)) {
        return in->vals;
    }
    if (auto* cst = dynamic_cast<ConstantNode*>(node)) {
        return static_cast<double>(cst->vals);
    }
    return 0;
}

double FunctionNode::getNodeValue(Node* node) {
    if (!node) return 0;
    if (auto* fn = dynamic_cast<FunctionNode*>(node)) {
        // Gate by Canvas: if this function node is inside any disabled Canvas, don’t execute
        if (!__dvx_inCanvasCheck) {
            __dvx_inCanvasCheck = true;
            Scene* sc = fn->scene;
            if (sc) {
                for (Node* n : sc->nodes) {
                    auto *canvas = dynamic_cast<CanvasNode*>(n);
                    if (!canvas) continue;
                    if (!canvas->isEnabled()) {
                        if (std::find(canvas->inner_nodes.begin(), canvas->inner_nodes.end(), fn) != canvas->inner_nodes.end()) {
                            __dvx_inCanvasCheck = false;
                            return 0.0; // blocked
                        }
                    }
                }
            }
            __dvx_inCanvasCheck = false;
        }
        fn->execute();
        return fn->getValues();
    }
    if (auto* in = dynamic_cast<InputNode*>(node)) {
        return in->vals;
    }
    if (auto* cst = dynamic_cast<ConstantNode*>(node)) {
        return static_cast<double>(cst->vals);
    }
    return 0;
}

Node* FunctionNode::getPrevNode(const int idx) const {
    if (idx < 0 || idx >= static_cast<int>(this->inputs.size())) return nullptr;
    auto *input = this->inputs[idx];
    if (input && input->hasEdge() && input->getFirstEdge() && input->getFirstEdge()->startSocket) {
        return input->getFirstEdge()->startSocket->node;
    }
    return nullptr;
}

void FunctionNode::showContent() {
    if (!content) {
        widget = new WidgetNode(this);
        auto textEdit = new TextEdit(QString::number(getValues()));
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
}

void FunctionNode::hideContent() {
    if (content) {
        content->hide();
        content = nullptr;
    }
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
    old_obj["multi_sockets"] = allow_addsocket;
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
        const QString socketType = new_i.value("socket_type").toString();
        auto *sok = new SocketNode(this, new_i.value("index").toInt(), pos, socketType);
        sok->deserialize(new_i, hashmap);
        inputs.push_back(sok);
    }


    if (!inputs.empty() && inputs[0]) {
        this->in_pos = inputs[0]->position;
    }

    bool multi_sockets = data.value("multi_sockets").toBool();
    if (multi_sockets) {
        bool hasAddSocket = false;
        for (auto *s : inputs) {
            if (s && s->socket_type == "addsocket") { hasAddSocket = true; break; }
        }
        if (!hasAddSocket) {
            const auto pos = getPosition(static_cast<int>(this->in_pos));
            auto *sok = new SocketNode(this, static_cast<int>(inputs.size()), pos, QString("addsocket"));
            inputs.push_back(sok);
        }
    }

    for (size_t idx = 0; idx < inputs.size(); ++idx) {
        if (inputs[idx]) inputs[idx]->index = static_cast<int>(idx);
    }

    QJsonArray out = data.value("outputs").toArray();
    for (auto s : outputs) { delete s; }
    outputs.clear();
    for (auto o : out) {
        auto new_o = o.toObject();
        const auto pos = getPosition(new_o.value("position").toInt());
        const QString socketType = new_o.value("socket_type").toString();
        auto *sok = new SocketNode(this, new_o.value("index").toInt(), pos, socketType);
        sok->deserialize(new_o, hashmap);
        outputs.push_back(sok);
    }

    if (!outputs.empty() && outputs[0]) {
        this->out_pos = outputs[0]->position;
    }

    refreshSocketsAndEdges();

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
AddNode::AddNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void AddNode::execute() {
    long double sum = 0;
    int connectedCount = 0;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        long double v = getNodeValue(prev, startSock);
        sum += v;
        connectedCount++;
    }
    if (connectedCount >= 2) {
        vals = sum;
    }
}

QJsonObject AddNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "AddNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool AddNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}


// SubNode subtracts the two numbers
SubNode::SubNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
    :FunctionNode(scene_, title, input_size, output_size){
    vals = 0;
}

void SubNode::execute() {
    long double result = 0;
    bool firstFound = false;
    int connectedCount = 0;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        long double v = getNodeValue(prev, startSock);
        if (!firstFound) {
            result = v;
            firstFound = true;
        } else {
            result -= v;
        }
        connectedCount++;
    }
    if (connectedCount >= 2 && firstFound) {
        vals = result;
    }
}

QJsonObject SubNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "SubNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool SubNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

// MulNode multiplies the two numbers
MulNode::MulNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
    :FunctionNode(scene_, title, input_size, output_size){
    vals = 0;
}

void MulNode::execute() {
    long double prod = 1;
    int connectedCount = 0;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        long double v = getNodeValue(prev, startSock);
        prod *= v;
        connectedCount++;
    }
    if (connectedCount >= 2) {
        vals = prod;
    }
}

QJsonObject MulNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "MulNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool MulNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

// DivNode divides the two numbers
DivNode::DivNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
    :FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void DivNode::execute() {
    long double result = 0;
    bool firstFound = false;
    int connectedCount = 0;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        long double v = getNodeValue(prev, startSock);
        if (!firstFound) {
            result = v;
            firstFound = true;
        } else {
            if (v == 0) {
                vals = INFINITY;
                setInfoText("Number is not Divisible by 0");
                return;
            }
            result /= v;
        }
        connectedCount++;
    }
    if (connectedCount >= 2 && firstFound) {
        vals = result;
    }
}

QJsonObject DivNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "DivNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool DivNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

// FloorDivNode performs floor(a/b) similar to Python's floor division
FloorDivNode::FloorDivNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
    : FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void FloorDivNode::execute() {
    long double result = 0;
    bool firstFound = false;
    int connectedCount = 0;
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto *s = inputs[i];
        if (!s) continue;
        if (s->socket_type == "addsocket") continue;
        auto *prev = getPrevNode(static_cast<int>(i));
        if (!prev) continue;
        auto *startSock = (s && s->getFirstEdge()) ? s->getFirstEdge()->startSocket : nullptr;
        long double v = getNodeValue(prev, startSock);
        if (!firstFound) {
            result = v;
            firstFound = true;
        } else {
            if (v == 0) {
                vals = INFINITY;
                setInfoText("Number is not Divisible by 0");
                return;
            }
            result /= v;
        }
        connectedCount++;
    }
    if (connectedCount >= 2 && firstFound) {
        // Apply floor at the end to mirror math.floor(a/b)
        vals = std::floor(static_cast<double>(result));
    }
}

QJsonObject FloorDivNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "FloorDivNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool FloorDivNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

void registerFunctionNodeType() {
    Node::registerType("AddNode", [](Scene* scene) { return new AddNode(scene); });
    Node::registerType("SubNode", [](Scene* scene) { return new SubNode(scene); });
    Node::registerType("MulNode", [](Scene* scene) { return new MulNode(scene); });
    Node::registerType("DivNode", [](Scene* scene) { return new DivNode(scene); });
    Node::registerType("FloorDivNode", [](Scene* scene) { return new FloorDivNode(scene); });
}