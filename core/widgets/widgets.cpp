//
// Created by Ruhaan on 12/08/25.
//

#include "widgets.h"

#include <QGraphicsView>
#include <QJsonObject>

#include "../nodes/node.h"

WidgetNode::WidgetNode(Node* node, QWidget *parent) : node(node), QWidget(parent), Serializable() {
    initUI();

}

void WidgetNode::initUI() {
    //set layout
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    //
    // wdg_label = new QLabel("Some Title");
    // layout->addWidget(wdg_label);
    // layout->addWidget(new TextEdit("foo"));
}

void WidgetNode::addContent(vector<QWidget *> content) {
    for (auto c: content) {
        layout->addWidget(c);
    }
}


void WidgetNode::setEditingFlag(bool flag) {
    node->setEditingFlag(flag);
}

QJsonObject WidgetNode::serialize() {
    auto arr = QJsonObject{{"id", static_cast<int>(id)}};
    return arr;
}

bool WidgetNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) {
    return false;
}


