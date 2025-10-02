#include "canvasNode.h"
#include "../nodes/socket.h"
#include "../nodes/edge.h"
#include "../../ui/graphics/canvasNodeGraphis.h"
#include "../../ui/graphics/socketGraphics.h"
#include "../scene/nodescene.h"
#include "../../ui/canvas/canvasScene.h"
#include <algorithm>
#include <QPointF>
#include "../../ui/graphics/nodeGraphics.h"
#include <QDebug>
#include <QTimer>

CanvasNode::CanvasNode(Scene* scene_, const std::string &title, std::vector<QString> input_size, std::vector<QString> output_size)
: Node(scene_, title, input_size, output_size) {
    setPosition(LEFT_TOP, RIGHT_TOP);
    setHeightWidth(200, 300);
}

void CanvasNode::show() {
    if (grNode) return;

    grNode = new CanvasNodeGraphics(this);
    grNode->setTitle("Canvas");

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

    int counter = 0;
    for (auto item : in_socket_type) {
        auto *s = new SocketNode(this, counter, in_pos, item);
        counter++;
        inputs.emplace_back(s);
    }

    counter = 0;
    for (auto item : out_socket_type) {
        auto *s = new SocketNode(this, counter, out_pos, item);
        counter++;
        outputs.emplace_back(s);
    }
}

void CanvasNode::addNode(Node* node) {
    if (!node || !grNode) return;

    {
        auto titleDbg = node->grNode ? QString::fromStdString(node->grNode->getTitle()) : QString::fromStdString(node->str());
    }

    if (!node->grNode) node->show();

    QRectF nodeSceneRect = node->grNode->mapToScene(node->grNode->boundingRect()).boundingRect();
    QPointF nodeTopLeftScene = nodeSceneRect.topLeft();

    node->grNode->setParentItem(grNode);
    QPointF localPos = grNode->mapFromScene(nodeTopLeftScene);
    node->grNode->setPos(localPos);

    for (auto *s : node->inputs) {
        if (!s || !s->grSocket) continue;
        s->grSocket->setParentItem(nullptr);
        s->grSocket->setParentItem(node->grNode);
        auto pos = node->getSocketPos(s->index, s->position);
        s->grSocket->setPos(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second));
        s->grSocket->setZValue(1);
        s->grSocket->update();
    }
    for (auto *s : node->outputs) {
        if (!s || !s->grSocket) continue;
        s->grSocket->setParentItem(nullptr);
        s->grSocket->setParentItem(node->grNode);
        auto pos = node->getSocketPos(s->index, s->position);
        s->grSocket->setPos(static_cast<qreal>(pos.first), static_cast<qreal>(pos.second));
        s->grSocket->setZValue(1);
        s->grSocket->update();
    }

    node->refreshSocketsAndEdges();

    QTimer::singleShot(0, [node]() { if (node) node->refreshSocketsAndEdges(); });
    QTimer::singleShot(20, [node]() { if (node) node->refreshSocketsAndEdges(); });
    QTimer::singleShot(50, [node]() { if (node) node->refreshSocketsAndEdges(); });

    QTimer::singleShot(0, [node]() {
        if (node) node->refreshSocketsAndEdges();
    });

    if (std::find(inner_nodes.begin(), inner_nodes.end(), node) == inner_nodes.end())
        inner_nodes.push_back(node);

    updateSizeToFitChildren();

    QTimer::singleShot(0, [this, node]() {
        if (node) node->refreshSocketsAndEdges();
        if (this->scene && this->scene->grScene) this->scene->grScene->update();
    });

}

void CanvasNode::removeNode(Node* node) {
    if (!node || !grNode) return;

    auto it = std::find(inner_nodes.begin(), inner_nodes.end(), node);
    if (it == inner_nodes.end()) return;

    QPointF localPos = node->grNode->pos();
    QPointF scenePos = grNode->mapToScene(localPos);

    node->grNode->setParentItem(nullptr);
    node->grNode->setPos(scenePos);

    QTimer::singleShot(0, [node]() {
        if (node) node->refreshSocketsAndEdges();
    });

    inner_nodes.erase(it);

    updateSizeToFitChildren();

    for (auto* n : inner_nodes) if (n) n->refreshSocketsAndEdges();
}

void CanvasNode::updateSizeToFitChildren() {
    if (!grNode) return;

    const int minW = 120;
    const int minH = 80;
    if (inner_nodes.empty()) {
        setHeightWidth(minH, minW);
        return;
    }

    qreal minX = std::numeric_limits<qreal>::max();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    for (auto* n : inner_nodes) {
        if (!n || !n->grNode) continue;
        QPointF p = n->grNode->pos();
        auto hw = n->getHeightAndWidth();
        qreal w = static_cast<qreal>(hw.second);
        qreal h = static_cast<qreal>(hw.first);
        minX = std::min(minX, p.x());
        minY = std::min(minY, p.y());
        maxX = std::max(maxX, p.x() + w);
        maxY = std::max(maxY, p.y() + h);
    }

    if (minX == std::numeric_limits<qreal>::max()) return;

    qreal shiftX = 0;
    qreal shiftY = 0;
    if (minX < child_padding) shiftX = child_padding - minX;
    if (minY < child_padding) shiftY = child_padding - minY;

    if (shiftX != 0 || shiftY != 0) {
        for (auto* n : inner_nodes) {
            if (!n || !n->grNode) continue;
            QPointF p = n->grNode->pos();
            n->grNode->setPos(p + QPointF(shiftX, shiftY));
            QTimer::singleShot(0, [n]() { if (n) n->refreshSocketsAndEdges(); });
        }
        minX += shiftX; minY += shiftY; maxX += shiftX; maxY += shiftY;
    }

    int newW = static_cast<int>(std::max<qreal>(maxX + child_padding, minW));
    int newH = static_cast<int>(std::max<qreal>(maxY + child_padding + grNode->title_height, minH));

    setHeightWidth(newH, newW);

    QTimer::singleShot(0, [this]() {
        for (auto* n : inner_nodes) {
            if (!n || !n->grNode) continue;
            n->grNode->update();
            n->refreshSocketsAndEdges();
        }
        if (this->scene && this->scene->grScene) this->scene->grScene->update();
    });
}

void CanvasNode::tryEmbedSelectedNodes() {
    if (!grNode || !grNode->scene()) return;

    auto items = grNode->scene()->selectedItems();
    if (items.isEmpty()) return;

    QRectF canvasSceneRect = grNode->mapToScene(grNode->boundingRect()).boundingRect();

    for (auto *it : items) {
        if (it == grNode) continue;

        auto *ng = dynamic_cast<NodeGraphics*>(it);
        if (!ng) continue;
        if (ng->parentItem() == grNode) continue;

        if (ng->parentItem() != nullptr) continue;

        QRectF nodeSceneRect = ng->mapToScene(ng->boundingRect()).boundingRect();
        QPointF nodeCenter = nodeSceneRect.center();

        bool shouldEmbed = false;
        if (canvasSceneRect.contains(nodeCenter)) {
            shouldEmbed = true;
        } else {
            QRectF inter = canvasSceneRect.intersected(nodeSceneRect);
            if (!inter.isEmpty()) {
                qreal interArea = inter.width() * inter.height();
                qreal nodeArea = nodeSceneRect.width() * nodeSceneRect.height();
                if (nodeArea > 0 && (interArea / nodeArea) >= 0.5) shouldEmbed = true;
            }
        }

        if (shouldEmbed && ng->node) addNode(ng->node);
    }
}

void registerCanvasNodeType() {
    Node::registerType("CanvasNode", [](Scene* scene){ return new CanvasNode(scene); });
}
