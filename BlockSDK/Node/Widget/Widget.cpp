//
// Created by Ruhaan on 29/07/25.
//

#include "Widget.h"

#include <QGraphicsView>

#include "../Node.h"


WidgetNode::WidgetNode(Node* node, QWidget *parent) : node(node), QWidget(parent) {
    initUI();

}

void WidgetNode::initUI() {
    //set layout
    layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    wdg_label = new QLabel("Some Title");
    layout->addWidget(wdg_label);
    layout->addWidget(new TextEdit("foo"));
}

void WidgetNode::setEditingFlag(bool flag) {
    node->setEditingFlag(flag);
}
