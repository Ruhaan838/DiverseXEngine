//
// Created by Ruhaan on 12/08/25.
//

#include "node.h"

#include <iostream>
#include <sstream>

#include "socket.h"
#include "edge.h"
#include "../scene/nodescene.h"
#include "../widgets/widgets.h"
#include "../serialization/serializator.h"
#include "../../ui/canvas/canvasScene.h"
#include "../../ui/canvas/canvasview.h"
#include "../../ui/graphics/nodeGraphics.h"

Node::Node(Scene *scene_, const  string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size) : scene(
    scene_), Serializable(), in_socket_type(input_size), out_socket_type(output_size), title(title) {

    grNode = nullptr;


}

void Node::initNode(string title, vector<SOCKETTYPES> in, vector<SOCKETTYPES> out) {
    grNode = new NodeGraphics(this);
    grNode->setTitle(title);

    scene->addNode(this);
    scene->grScene->addItem(grNode);

    if (pending_h > 0 && pending_w > 0)
        grNode->setHeightWidth(pending_h, pending_w);

    if (content != nullptr)
        grNode->initContent();

    //create socket for inputs
    int counter = 0;
    for (auto item : in) {
        auto *s = new SocketNode(this, counter, in_pos, item);
        counter++;
        inputs.emplace_back(s);
    }

    //create socket for outputs
    counter = 0;
    for (auto item : out) {
        auto *s = new SocketNode(this, counter, out_pos, item);
        counter++;
        outputs.emplace_back(s);
    }
}


 pair<int, int> Node::getSocketPos(int index, POSITION position) {
    int x = 0, y = 0;

    if (position == POSITION::LEFT_TOP || position == POSITION::LEFT_BOTTOM) {x = 0;}
    else {x = grNode->width;}
    if (position == POSITION::LEFT_BOTTOM || position == RIGHT_BOTTOM)
        y = grNode->height - grNode->edge_size - grNode->_padding - index * socket_spacing;
    else
        y = grNode->title_height + grNode->_padding + grNode->edge_size + index * socket_spacing;

    return {x, y};
}

void Node::setPos(int x, int y) {
    grNode->setPos(x, y);
}

QPointF Node::pos() const {
    return grNode->pos();
}

void Node::updateConnectedEdges() const {


    size_t max_size =  max(inputs.size(), outputs.size());

    for (size_t i = 0; i < max_size; i++) {
        if (i < inputs.size() && inputs[i]->hasEdge()) {
            inputs[i]->edge->updatePos();
        }

        if (i < outputs.size() && outputs[i]->hasEdge()) {
            outputs[i]->edge->updatePos();
        }
    }
}

string Node::str() {
     ostringstream oss;
    oss << "\t <Node " <<  hex << reinterpret_cast< uintptr_t>(this) << " title:" << grNode->getTitle().c_str() << ">";
    return oss.str();
}

void Node::setEditingFlag(bool flag) {
    auto grScene = scene->grScene;
    QList<QGraphicsView*> viewList = grScene->views();
    if (!viewList.isEmpty()) {
        if (CanvasView* canvasView = dynamic_cast<CanvasView*>(viewList[0])) {
            canvasView->editingFlag = flag;
        }
    }
}

QJsonObject Node::serialize() {
    QJsonArray in;
    for (auto i : inputs) {
        in.append(i->serialize());
    }

    QVector<QJsonObject> inVector;
    for (const auto &val : in) {
        inVector.append(val.toObject());
    }

    std::sort(inVector.begin(), inVector.end(), [](const QJsonObject &a, const QJsonObject &b) {
        int keyA = a.value("index").toInt() + a.value("position").toInt() * 10000;
        int keyB = b.value("index").toInt() + b.value("position").toInt() * 10000;
        return keyA < keyB;
    });

    in = QJsonArray();
    for (const auto &obj : inVector) {
        in.append(obj);
    }

    QJsonArray out;
    for (auto o : outputs) {
        out.append(o->serialize());
    }

    QJsonObject arr{
            {"id", static_cast<int>(id)},
            {"title", QString::fromStdString(grNode->getTitle())},
            {"x", this->pos().x()},
            {"y", this->pos().y()},
            {"inputs", in},
            {"outputs", out}
    };

    return arr;
}

bool Node::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t>& hashmap) {
    auto i = data.value("id");
    id = i.toInt();

    hashmap[std::to_string(i.toInt())] = reinterpret_cast<uintptr_t>(this);

    int pos_x = data["x"].toInt();
    int pos_y = data["y"].toInt();

    setPos(pos_x, pos_y);

    grNode->setTitle(data.value("title").toString().toStdString());
    auto in = data.value("inputs").toArray();
    auto out = data.value("outputs").toArray();

    inputs.clear();
    for (auto i: in) {
        auto new_i = i.toObject();
        const auto pos = getPosition(new_i.value("position").toInt());
        const auto item = getSocketType(new_i.value("socket_type").toInt());
        auto *sok = new SocketNode(this, new_i.value("index").toInt(), pos, item);
        sok->deserialize(new_i, hashmap);
        inputs.push_back(sok);
    }

    for (auto o: out) {
        auto new_o = o.toObject();
        const auto pos = getPosition(new_o.value("position").toInt());
        const auto item = getSocketType(new_o.value("socket_type").toInt());
        auto *sok = new SocketNode(this, new_o.value("index").toInt(), pos, item);
        sok->deserialize(new_o, hashmap);
        outputs.push_back(sok);
    }

    return true;
}

void Node::remove() {

    for (auto s: inputs) {
        if (s->hasEdge()) {
            // if (DEBUG) qDebug() << "\t\t Removing node" << s->str().c_str();
            s->edge->remove();
            s->edge = nullptr;
        }
    }

    for (auto s: outputs) {
        if (s->hasEdge()) {
            // if (DEBUG) qDebug() << "\t\t Removing Edge" << s->str().c_str();
            s->edge->remove();
            s->edge = nullptr;
        }
    }

    scene->grScene->removeItem(grNode);
    grNode = nullptr;
    scene->removeNode(this);
}

std::pair<int, int> Node::getHeightAndWidth() const {
    return grNode->getHeightAndWidth();
}

void Node::setContent(WidgetNode *content) {
    this->content = content;
}

void Node::setPosition(POSITION in_pos, POSITION out_pos) {
    this->in_pos = in_pos;
    this->out_pos = out_pos;
}

void Node::setHeightWidth(int h, int w) {
    pending_h = h;
    pending_w = w;
    if (grNode) {
        grNode->setHeightWidth(h, w);
    }
}

void Node::show() {
    if (!grNode) {
        initNode(title, in_socket_type, out_socket_type);
    }
}

