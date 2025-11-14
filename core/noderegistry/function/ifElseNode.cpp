// filepath: /Users/ruhaan/Documents/DiverseXEngine/core/noderegistry/function/ifElseNode.cpp
#include "ifElseNode.h"
#include "functionNode.h"
#include "../inout/inoutNode.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include "../../widgets/widgets.h"

#include <QJsonObject>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFont>
#include <cmath>

static inline bool toBool(long double v) {
    return std::fabs(static_cast<double>(v)) > 1e-9;
}

IfElseNode::IfElseNode(Scene *scene_, const std::string &title)
: FunctionNode(scene_, title, {"If (Bool)", "Then Value", "Else Value"}, {"Output"}, false) {
    // pairCount starts at 1: (If, Then) + Else
    pairCount = 1;
    activeValue = 0.0;
    showContent();
}

long double IfElseNode::fetchInputValue(int idx) const {
    if (idx < 0 || idx >= static_cast<int>(inputs.size())) return 0.0L;
    auto *sock = inputs[idx];
    if (!sock || !sock->hasEdge() || !sock->getFirstEdge() || !sock->getFirstEdge()->startSocket) return 0.0L;
    Node* prev = sock->getFirstEdge()->startSocket->node;
    if (!prev) return 0.0L;
    SocketNode* startSock = sock->getFirstEdge()->startSocket;
    return static_cast<long double>(const_cast<IfElseNode*>(this)->getNodeValue(prev, startSock));
}

void IfElseNode::execute() {
    // Layout: [C0, V0, C1, V1, ..., Cn-1, Vn-1, Else]
    int totalInputs = static_cast<int>(inputs.size());
    if (totalInputs < 3) { activeValue = 0.0; return; }

    // Compute number of pairs from inputs
    int computedPairs = (totalInputs - 1) / 2; // floor
    if (computedPairs < 1) computedPairs = 1;

    // Iterate pairs
    for (int i = 0; i < computedPairs; ++i) {
        int condIdx = 2 * i;
        int valIdx  = 2 * i + 1;
        long double condV = fetchInputValue(condIdx);
        if (toBool(condV)) {
            activeValue = static_cast<double>(fetchInputValue(valIdx));
            return;
        }
    }
    // Else
    int elseIdx = 2 * computedPairs; // last input
    activeValue = static_cast<double>(fetchInputValue(elseIdx));
}

void IfElseNode::addElseIfPair() {
    // Insert two sockets just before the final Else socket
    // Current layout size
    int sz = static_cast<int>(inputs.size());
    if (sz < 3) return;
    int insertIndex = sz - 1; // before Else

    // Label numbers start from 2 for first Else If
    int pairNo = pairCount + 1; // human-friendly: If=1, Else If 2,3,...

    addInputSocket(insertIndex, QString("Else If %1 (Bool)").arg(pairNo));
    addInputSocket(insertIndex + 1, QString("Then Value %1").arg(pairNo));

    pairCount++;
}

void IfElseNode::showContent() {
    if (content) return;

    auto *widget = new WidgetNode(this);

    // Add a compact horizontal strip with an Add Else If button
    auto *strip = new QWidget(widget);
    auto *lay = new QHBoxLayout(strip);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(6);

    addBtn = new QPushButton("Add Else If", strip);
    QFont font("Arial", 11, QFont::Normal);
    addBtn->setFont(font);
    addBtn->setMinimumHeight(24);

    QObject::connect(addBtn, &QPushButton::clicked, [this]() {
        this->addElseIfPair();
    });

    lay->addWidget(addBtn);
    strip->setLayout(lay);

    widget->addContent({strip});
    setContentHeight(70);
    setContent(widget);
}

void IfElseNode::hideContent() {
    if (content) {
        content->hide();
        content = nullptr;
    }
}

QJsonObject IfElseNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "IfElseNode";
    // Recompute pairCount from inputs: (N-1)/2
    int totalInputs = static_cast<int>(inputs.size());
    int computedPairs = (totalInputs - 1) / 2;
    obj["pair_count"] = computedPairs;
    obj["active_value"] = activeValue;
    return obj;
}

bool IfElseNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    bool ok = FunctionNode::deserialize(data, hashmap);

    // After sockets are reconstructed, re-derive pairCount
    int totalInputs = static_cast<int>(inputs.size());
    pairCount = std::max(1, (totalInputs - 1) / 2);
    activeValue = data.value("active_value").toDouble(0.0);

    showContent();
    return ok;
}

void registerIfElseNodeType() {
    Node::registerType("IfElseNode", [](Scene* scene){ return new IfElseNode(scene); });
}
