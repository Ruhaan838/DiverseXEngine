//
// Created by Ruhaan on 18/08/25.
//

#include "noderegister.h"

#include <QMimeData>
#include <QDrag>
#include <QVBoxLayout>
#include <QStyle>
#include <QMouseEvent>
#include <QApplication>
#include <QPushButton>
#include <QSet>

#include "../nodes/node.h"
#include "../scene/nodescene.h"

void registerAllNodeTypes();

// Small helper button that starts a drag with node mime and supports double-click
class DraggableButton : public QPushButton {
    Q_OBJECT
public:
    DraggableButton(const QString& text, const QString& nodeName, QWidget* parent=nullptr)
        : QPushButton(text, parent), node(nodeName) {
        setCursor(Qt::PointingHandCursor);
        setFocusPolicy(Qt::NoFocus);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        // compact, bordered style
        setStyleSheet(
            "QPushButton{ background:#3a3f44; color:#ddd; border:1px solid #555;"
            "padding:4px 8px; border-radius:6px; font-size:12px; }"
            "QPushButton:hover{ background:#444a50; }"
            "QPushButton:pressed{ background:#2e7d32; color:#fff; }"
        );
    }

    QString node;

protected:
    void mousePressEvent(QMouseEvent* e) override {
        if (e->button() == Qt::LeftButton) startPos = e->pos();
        QPushButton::mousePressEvent(e);
    }
    void mouseMoveEvent(QMouseEvent* e) override {
        if (!(e->buttons() & Qt::LeftButton)) return QPushButton::mouseMoveEvent(e);
        if ((e->pos() - startPos).manhattanLength() < QApplication::startDragDistance())
            return QPushButton::mouseMoveEvent(e);
        auto* drag = new QDrag(this);
        auto* md = new QMimeData();
        md->setData(kNodeMimeType, node.toUtf8());
        drag->setMimeData(md);
        drag->exec(Qt::CopyAction);
    }
    void mouseDoubleClickEvent(QMouseEvent* e) override {
        if (e->button() == Qt::LeftButton) emit nodeActivated(node);
        QPushButton::mouseDoubleClickEvent(e);
    }
signals:
    void nodeActivated(const QString& name);
private:
    QPoint startPos;
};

NodeRegistery::NodeRegistery(Scene* parent) : QTreeWidget(), functions(nullptr), arithmetic(nullptr), math(nullptr), permutation(nullptr), bitoperations(nullptr), inputs(nullptr), outputs(nullptr), containers(nullptr), matrix(nullptr), scene(parent) {
    setHeaderHidden(true);
    setIndentation(14);
    setRootIsDecorated(true);
    setAnimated(true);
    setItemsExpandable(true);
    setExpandsOnDoubleClick(true);
    setSelectionMode(QAbstractItemView::NoSelection);
    setStyleSheet(
        "QTreeWidget{ border:0; background:#2f3235; }"
        "QTreeView::branch:closed:has-children{ image:url(:/qt-project.org/styles/commonstyle/images/branch-closed-16.png); }"
        "QTreeView::branch:open:has-children{ image:url(:/qt-project.org/styles/commonstyle/images/branch-open-16.png); }"
        "QTreeWidget::item{ height:28px; }" // slightly taller base row
    );

    registerAllNodeTypes();
    buildUI();
    setupDoubleClickHandler();

    // Single-click on a top-level item toggles expansion
    connect(this, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int){
        if (!item) return;
        if (!item->parent()) { // top-level
            item->setExpanded(!item->isExpanded());
        }
    });
}

QHash<QString, NodeInfo>& NodeRegistery::getRegistry() {
    static QHash<QString, NodeInfo> registry;
    return registry;
}

void NodeRegistery::registerNode(const QString& name, NodeCategory category,
                                const std::function<Node*(Scene*)>& creator,
                                int width, int height) {
    NodeInfo info;
    info.name = name;
    info.category = category;
    info.creator = creator;
    info.width = width;
    info.height = height;

    getRegistry()[name] = info;

    Node::registerType(name, creator);
}

QList<NodeInfo> NodeRegistery::getNodesByCategory(NodeCategory category) {
    QList<NodeInfo> result;
    for (auto it = getRegistry().begin(); it != getRegistry().end(); ++it) {
        if (it.value().category == category) {
            result.append(it.value());
        }
    }
    std::sort(result.begin(), result.end(), [](const NodeInfo& a, const NodeInfo& b){ return a.name.localeAwareCompare(b.name) < 0; });
    return result;
}

Node* NodeRegistery::createNode(const QString& name, Scene* scene) {
    auto& registry = getRegistry();
    if (registry.contains(name)) {
        const NodeInfo& info = registry[name];
        Node* node = info.creator(scene);
        if (node) {
            node->setHeightWidth(info.height, info.width);
            if (name == "Input Matrix") {
                node->setPosition(LEFT_BOTTOM, RIGHT_TOP);
            } else {
                switch (info.category) {
                    case NodeCategory::INPUT:
                        node->setPosition(LEFT_TOP, RIGHT_TOP);
                        break;
                    case NodeCategory::CONTAINER:
                        node->setPosition(LEFT_TOP, RIGHT_TOP);
                        break;
                    default:
                        node->setPosition(LEFT_TOP, RIGHT_BOTTOM);
                        break;
                }
            }
        }
        return node;
    }
    return nullptr;
}

static QTreeWidgetItem* makeCategory(QTreeWidget* tree, const QString& title) {
    auto* item = new QTreeWidgetItem(tree);
    item->setText(0, title);
    QFont f; f.setBold(true); item->setFont(0, f);
    return item;
}

void NodeRegistery::buildUI() {
    clear();

    functions = makeCategory(this, "Functions");
    arithmetic = makeCategory(this, "Arithmetic");
    math = makeCategory(this, "Math");
    permutation = makeCategory(this, "Permutation");
    bitoperations = makeCategory(this, "Bit Operations");
    inputs = makeCategory(this, "Inputs");
    outputs = makeCategory(this, "Outputs");
    containers = makeCategory(this, "Containers");
    matrix = makeCategory(this, "Matrix");

    auto addLeaf = [&](QTreeWidgetItem* parent, const QString& nodeName){
        auto* child = new QTreeWidgetItem(parent);
        // store name in data; clear display text to avoid background text bleeding behind the button
        child->setData(0, Qt::UserRole + 1, nodeName);
        child->setText(0, "");
        // Make room for external padding via an item widget wrapper
        child->setSizeHint(0, QSize(10, 36));

        auto* wrapper = new QWidget();
        auto* lay = new QVBoxLayout(wrapper);
        lay->setContentsMargins(8, 4, 8, 6); // side + vertical padding
        lay->setSpacing(0);

        auto* btn = new DraggableButton(nodeName, nodeName);
        btn->setToolTip(nodeName);
        wrapper->setToolTip(nodeName);
        lay->addWidget(btn);

        connect(btn, &DraggableButton::clicked, this, [this, nodeName](){
            Node* node = createNode(nodeName, scene);
            if (!node) return;
            scene->addNode(node);
            node->show();
            QPointF pending;
            if (scene && scene->takePendingNodePos(pending)) {
                node->setPos(static_cast<int>(pending.x()), static_cast<int>(pending.y()));
            }
        });
        connect(btn, &DraggableButton::nodeActivated, this, [this](const QString& name){
            Node* node = createNode(name, scene);
            if (!node) return;
            scene->addNode(node);
            node->show();
            QPointF pending;
            if (scene && scene->takePendingNodePos(pending)) {
                node->setPos(static_cast<int>(pending.x()), static_cast<int>(pending.y()));
            }
        });
        setItemWidget(child, 0, wrapper);
    };

    auto& registry = getRegistry();
    for (auto it = registry.begin(); it != registry.end(); ++it) {
        const NodeInfo& info = it.value();
        QTreeWidgetItem* parent = nullptr;
        switch (info.category) {
            case NodeCategory::FUNCTION: parent = functions; break;
            case NodeCategory::ARITHMETIC: parent = arithmetic; break;
            case NodeCategory::MATH: parent = math; break;
            case NodeCategory::PERMUTATION: parent = permutation; break;
            case NodeCategory::BITOPERATIONS: parent = bitoperations; break;
            case NodeCategory::INPUT: parent = inputs; break;
            case NodeCategory::OUTPUT: parent = outputs; break;
            case NodeCategory::MATRIX: parent = matrix; break;
            case NodeCategory::CONTAINER: parent = containers; break;
        }
        if (parent) addLeaf(parent, info.name);
    }
}

void NodeRegistery::setupDoubleClickHandler() {
    // Keep double-click on non-button areas for compatibility
    connect(this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int){
        if (!item) return;
        QString nodeName = item->data(0, Qt::UserRole + 1).toString();
        if (nodeName.isEmpty()) nodeName = item->text(0);
        // ignore categories
        static const QSet<QString> cats = {"Functions","Arithmetic","Math","Permutation","Bit Operations","Inputs","Outputs","Matrix","Containers"};
        if (cats.contains(nodeName)) return;
        Node* node = createNode(nodeName, scene);
        if (!node) return;
        scene->addNode(node);
        node->show();
        QPointF pending;
        if (scene && scene->takePendingNodePos(pending))
            node->setPos(static_cast<int>(pending.x()), static_cast<int>(pending.y()));
    });
}

void NodeRegistery::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) dragStartPos = event->pos();
    QTreeWidget::mousePressEvent(event);
}

void NodeRegistery::mouseMoveEvent(QMouseEvent *event) {
    if (!(event->buttons() & Qt::LeftButton)) return QTreeWidget::mouseMoveEvent(event);
    if ((event->pos() - dragStartPos).manhattanLength() < QApplication::startDragDistance())
        return QTreeWidget::mouseMoveEvent(event);

    auto* item = itemAt(dragStartPos);
    if (!item) return QTreeWidget::mouseMoveEvent(event);
    QString nodeName = item->data(0, Qt::UserRole + 1).toString();
    if (nodeName.isEmpty()) nodeName = item->text(0);
    static const QSet<QString> cats = {"Functions","Arithmetic","Math","Permutation","Bit Operations","Inputs","Outputs","Matrix","Containers"};
    if (cats.contains(nodeName)) return QTreeWidget::mouseMoveEvent(event);

    auto* drag = new QDrag(this);
    auto* md = new QMimeData();
    md->setData(kNodeMimeType, nodeName.toUtf8());
    drag->setMimeData(md);
    drag->exec(Qt::CopyAction);
}

QHash<QString, Node*> NodeRegistery::getfunctionalNode() const {
    QHash<QString, Node*> nodes;
    QList<NodeInfo> combined;
    auto fn = getNodesByCategory(NodeCategory::FUNCTION);
    auto ar = getNodesByCategory(NodeCategory::ARITHMETIC);
    auto ma = getNodesByCategory(NodeCategory::MATH);
    combined.append(fn); combined.append(ar); combined.append(ma);
    for (const NodeInfo& info : combined) {
        Node* node = info.creator(scene);
        if (node) { node->setHeightWidth(info.height, info.width); node->setPosition(LEFT_TOP, RIGHT_BOTTOM); }
        nodes[info.name] = node;
    }
    return nodes;
}

vector<Node*> NodeRegistery::getInputNode() const {
    vector<Node*> result;
    auto inputNodes = getNodesByCategory(NodeCategory::INPUT);
    for (const NodeInfo& info : inputNodes) {
        Node* node = info.creator(scene);
        if (node) { node->setHeightWidth(info.height, info.width); node->setPosition(LEFT_TOP, RIGHT_TOP); result.push_back(node); }
    }
    return result;
}

vector<Node*> NodeRegistery::getOutputNode() const {
    vector<Node*> result;
    auto outputNodes = getNodesByCategory(NodeCategory::OUTPUT);
    for (const NodeInfo& info : outputNodes) {
        Node* node = info.creator(scene);
        if (node) { node->setHeightWidth(info.height, info.width); node->setPosition(LEFT_TOP, RIGHT_BOTTOM); result.push_back(node); }
    }
    return result;
}

#include "noderegister.moc"
