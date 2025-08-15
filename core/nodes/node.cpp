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

Node::Node(Scene *scene_, const  string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size) : scene(scene_), Serializable() {

    content = new WidgetNode(this);

    grNode = new NodeGraphics(this);
    grNode->setTitle(title);

    scene->addNode(this);
    scene->grScene->addItem(grNode);

    //create the socket for input
    int counter = 0;
    for (auto item : input_size) {
        auto *s = new SocketNode(this, counter, POSITION::LEFT_BOTTOM, item);
        counter++;
        inputs.emplace_back(s);
    }

    //create the socket for output
    counter = 0;
    for (auto item : output_size) {
        auto *s = new SocketNode(this, counter, POSITION::RIGHT_TOP, item);
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
    x_pos = x; y_pos = y;
    grNode->setPos(x, y);
}

pair<int, int> Node::Pos() {
    return {x_pos, y_pos};
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
    QJsonObject arr;
    QJsonArray in, out;

    auto pos = Pos();

    for (auto i: inputs) {
        in.append(i->serialize());
    }
    for (auto o:outputs) {
        out.append(o->serialize());
    }

    arr = QJsonObject{
            {"id", static_cast<int>(id)},
            { "title", QString::fromStdString(grNode->getTitle())},
            {"x", pos.first},
            {"y", pos.second},
            {"inputs", in},
            {"outputs", out},
            {"contents", content->serialize()}
    };

    return arr;
}

bool Node::deserialize(const QJsonObject &data, unordered_map<string, int> hashmap) {
    return false;
}
