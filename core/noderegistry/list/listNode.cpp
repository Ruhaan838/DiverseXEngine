#include "listNode.h"

#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QJsonArray>
#include <QJsonObject>
#include <QMouseEvent>
#include <QLabel>

#include "../../widgets/widgets.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"

static inline const char* list_input_stylesheet = R"(
    QLineEdit {
        background-color: #2b2b2b;
        color: #ffffff;
        border: 1px solid #3d3d3d;
        border-radius: 5px;
        padding: 2px 6px;
    }
    QLineEdit:focus {
        border: 2px solid #4a9eff;
    }
    QPushButton {
        background-color: #3a3a3a;
        color: #ffffff;
        border: 1px solid #4a4a4a;
        border-radius: 6px;
        padding: 4px 8px;
    }
    QPushButton:hover { background-color: #4a4a4a; }
)";

class AddRemoveButtonFilter : public QObject {
public:
    using Handler = std::function<void()>;
    AddRemoveButtonFilter(Handler onLeft, Handler onRight, QObject* parent=nullptr)
        : QObject(parent), leftHandler(std::move(onLeft)), rightHandler(std::move(onRight)) {}
protected:
    bool eventFilter(QObject* obj, QEvent* ev) override {
        if (ev->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(ev);
            if (me->button() == Qt::LeftButton) { if (leftHandler) leftHandler(); return true; }
            if (me->button() == Qt::RightButton) { if (rightHandler) rightHandler(); return true; }
        }
        return QObject::eventFilter(obj, ev);
    }
private:
    Handler leftHandler;
    Handler rightHandler;
};

ListInputNode::ListInputNode(Scene *scene_, const std::string &title,
                             std::vector<QString> input_size, std::vector<QString> output_size)
    : Node(scene_, title, std::move(input_size), std::move(output_size)) {
    // No inputs, single output on right-top
    setPosition(LEFT_BOTTOM, RIGHT_TOP);
    buildContent();
}

void ListInputNode::buildContent() {
    auto *widget = new WidgetNode(this);

    auto *root = new QWidget(widget);
    auto *rootLayout = new QVBoxLayout(root);
    rootLayout->setContentsMargins(0,0,0,0);
    rootLayout->setSpacing(4);

    scroll = new QScrollArea(root);
    scroll->setWidgetResizable(true);

    listHolder = new QWidget(scroll);
    listLayout = new QVBoxLayout(listHolder);
    listLayout->setContentsMargins(4,4,4,4);
    listLayout->setSpacing(6);
    listHolder->setLayout(listLayout);
    scroll->setWidget(listHolder);

    addBtn = new QPushButton("+", root);
    addBtn->setFixedWidth(36);
    addBtn->setToolTip("Left click: add entry\nRight click: remove last entry (min 1)");

    auto *btnHolder = new QWidget(root);
    auto *btnLayout = new QHBoxLayout(btnHolder);
    btnLayout->setContentsMargins(0,0,0,0);
    btnLayout->addStretch(1);
    btnLayout->addWidget(addBtn);
    btnLayout->addStretch(1);

    rootLayout->addWidget(scroll, 1);
    rootLayout->addWidget(btnHolder, 0);

    root->setStyleSheet(list_input_stylesheet);

    // At least one entry
    if (values.empty()) values.push_back(0.0);
    rebuildEntries();

    auto *filter = new AddRemoveButtonFilter(
        [this]() { addEntry(0.0); },
        [this]() { removeLastEntry(); },
        addBtn
    );
    addBtn->installEventFilter(filter);

    widget->addContent({root});
    setContentHeight(160);
    setContent(widget);
}

void ListInputNode::addEntry(double initial) {
    values.push_back(initial);
    rebuildEntries();
}

void ListInputNode::removeLastEntry() {
    if (values.size() <= 1) return; // keep minimum 1
    values.pop_back();
    rebuildEntries();
}

void ListInputNode::rebuildEntries() {
    // Clear existing
    edits.clear();
    if (!listLayout) return;
    while (QLayoutItem* item = listLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) { w->deleteLater(); }
        delete item;
    }

    for (size_t i = 0; i < values.size(); ++i) {
        auto *row = new QWidget(listHolder);
        auto *hl = new QHBoxLayout(row);
        hl->setContentsMargins(0,0,0,0);
        hl->setSpacing(4);

        auto *label = new QLabel(QString("[%1]").arg(i), row);
        auto *edit = new QLineEdit(row);
        edit->setAlignment(Qt::AlignCenter);
        edit->setValidator(new QDoubleValidator(edit));
        edit->setText(QString::number(values[i]));
        edit->setMinimumWidth(60);
        edit->setMaximumWidth(90);

        QObject::connect(edit, &QLineEdit::textChanged, [this, idx=i](const QString &t){
            bool ok=false; double v=t.toDouble(&ok); values[idx] = ok ? v : 0.0; });

        hl->addWidget(label);
        hl->addWidget(edit);
        row->setLayout(hl);
        listLayout->addWidget(row);
        edits.push_back(edit);
    }
    listLayout->addStretch(1);
}

void ListInputNode::resetValues() {
    for (auto &v : values) v = 0.0;
    for (size_t i = 0; i < edits.size() && i < values.size(); ++i) {
        if (edits[i]) edits[i]->setText(QString::number(values[i]));
    }
}

QJsonObject ListInputNode::serialize() {
    QJsonObject obj = Node::serialize();
    obj["node_type"] = "ListInputNode";

    QJsonArray arr;
    for (double v : values) arr.append(v);
    obj["list"] = arr;

    QJsonArray out;
    for (auto *o : outputs) if (o) out.append(o->serialize());
    obj["outputs"] = out;

    auto hw = this->getHeightAndWidth();
    obj["h"] = hw.first;
    obj["w"] = hw.second;

    return obj;
}

bool ListInputNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    int h = data.value("h").toInt();
    int w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();
    bool ok = Node::deserialize(data, hashmap);

    values.clear();
    QJsonArray arr = data.value("list").toArray();
    for (auto v : arr) values.push_back(v.toDouble(0.0));
    if (values.empty()) values.push_back(0.0);

    rebuildEntries();

    QJsonArray out = data.value("outputs").toArray();
    for (auto *s : outputs) { delete s; }
    outputs.clear();
    for (auto v : out) {
        QJsonObject o = v.toObject();
        POSITION pos = RIGHT_TOP; // force
        QString socketType = o.value("socket_type").toString();
        auto *sok = new SocketNode(this, o.value("index").toInt(), pos, socketType);
        sok->deserialize(o, hashmap);
        outputs.push_back(sok);
    }

    refreshSocketsAndEdges();
    return ok;
}

