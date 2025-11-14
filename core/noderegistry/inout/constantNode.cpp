
#include "constantNode.h"
#include "inoutNode.h"
#include "../../scene/nodescene.h"
#include "../../widgets/widgets.h"
#include "../../nodes/socket.h"
#include "../../nodes/node.h"

#include <QLineEdit>
#include <QDoubleValidator>

using namespace std;

inline const string const_node_stylesheet = R"(
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

ConstantNode::ConstantNode(Scene *scene_, const std::string &title, std::vector<QString> output_size)
: Node(scene_, title, {}, output_size) {
    auto *widget = new WidgetNode(this);
    lineEdit = new QLineEdit(widget);

    QFont font("Arial", 20, QFont::Normal);
    lineEdit->setFont(font);
    lineEdit->setMinimumWidth(120);
    lineEdit->setMinimumHeight(30);
    lineEdit->setAlignment(Qt::AlignCenter);

    lineEdit->setStyleSheet(const_node_stylesheet.c_str());

    lineEdit->setValidator(new QDoubleValidator(lineEdit));
    lineEdit->setText("0");
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

QJsonObject ConstantNode::serialize() {
    auto old_obj = Node::serialize();
    old_obj["value"] = static_cast<double>(vals);
    QJsonArray out;
    for (auto o : outputs) {
        out.append(o->serialize());
    }
    old_obj["outputs"] = out;
    old_obj["node_type"] = "ConstantNode";
    auto hw = this->getHeightAndWidth();
    old_obj["h"] = hw.first;
    old_obj["w"] = hw.second;
    return old_obj;
}

bool ConstantNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    auto h = data.value("h").toInt();
    auto w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();
    auto old_obj = Node::deserialize(data, hashmap);

    auto val = data.value("value").toDouble();
    vals = val;
    if (lineEdit) {
        lineEdit->setText(QString::number(static_cast<double>(vals)));
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

