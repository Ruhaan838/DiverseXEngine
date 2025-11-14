//
// Created by Ruhaan on 19/08/25.
//

#include "inoutNode.h"
#include "../function/functionNode.h"
#include "../../scene/nodescene.h"
#include "../../widgets/widgets.h"
#include "../../nodes/socket.h"
#include "../../nodes/node.h"

#include <QLineEdit>
#include <QDoubleValidator>

using namespace std;

inline const string in_node_stylesheet = R"(
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


InputNode::InputNode(Scene *scene_, const string &title, vector<QString> output_size)
: Node(scene_, title, {}, output_size){
    auto *widget = new WidgetNode(this);
    lineEdit = new QLineEdit(widget);

    QFont font("Arial", 20, QFont::Normal);
    lineEdit->setFont(font);
    lineEdit->setMinimumWidth(120);
    lineEdit->setMinimumHeight(30);
    lineEdit->setAlignment(Qt::AlignCenter);

    lineEdit->setStyleSheet(
        in_node_stylesheet.c_str()
    );

    lineEdit->setValidator(new QDoubleValidator(lineEdit));
    widget->addContent({lineEdit});
    setContent(widget);

    QObject::connect(lineEdit, &QLineEdit::textChanged, [this](const QString &text) {
        bool ok = false;
        long double value = text.toDouble(&ok);
        if (ok) {
            this->vals = value;
        } else {
            this->vals = 0;
        }
    });
}

QJsonObject InputNode::serialize() {
    auto old_obj = Node::serialize();
    old_obj["value"] = vals;
    QJsonArray out;
    for (auto o : outputs) {
        out.append(o->serialize());
    }
    old_obj["outputs"] = out;
    old_obj["node_type"] = "InputNode";
    auto hw = this->getHeightAndWidth();
    old_obj["h"] = hw.first;
    old_obj["w"] = hw.second;
    return old_obj;
}

bool InputNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    auto h = data.value("h").toInt();
    auto w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();
    auto old_obj = Node::deserialize(data, hashmap);

    auto val = data.value("value").toDouble();
    vals = val;
    if (lineEdit) {
        lineEdit->setText(std::to_string(vals).c_str());
    }

    auto out = data.value("outputs").toArray();
    for (auto s : outputs) { delete s; }
    outputs.clear();
    for (auto o: out) {
        auto new_o = o.toObject();
        const auto pos = getPosition(new_o.value("position").toInt());
        const QString socketType = new_o.value("socket_type").toString();
        auto *sok = new SocketNode(this, new_o.value("index").toInt(), pos, socketType);
        sok->deserialize(new_o, hashmap);
        outputs.push_back(sok);
    }
    return true && old_obj;
}

void registerInputNodeType() {
    Node::registerType("InputNode", [](Scene* scene) { return new InputNode(scene); });
}
