//
// Created by Ruhaan on 12/08/25.
//

#include "nodeGraphics.h"

#include <iostream>
#include <QDebug>
#include <QPainter>
#include <QVariant>
#include <QTimer>
#include <QGraphicsSceneMouseEvent>
#include <QAbstractTextDocumentLayout>

#include "canvasNodeGraphis.h"
#include "../../core/nodes/node.h"
#include "../../core/widgets/widgets.h"
#include "../canvas/canvasScene.h"
#include "../../core/scene/nodescene.h"
#include "../../core/noderegistry/canvasNode.h"
#include "../../core/nodes/socket.h"
#include "../../core/nodes/edge.h"
#include "InfoButton.h"

inline bool DEBUG = false;


NodeGraphics::NodeGraphics(Node *node, QGraphicsItem *parent) : QGraphicsItem(parent), node(node) {

    _title_color = Qt::white;
    _title_font = QFont("Ubuntu", 12);
    _pen_default =  QPen(QColor("#7F000000"));
    _pen_selected =  QPen(QColor("#FFFFA637"));

    _title_brush = QBrush(QColor("#FF313131"));
    _bg_brush = QBrush(QColor("#E3212121"));

    initUI();
    initTitle();
    setTitle("Undefine Node");
}

void NodeGraphics::initUI() {
    setFlags(QGraphicsItem::ItemIsSelectable |
        QGraphicsItem::ItemIsMovable |
        QGraphicsItem::ItemSendsScenePositionChanges);
}

void NodeGraphics::initTitle() {
    title_item = new QGraphicsTextItem(this);
    title_item->setDefaultTextColor(_title_color);
    title_item->setFont(_title_font);
    if (title_item->document()) title_item->document()->setDocumentMargin(0);

    QFontMetricsF fm(_title_font);
    qreal textH = fm.height();
    int desired = int(textH) + 8;
    if (title_height < desired) title_height = desired;

    int infoDiameter = std::max(9, (int)(title_height * 0.6));
    title_item->setTextWidth(width - infoDiameter - (4 * _padding));

    QRectF br = title_item->boundingRect();
    qreal brTop = br.top();
    qreal brH = br.height();
    qreal titleY = (title_height - brH) / 2.0 - brTop;
    title_item->setPos(_padding, titleY);

    if (!infoButton) {
        infoButton = new InfoButton(this, infoDiameter);
        infoButton->setFlag(QGraphicsItem::ItemIsSelectable, false);
        infoButton->setFlag(QGraphicsItem::ItemIsMovable, false);
        infoButton->setAcceptedMouseButtons(Qt::LeftButton);
        updateInfoButtonPos();
    }
}

void NodeGraphics::initContent() {
    content = node->content;
    grContent = new QGraphicsProxyWidget(this);
    content->setGeometry(edge_size, title_height + edge_size,
        width - 2 * edge_size, height - 2 * edge_size - title_height);
    grContent->setWidget(content);
}

void NodeGraphics::setTitle(const std::string& title) {
    _title = title;
    if (title_item) {
        title_item->setPlainText(QString::fromStdString(_title));
        if (title_item->document()) title_item->document()->setDocumentMargin(0);

        int infoDiameter = infoButton ? infoButton->rect().width() : std::max(9, (int)(title_height * 0.6));
        title_item->setTextWidth(width - infoDiameter - (4 * _padding));

        qreal textH = 0;
        if (title_item->document() && title_item->document()->documentLayout())
            textH = title_item->document()->documentLayout()->documentSize().height();
        if (textH <= 0) textH = title_item->boundingRect().height();

        int desired = int(textH) + 8;
        if (title_height < desired) title_height = desired;

        QRectF br = title_item->boundingRect();
        qreal brTop = br.top();
        qreal brH = br.height();
        qreal titleY = (title_height - brH) / 2.0 - brTop;
        title_item->setPos(_padding, titleY);
    }
    updateInfoButtonPos();
}

std::string NodeGraphics::getTitle() const {
    return _title;
}

std::string NodeGraphics::getTitle() {
    return _title;
}

void NodeGraphics::setInfoText(const std::string &text) {
    if (!infoPopup) {
        infoPopup = new QGraphicsTextItem(this);
        infoPopupBg = new QGraphicsRectItem(this);
        infoPopup->setDefaultTextColor(Qt::white);
        QFont f("Ubuntu", 10);
        infoPopup->setFont(f);
        infoPopupBg->setBrush(QBrush(QColor(0,0,0,200)));
        infoPopupBg->setPen(Qt::NoPen);
        infoPopup->setZValue(1000);
        infoPopupBg->setZValue(999);
        infoPopup->setVisible(false);
        infoPopupBg->setVisible(false);
    }

    infoPopup->setPlainText(QString::fromStdString(text));
    QRectF tr = infoPopup->boundingRect();
    qreal pad = 6;
    infoPopupBg->setRect(0, 0, tr.width() + pad * 2, tr.height() + pad * 2);

    qreal by = title_height + 6;
    qreal popupW = infoPopupBg->rect().width();
    qreal popupX = _padding;
    if (infoButton) {
        qreal bx = infoButton->pos().x();
        qreal bW = infoButton->rect().width();

        popupX = bx + bW / 2.0 - popupW / 2.0;
    }

    if (popupX < _padding) popupX = _padding;
    if (popupX + popupW > width - _padding) popupX = width - _padding - popupW;

    infoPopupBg->setPos(popupX, by);
    infoPopup->setPos(infoPopupBg->pos().x() + pad, infoPopupBg->pos().y() + pad);
}

void NodeGraphics::toggleInfoPopup() {

    if (!infoPopup) {
        setInfoText("(no info)");
    }
    bool vis = infoPopup->isVisible();
    infoPopup->setVisible(!vis);
    infoPopupBg->setVisible(!vis);

    if (!vis) {
        infoPopup->setZValue(10000);
        infoPopupBg->setZValue(9999);
    }
}

void NodeGraphics::updateInfoButtonPos() {
    if (!infoButton) return;
    qreal infoDiameter = infoButton->rect().width();
    qreal by = (title_height - infoDiameter) / 2.0;

    qreal inset = 6.0;
    qreal bx = width - inset - infoDiameter;
    if (title_item) {
        qreal titleRight = title_item->pos().x() + title_item->textWidth();
        if (bx < titleRight + _padding) bx = titleRight + _padding;
    }

    if (bx + infoDiameter > width - _padding) bx = width - _padding - infoDiameter;
    if (bx < _padding) bx = _padding;
    infoButton->setParentItem(this);
    infoButton->setPos(bx, by);

     if (infoPopup && infoPopupBg) {
         QString currentText = infoPopup->toPlainText();
         setInfoText(currentText.toStdString());
     }
}

QRectF NodeGraphics::boundingRect() const {
    return QRectF(0, 0, width, height).normalized();
}

void NodeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //title
    auto path_title = QPainterPath();
    path_title.setFillRule(Qt::WindingFill);
    path_title.addRoundedRect(0, 0, width, title_height, edge_size, edge_size);
    path_title.addRect(0, title_height - edge_size, edge_size, edge_size);
    path_title.addRect(width - edge_size, title_height - edge_size, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_title_brush);
    painter->drawPath(path_title.simplified());

    //content
    auto path_content = QPainterPath();
    path_content.setFillRule(Qt::WindingFill);
    auto hw = getHeightAndWidth();
    path_content.addRoundedRect(0, title_height, hw.second, hw.first - title_height, edge_size, edge_size);
    path_content.addRect(0, title_height, edge_size, edge_size);
    path_content.addRect(hw.second - edge_size, title_height, edge_size, edge_size);
    painter->setPen(Qt::NoPen);
    painter->setBrush(_bg_brush);
    painter->drawPath(path_content.simplified());

    // outline
    auto path_outline = QPainterPath();
    path_outline.addRoundedRect(0, 0, hw.second, hw.first, edge_size, edge_size);
    if (!isSelected())
        painter->setPen(_pen_default);
    else
        painter->setPen(_pen_selected);

    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path_outline.simplified());
}

void NodeGraphics::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);

    if (scene()) {
        auto* nodeScene = dynamic_cast<CanvasScene*>(scene());
        if (nodeScene && nodeScene->scene) {
            for (auto* sceneNode : nodeScene->scene->nodes) {
                if (sceneNode->grNode->isSelected()) {
                    sceneNode->updateConnectedEdges();
                }
            }
        }
    }
}

void NodeGraphics::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
    if (!scene()) return;

    QRectF nodeSceneRect = mapToScene(boundingRect()).boundingRect();
    qreal bestArea = 0.0;
    CanvasNodeGraphics* bestCanvas = nullptr;

    for (QGraphicsItem* it : scene()->items()) {
        auto* canvas = dynamic_cast<CanvasNodeGraphics*>(it);
        if (!canvas) continue;
        if (canvas == this) continue;

        QRectF canvasSceneRect = canvas->mapToScene(canvas->boundingRect()).boundingRect();
        QRectF inter = canvasSceneRect.intersected(nodeSceneRect);
        if (inter.isEmpty()) continue;
        qreal area = inter.width() * inter.height();
        if (area > bestArea) {
            bestArea = area;
            bestCanvas = canvas;
        }
    }

    if (bestCanvas && bestArea > 0) {
        QPointF nodeCenter = nodeSceneRect.center();
        QRectF canvasRect = bestCanvas->mapToScene(bestCanvas->boundingRect()).boundingRect();
        bool shouldEmbed = false;
        if (canvasRect.contains(nodeCenter)) shouldEmbed = true;
        else {
            qreal nodeArea = nodeSceneRect.width() * nodeSceneRect.height();
            if (nodeArea > 0 && (bestArea / nodeArea) >= 0.25) shouldEmbed = true;
        }

        if (shouldEmbed) {
            if (node && dynamic_cast<Node*>(node)) {
                Node* n = node;
                if (n->grNode && n->grNode->parentItem() == bestCanvas) return;
                if (bestCanvas->node) {
                    auto* canvasNode = dynamic_cast<CanvasNode*>(bestCanvas->node);
                    if (canvasNode) {
                        canvasNode->addNode(n);
                        if (n->grNode) n->grNode->setSelected(true);
                    }
                }
            }
        }
    }
}

std::pair<int, int> NodeGraphics::getHeightAndWidth() {
    return {height, width};
}

void NodeGraphics::setHeightWidth(int h, int w) {
    height = h; width = w;

    if (title_item) {

        if (title_item->document()) title_item->document()->setDocumentMargin(0);

        int infoDiameter = infoButton ? infoButton->rect().width() : std::max(9, (int)(title_height * 0.6));
        title_item->setTextWidth(width - infoDiameter - (4 * _padding));


        qreal textH = 0;
        if (title_item->document() && title_item->document()->documentLayout())
            textH = title_item->document()->documentLayout()->documentSize().height();
        if (textH <= 0) textH = title_item->boundingRect().height();
        int desired = int(textH) + 8;
        if (title_height < desired) title_height = desired;


        QRectF br = title_item->boundingRect();
        qreal brTop = br.top();
        qreal brH = br.height();
        qreal titleY = (title_height - brH) / 2.0 - brTop;
        title_item->setPos(_padding, titleY);
    }
    updateInfoButtonPos();
}

QVariant NodeGraphics::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == QGraphicsItem::ItemPositionHasChanged || change == QGraphicsItem::ItemScenePositionHasChanged) {
        if (node) {
            node->refreshSocketsAndEdges();
            Node* n = node;
            QTimer::singleShot(0, [n]() {
                if (!n) return;
                n->refreshSocketsAndEdges();
                for (auto *s : n->inputs) if (s && s->hasEdge() && s->edge) s->edge->updatePos();
                for (auto *s : n->outputs) if (s && s->hasEdge() && s->edge) s->edge->updatePos();
            });
            QTimer::singleShot(30, [n]() {
                if (!n) return;
                n->refreshSocketsAndEdges();
                for (auto *s : n->inputs) if (s && s->hasEdge() && s->edge) s->edge->updatePos();
                for (auto *s : n->outputs) if (s && s->hasEdge() && s->edge) s->edge->updatePos();
            });
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
