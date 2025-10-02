//
// Created by Ruhaan on 12/08/25.
//

#include "node.h"

#include <iostream>
#include <sstream>
#include <QDebug>

#include "socket.h"
#include "edge.h"
#include "../scene/nodescene.h"
#include "../widgets/widgets.h"
#include "../serialization/serializator.h"
#include "../../ui/canvas/canvasScene.h"
#include "../../ui/canvas/canvasview.h"
#include "../../ui/graphics/nodeGraphics.h"
#include "../../ui/graphics/socketGraphics.h"

Node::Node(Scene *scene_, const  string &title, vector<QString> input_size, vector<QString> output_size) : scene(
    scene_), Serializable(), in_socket_type(input_size), out_socket_type(output_size), title(title) {
    grNode = nullptr;
}

void Node::initNode(string title, vector<QString> in, vector<QString> out) {
    grNode = new NodeGraphics(this);
    grNode->setTitle(title);

    if (scene) {
        scene->addNode(this);
        if (scene->grScene) {
            scene->grScene->addItem(grNode);
        }
    }

    if (pending_h > 0 && pending_w > 0 && grNode)
        grNode->setHeightWidth(pending_h, pending_w);

    if (content != nullptr && grNode)
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
    if (!grNode) return {0,0};

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
    if (grNode) grNode->setPos(x, y);
}

QPointF Node::pos() const {
    if (grNode) return grNode->pos();
    return QPointF(0,0);
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

void Node::refreshSocketsAndEdges() {

    for (size_t i = 0; i < inputs.size(); i++) {
        if (inputs[i] && inputs[i]->grSocket) {
            auto pos = getSocketPos(static_cast<int>(i), in_pos);
            if (inputs[i]->grSocket->parentItem() == grNode) {
                inputs[i]->grSocket->setPos(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second));
            } else if (grNode) {
                QPointF scenePos = grNode->mapToScene(QPointF(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second)));
                inputs[i]->grSocket->setPos(scenePos);
            } else {
                inputs[i]->grSocket->setPos(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second));
            }
        }
    }

    for (size_t i = 0; i < outputs.size(); i++) {
        if (outputs[i] && outputs[i]->grSocket) {
            auto pos = getSocketPos(static_cast<int>(i), out_pos);
            if (outputs[i]->grSocket->parentItem() == grNode) {
                outputs[i]->grSocket->setPos(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second));
            } else if (grNode) {
                QPointF scenePos = grNode->mapToScene(QPointF(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second)));
                outputs[i]->grSocket->setPos(scenePos);
            } else {
                outputs[i]->grSocket->setPos(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second));
            }
        }
    }

    updateConnectedEdges();
}

string Node::str() {
     ostringstream oss;
    oss << "\t <Node " <<  hex << reinterpret_cast< uintptr_t>(this);
    if (grNode) oss << " title:" << grNode->getTitle().c_str();
    else oss << " title:" << title.c_str();
    oss << ">";
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
    QJsonObject arr{
            {"id", static_cast<int>(id)},
            {"title", QString::fromStdString(grNode ? grNode->getTitle() : title)},
            {"x", this->pos().x()},
            {"y", this->pos().y()}
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
    if (grNode)
        grNode->setTitle(data.value("title").toString().toStdString());
    else
        title = data.value("title").toString().toStdString();

    return true;
}

void Node::remove() {

    for (auto s: inputs) {
        if (s->hasEdge()) {
            s->edge->remove();
            s->edge = nullptr;
        }
    }

    for (auto s: outputs) {
        if (s->hasEdge()) {
            s->edge->remove();
            s->edge = nullptr;
        }
    }

    if (scene) {
        scene->removeNode(this);
    }

    if (grNode && scene && scene->grScene) {
        scene->grScene->removeItem(grNode);
        delete grNode;
        grNode = nullptr;
    }

    if (content)
        content = nullptr;

}

std::pair<int, int> Node::getHeightAndWidth() const {
    if (grNode) return grNode->getHeightAndWidth();
    return {pending_h > 0 ? pending_h : 0, pending_w > 0 ? pending_w : 0};
}

void Node::setContent(WidgetNode *content) {
    this->content = content;
    if (grNode) grNode->initContent();
}

void Node::setPosition(POSITION in_pos, POSITION out_pos) {
    this->in_pos = in_pos;
    this->out_pos = out_pos;
}

void Node::addInputSocket(int insertIndex, const QString& label) {

    if (insertIndex < 0) insertIndex = 0;
    if (insertIndex > static_cast<int>(inputs.size())) insertIndex = static_cast<int>(inputs.size());

    QString resolvedLabel = label;
    if (resolvedLabel.isEmpty()) {
        int maxNum = 0;
        for (auto *s : inputs) {
            if (!s) continue;
            if (s->socket_type == "addsocket") continue;
            QString t = s->socket_type;

            if (t.startsWith("Number")) {
                QString tail = t.mid(QString("Number").length()).trimmed();
                bool ok = false;
                int n = tail.toInt(&ok);
                if (ok && n > maxNum) maxNum = n;
            }
        }
        resolvedLabel = QString("Number %1").arg(maxNum + 1);
    }


    auto *newSock = new SocketNode(this, insertIndex, in_pos, resolvedLabel);
    inputs.insert(inputs.begin() + insertIndex, newSock);

    for (size_t i = 0; i < inputs.size(); ++i) {
        if (inputs[i]) inputs[i]->index = static_cast<int>(i);
    }

    if (grNode) {
        int maxSockets = static_cast<int>(std::max(inputs.size(), outputs.size()));
        int neededHeight = grNode->title_height + static_cast<int>(2 * grNode->edge_size) + grNode->_padding * 2 + maxSockets * socket_spacing;
        auto hw = getHeightAndWidth();
        int curH = hw.first;
        int newWidth = hw.second;

        const int widthThreshold = 6;
        if (maxSockets > widthThreshold) {
            newWidth = hw.second + (maxSockets - widthThreshold) * 12;
        }
        if (neededHeight > curH || newWidth != hw.second) {
            setHeightWidth(neededHeight > curH ? neededHeight : curH, newWidth);
        }
        refreshSocketsAndEdges();
    }
}

void Node::setHeightWidth(int h, int w) {
    pending_h = h;
    pending_w = w;
    if (grNode) {
        grNode->setHeightWidth(h, w);
        refreshSocketsAndEdges();
    }
}

void Node::setContentHeight(int h) {
    pending_h = h;
    if (grNode) {
        grNode->setHeightWidth(h, grNode->width);
    }
}

void Node::show() {
    if (!grNode) {
        initNode(title, in_socket_type, out_socket_type);
    }
}

std::unordered_map<QString, std::function<Node*(Scene*)>>& Node::registry() {
    static std::unordered_map<QString, std::function<Node*(Scene*)>> instance;
    return instance;
}

void Node::registerType(const QString& type, function<Node*(Scene*)> creator) {
    registry()[type] = creator;
}

Node* Node::createNode(const QString& type, Scene* scene) {
    auto it = registry().find(type);
    if (it != registry().end()) {
        return it->second(scene);
    }
    return new Node(scene);
}

void Node::removeLastInputSocket() {

    if (inputs.empty()) return;

    int lastIdx = -1;
    int nonAddCount = 0;
    for (int i = 0; i < static_cast<int>(inputs.size()); ++i) {
        if (!inputs[i]) continue;
        if (inputs[i]->socket_type == "addsocket") continue;

        nonAddCount++;
        lastIdx = i;
    }

    if (nonAddCount <= 2) {
        setInfoText("Not removing socket: minimum 1 input required");
        qDebug() << "Not removing socket: minimum 1 input required";
        return;
    }

    if (lastIdx < 0 || lastIdx >= static_cast<int>(inputs.size())) return;

    auto *s = inputs[lastIdx];
    if (!s) return;

    if (s->hasEdge() && s->edge) {
        s->edge->remove();
        s->edge = nullptr;
    }

    delete s;
    inputs.erase(inputs.begin() + lastIdx);

    for (size_t i = 0; i < inputs.size(); ++i) {
        if (inputs[i]) inputs[i]->index = static_cast<int>(i);
    }

    if (grNode) {
        int maxSockets = static_cast<int>(std::max(inputs.size(), outputs.size()));
        int neededHeight = grNode->title_height + static_cast<int>(2 * grNode->edge_size) + grNode->_padding * 2 + maxSockets * socket_spacing;
        auto hw = getHeightAndWidth();
        int curH = hw.first;
        int newWidth = hw.second;
        const int widthThreshold = 6;
        if (maxSockets > widthThreshold) {

            int shrink = (maxSockets - widthThreshold) * 12;
            if (hw.second - shrink > grNode->width) newWidth = hw.second - shrink;
            else newWidth = grNode->width;
        }
        if (neededHeight != curH || newWidth != hw.second) {
            setHeightWidth(neededHeight, newWidth);
        }
        refreshSocketsAndEdges();
    }
}

void Node::setInfoText(const std::string &text) {
    if (grNode) {
        grNode->setInfoText(text);
    }
}