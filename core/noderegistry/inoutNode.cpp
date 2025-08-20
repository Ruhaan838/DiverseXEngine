//
// Created by Ruhaan on 19/08/25.
//

#include "inoutNode.h"
#include "../scene/nodescene.h"
#include "../widgets/widgets.h"
#include "../nodes/socket.h"
#include "../nodes/edge.h"

#include <QLineEdit>
#include <QDoubleValidator>

#include "functionNode.h"

InputNode::InputNode(Scene *scene_, const string &title, vector<SOCKETTYPES> output_size)
: Node(scene_, title, {}, output_size){
    auto *widget = new WidgetNode(this);
    auto *lineEdit = new QLineEdit(widget);
    lineEdit->setValidator(new QDoubleValidator(lineEdit));
    widget->addContent({lineEdit});
    setContent(widget);
    QObject::connect(lineEdit, &QLineEdit::textChanged, [this, lineEdit](const QString &text) {
        bool ok = false;
        long double value = text.toDouble(&ok);
        if (ok) {
            this->vals = value;
        } else {
            this->vals = 0;
        }
    });
}

OutputNode::OutputNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size)
: Node(scene_, title, input_size, {}) {
auto *widget = new WidgetNode(this);
    textEdit = new TextEdit(std::to_string(vals).c_str());
    textEdit->setReadOnly(true);
    widget->addContent({textEdit});
    setContent(widget);
}

void OutputNode::execute() {
    if (inputs.size() >= 1 && inputs[0] && inputs[0]->hasEdge() && inputs[0]->edge && inputs[0]->edge->startSocket) {
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
