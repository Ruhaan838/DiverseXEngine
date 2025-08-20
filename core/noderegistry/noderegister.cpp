//
// Created by Ruhaan on 18/08/25.
//

#include "noderegister.h"

#include "../nodes/node.h"
#include "../scene/nodescene.h"

NodeRegistery::NodeRegistery(Scene* parent) : scene(parent) {
    setHeaderHidden(true);

    addFunctions();
    addInputs();
    addOutputs();

    connect(this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int column) {
        auto text = item->text(0);

        auto nodes = getfunctionalNode();
        auto inputs = getInputNode();
        auto outputs = getOutputNode();

        if (nodes.contains(text)) {
            auto *node = nodes.value(text);
            scene->addNode(node);
            node->show();
        } else if (text == "Input") {
            auto *node = inputs.at(0);
            scene->addNode(node);
            node->show();
        } else if (text == "Output") {
            auto *node = outputs.at(0);
            scene->addNode(node);
            node->show();
        }

    });
}

void NodeRegistery::addFunctions() {
    functions = new QTreeWidgetItem(this);
    functions->setText(0, "Functions");

    QStringList funcs = {"Add", "Subtract", "Multiply", "Divide"};
    for (const QString& f : funcs) {
        auto* item = new QTreeWidgetItem(functions);
        item->setText(0, f);
    }
}

void NodeRegistery::addInputs() {
    inputs = new QTreeWidgetItem(this);
    inputs->setText(0, "Inputs");

    QStringList input = {"Input"};
    for (const QString& i : input) {
        auto* item = new QTreeWidgetItem(inputs);
        item->setText(0, i);
    }
}

void NodeRegistery::addOutputs() {
    outputs = new QTreeWidgetItem(this);
    outputs->setText(0, "Outputs");

    QStringList output = {"Output"};
    for (const QString& o : output) {
        auto item = new QTreeWidgetItem(outputs);
        item->setText(0, o);
    }
}
