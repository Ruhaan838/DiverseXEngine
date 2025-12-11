#include "printListMatrixNode.h"
#include "printNode.h" // for stylesheet reuse maybe
#include "inoutNode.h"
#include "../list/listNode.h"
#include "../matrix/matrixNode.h"
#include "../function/functionNode.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include "../../widgets/widgets.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QFont>

static inline const char* print_list_matrix_stylesheet = R"(
    QTextEdit {
        background-color: #2b2b2b;
        color: #ffffff;
        border: 1px solid #3d3d3d;
        border-radius: 5px;
        padding: 4px 8px;
    }
    QTextEdit:focus { border: 2px solid #4a9eff; }
)";

PrintListMatrixNode::PrintListMatrixNode(Scene *scene_, const std::string &title,
                                         std::vector<QString> input_size, std::vector<QString> output_size)
: FunctionNode(scene_, title,
               (input_size.empty() ? std::vector<QString>{QString("List"), QString("Matrix")} : input_size),
               output_size,
               false /* allow add socket */) {
    showContent();
}

void PrintListMatrixNode::showContent() {
    if (content) return;
    auto *widget = new WidgetNode(this);
    display = new TextEdit("No data printed yet", widget);
    QFont font("Arial", 12, QFont::Normal);
    display->setFont(font);
    display->setReadOnly(true);
    display->setMinimumWidth(200);
    display->setMinimumHeight(100);
    display->setStyleSheet(print_list_matrix_stylesheet);

    widget->addContent({display});
    setContentHeight(120);
    setContent(widget);
}

void PrintListMatrixNode::hideContent() {
    if (content) {
        content->hide();
        content = nullptr;
        display = nullptr;
    }
}

QString PrintListMatrixNode::formatList(ListProvider* lp) const {
    if (!lp) return QString("<no list>");
    auto vals = lp->getListValues();
    QStringList parts;
    parts.reserve(static_cast<int>(vals.size()));
    for (double v : vals) parts << QString::number(v);
    return QString("List[%1] = [ %2 ]").arg(lp->count()).arg(parts.join(", "));
}

QString PrintListMatrixNode::formatMatrix(MatrixProvider* mp) const {
    if (!mp) return QString("<no matrix>");
    auto mat = mp->getMatrixValues();
    QStringList rows;
    for (size_t r = 0; r < mat.size(); ++r) {
        QStringList cols;
        for (size_t c = 0; c < mat[r].size(); ++c) cols << QString::number(mat[r][c]);
        rows << (QString("[ %1 ]").arg(cols.join(", ")));
    }
    return QString("Matrix[%1x%2]:\n%3").arg(mp->rowCount()).arg(mp->colCount()).arg(rows.join("\n"));
}

void PrintListMatrixNode::execute() {
    // Acquire list provider from input 0
    ListProvider* lp = nullptr;
    if (inputs.size() > 0) {
        auto *s = inputs[0];
        if (s && s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
            lp = dynamic_cast<ListProvider*>(s->getFirstEdge()->startSocket->node);
        }
    }
    // Acquire matrix provider from input 1
    MatrixProvider* mp = nullptr;
    if (inputs.size() > 1) {
        auto *s = inputs[1];
        if (s && s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
            mp = dynamic_cast<MatrixProvider*>(s->getFirstEdge()->startSocket->node);
        }
    }

    QString dump;
    dump += formatList(lp);
    dump += "\n";
    dump += formatMatrix(mp);

    lastDump = dump;
    qDebug() << "PrintListMatrix:\n" << dump;
    if (display) display->setPlainText(dump);
}

QJsonObject PrintListMatrixNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "PrintListMatrixNode";
    obj["dump"] = lastDump;
    return obj;
}

bool PrintListMatrixNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    bool ok = FunctionNode::deserialize(data, hashmap);
    lastDump = data.value("dump").toString();
    if (display) display->setPlainText(lastDump.isEmpty() ? QString("No data printed yet") : lastDump);
    return ok;
}

