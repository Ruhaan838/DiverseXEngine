#include "printNode.h"
#include "../function/functionNode.h"
#include "inoutNode.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include "../../widgets/widgets.h"
#include <QJsonObject>
#include <QLineEdit>
#include <QDebug>
#include <QFont>
#include <cmath>

inline const char* print_node_stylesheet = R"(
    QLineEdit {
        background-color: #2b2b2b;
        color: #ffffff;
        border: 1px solid #3d3d3d;
        border-radius: 5px;
        padding: 2px 8px;
    }
    QLineEdit:focus {
        border: 2px solid #4a9eff;
    }
)";

PrintIfNode::PrintIfNode(Scene *scene_, const std::string &title, std::vector<QString> input_size, std::vector<QString> output_size)
: FunctionNode(scene_, title,
               (input_size.empty() ? std::vector<QString>{"Condition (Bool)"} : input_size),
               output_size,
               false /* allow_addsocket */) {
    showContent();
}

void PrintIfNode::showContent() {
    if (content) return;
    auto *widget = new WidgetNode(this);
    lineEdit = new QLineEdit(widget);

    QFont font("Arial", 16, QFont::Normal);
    lineEdit->setFont(font);
    lineEdit->setMinimumWidth(160);
    lineEdit->setMinimumHeight(28);
    lineEdit->setAlignment(Qt::AlignLeft);

    lineEdit->setStyleSheet(print_node_stylesheet);
    lineEdit->setText(message);

    QObject::connect(lineEdit, &QLineEdit::textChanged, [this](const QString &text){
        this->message = text;
    });

    widget->addContent({lineEdit});
    setContentHeight(70);
    setContent(widget);
}

void PrintIfNode::hideContent() {
    if (content) {
        content->hide();
        content = nullptr;
    }
}

void PrintIfNode::execute() {
    // Evaluate condition from input[0]
    bool cond = false;
    if (!inputs.empty()) {
        auto *s = inputs[0];
        if (s && s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
            Node* prev = s->getFirstEdge()->startSocket->node;
            if (prev) {
                long double v = 0.0L;
                if (auto *fn = dynamic_cast<FunctionNode*>(prev)) v = fn->getNodeValue(fn, s->getFirstEdge()->startSocket);
                else if (auto *in = dynamic_cast<InputNode*>(prev)) v = in->vals;
                cond = std::fabs(static_cast<double>(v)) > 1e-9;
            }
        }
    }
    lastActive = cond;
    if (lastActive) {
        // Print to Qt debug/console
        qDebug() << "PrintIf:" << (message.isEmpty() ? QString("") : message);
    }
}

QJsonObject PrintIfNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "PrintIfNode";
    obj["message"] = message;
    obj["active"] = lastActive;
    return obj;
}

bool PrintIfNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    bool ok = FunctionNode::deserialize(data, hashmap);
    message = data.value("message").toString();
    lastActive = data.value("active").toBool(false);
    if (lineEdit) lineEdit->setText(message);
    return ok;
}

void registerPrintNodeType() {
    Node::registerType("PrintIfNode", [](Scene* scene){ return new PrintIfNode(scene); });
}
