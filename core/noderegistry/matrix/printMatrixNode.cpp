#include "printMatrixNode.h"
#include "matrixNode.h"
#include "../function/functionNode.h"
#include "../../widgets/widgets.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include <QJsonObject>
#include <QDebug>
#include <QFont>

static inline const char* print_matrix_stylesheet = R"(
    QTextEdit {
        background-color: #2b2b2b;
        color: #ffffff;
        border: 1px solid #3d3d3d;
        border-radius: 5px;
        padding: 4px 8px;
    }
    QTextEdit:focus { border: 2px solid #4a9eff; }
)";

PrintMatrixNode::PrintMatrixNode(Scene *scene_, const std::string &title,
                                 std::vector<QString> input_size,
                                 std::vector<QString> output_size)
: FunctionNode(scene_, title,
               (input_size.empty() ? std::vector<QString>{QString("Matrix")} : input_size),
               output_size,
               false) {
    showContent();
}

void PrintMatrixNode::showContent() {
    if (content) return;
    auto *widget = new WidgetNode(this);
    display = new TextEdit("No matrix printed yet", widget);
    QFont font("Arial", 12, QFont::Normal);
    display->setFont(font);
    display->setReadOnly(true);
    display->setMinimumWidth(200);
    display->setMinimumHeight(100);
    display->setStyleSheet(print_matrix_stylesheet);
    widget->addContent({display});
    setContentHeight(120);
    setContent(widget);
}

void PrintMatrixNode::hideContent() {
    if (content) {
        content->hide();
        content = nullptr;
        display = nullptr;
    }
}

void PrintMatrixNode::execute() {
    MatrixProvider* mp = nullptr;
    if (!inputs.empty()) {
        auto *s = inputs[0];
        if (s && s->hasEdge() && s->getFirstEdge() && s->getFirstEdge()->startSocket) {
            mp = dynamic_cast<MatrixProvider*>(s->getFirstEdge()->startSocket->node);
        }
    }
    QString dump;
    if (!mp) {
        dump = "<no matrix connected>";
    } else {
        auto mat = mp->getMatrixValues();
        QStringList rows;
        for (size_t r = 0; r < mat.size(); ++r) {
            QStringList cols;
            for (size_t c = 0; c < mat[r].size(); ++c) cols << QString::number(mat[r][c]);
            rows << (QString("[ %1 ]").arg(cols.join(", ")));
        }
        dump = rows.join("\n");
    }
    lastDump = dump;
    qDebug() << "PrintMatrix:\n" << dump;
    if (display) display->setPlainText(dump);
}

QJsonObject PrintMatrixNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "PrintMatrixNode";
    obj["dump"] = lastDump;
    return obj;
}

bool PrintMatrixNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    bool ok = FunctionNode::deserialize(data, hashmap);
    lastDump = data.value("dump").toString();
    if (display) display->setPlainText(lastDump.isEmpty() ? QString("No matrix printed yet") : lastDump);
    return ok;
}

