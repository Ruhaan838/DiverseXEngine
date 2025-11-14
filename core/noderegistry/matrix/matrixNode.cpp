#include "matrixNode.h"

#include <QScrollArea>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QMouseEvent>
#include <QLabel>
#include <algorithm>
#include <functional>
#include <QIntValidator>

#include "../function/functionNode.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include "../../widgets/widgets.h"

using std::vector;

class ButtonMouseFilter : public QObject {
public:
    using Handler = std::function<void()>;
    ButtonMouseFilter(Handler onLeft, Handler onRight, QObject* parent=nullptr)
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

static inline const char* matrix_input_stylesheet = R"(
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

MatrixInputNode::MatrixInputNode(Scene *scene_, const std::string &title,
                                 vector<QString> input_size, vector<QString> output_size)
    : Node(scene_, title, std::move(input_size), std::move(output_size)) {
    // Inputs on left-bottom, outputs on right-top (top-aligned ordering)
    setPosition(LEFT_BOTTOM, RIGHT_TOP);
    rows = 1; cols = 1;
    values.assign(rows, vector<double>(cols, 0.0));
    buildContent();
}

void MatrixInputNode::buildContent() {
    auto *widget = new WidgetNode(this);

    auto *topRow = new QWidget(widget);
    auto *topLayout = new QHBoxLayout(topRow);
    topLayout->setContentsMargins(0, 0, 0, 0);

    scroll = new QScrollArea(topRow);
    scroll->setWidgetResizable(true);

    container = new QWidget(scroll);
    grid = new QGridLayout(container);
    grid->setContentsMargins(4, 4, 4, 4);
    grid->setSpacing(6);

    container->setLayout(grid);
    scroll->setWidget(container);

    addColBtn = new QPushButton("+", topRow);
    addColBtn->setFixedWidth(28);

    topLayout->addWidget(scroll, /*stretch*/ 1);
    topLayout->addWidget(addColBtn, /*stretch*/ 0);

    // Bottom controls
    auto *bottomRow = new QWidget(widget);
    auto *bottomLayout = new QHBoxLayout(bottomRow);
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    addRowBtn = new QPushButton("+", bottomRow);
    addRowBtn->setFixedWidth(36);
    dimsLabel = new QLabel(bottomRow);
    dimsLabel->setText("1 x 1");
    QFont dimsFont("Arial", 15);
    dimsLabel->setFont(dimsFont);
    dimsLabel->setStyleSheet("color: #a0a0a0;");

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(addRowBtn);
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(dimsLabel);

    topRow->setStyleSheet(matrix_input_stylesheet);
    bottomRow->setStyleSheet(matrix_input_stylesheet);

    rebuildGrid();

    addColBtn->setToolTip("Left click: add column\nRight click: remove last column");
    auto *colFilter = new ButtonMouseFilter(
        [this]() { resizeMatrix(rows, cols + 1); },
        [this]() { if (cols > 1) resizeMatrix(rows, cols - 1); },
        addColBtn
    );
    addColBtn->installEventFilter(colFilter);

    addRowBtn->setToolTip("Left click: add row\nRight click: remove last row");
    auto *rowFilter = new ButtonMouseFilter(
        [this]() { resizeMatrix(rows + 1, cols); },
        [this]() { if (rows > 1) resizeMatrix(rows - 1, cols); },
        addRowBtn
    );
    addRowBtn->installEventFilter(rowFilter);

    widget->addContent({topRow, bottomRow});

    setContentHeight(180);
    setContent(widget);

    updateDimsLabel();
}

void MatrixInputNode::resizeMatrix(int newRows, int newCols) {
    if (newRows < 1) newRows = 1;
    if (newCols < 1) newCols = 1;

    vector<vector<double>> newVals(newRows, vector<double>(newCols, 0.0));

    for (int r = 0; r < std::min(rows, newRows); ++r) {
        for (int c = 0; c < std::min(cols, newCols); ++c) {
            newVals[r][c] = values[r][c];
        }
    }

    rows = newRows; cols = newCols; values.swap(newVals);
    rebuildGrid();
    updateDimsLabel();
}

void MatrixInputNode::updateDimsLabel() {
    if (dimsLabel) dimsLabel->setText(QString("%1 x %2").arg(rows).arg(cols));
}

void MatrixInputNode::updateButtonsLabel() {
    // Always show '+' now; right-click handles removal.
    if (addColBtn) addColBtn->setText("+");
    if (addRowBtn) addRowBtn->setText("+");
}

void MatrixInputNode::setRemoveMode(bool on) {
    // Deprecated: removal is via right-click; keep for compatibility
    (void)on;
}

bool MatrixInputNode::isRemoveMode() const {
    return false;
}

void MatrixInputNode::rebuildGrid() {
    edits.clear();
    if (!grid) return;
    while (QLayoutItem* item = grid->takeAt(0)) {
        if (QWidget* w = item->widget()) { w->deleteLater(); }
        delete item;
    }

    edits.resize(rows);
    for (int r = 0; r < rows; ++r) {
        edits[r].resize(cols);
        for (int c = 0; c < cols; ++c) {
            auto *le = new QLineEdit(container);
            le->setAlignment(Qt::AlignCenter);
            le->setValidator(new QDoubleValidator(le));
            le->setText(QString::number(values[r][c]));
            le->setMinimumWidth(48);
            le->setMaximumWidth(72);

            QObject::connect(le, &QLineEdit::textChanged, [this, r, c](const QString &t) {
                bool ok = false; double v = t.toDouble(&ok);
                values[r][c] = ok ? v : 0.0;
            });

            grid->addWidget(le, r, c);
            edits[r][c] = le;
        }
    }
}

vector<vector<double>> MatrixInputNode::getMatrixValues() const {
    return values; // copy
}

QJsonObject MatrixInputNode::serialize() {
    QJsonObject obj = Node::serialize();
    obj["node_type"] = "MatrixInputNode";
    obj["rows"] = rows;
    obj["cols"] = cols;

    QJsonArray arrRows;
    for (int r = 0; r < rows; ++r) {
        QJsonArray arrCols;
        for (int c = 0; c < cols; ++c) { arrCols.append(values[r][c]); }
        arrRows.append(arrCols);
    }
    obj["matrix"] = arrRows;

    QJsonArray out;
    for (auto *o : outputs) if (o) out.append(o->serialize());
    obj["outputs"] = out;

    auto hw = this->getHeightAndWidth();
    obj["h"] = hw.first;
    obj["w"] = hw.second;

    return obj;
}

bool MatrixInputNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    int h = data.value("h").toInt();
    int w = data.value("w").toInt();
    setHeightWidth(h, w);
    show();
    bool ok = Node::deserialize(data, hashmap);

    rows = data.value("rows").toInt(1);
    cols = data.value("cols").toInt(1);

    values.assign(rows, vector<double>(cols, 0.0));
    QJsonArray arrRows = data.value("matrix").toArray();
    for (int r = 0; r < rows; ++r) {
        QJsonArray arrCols = arrRows[r].toArray();
        for (int c = 0; c < cols; ++c) {
            values[r][c] = arrCols[c].toDouble(0.0);
        }
    }

    rebuildGrid();

    QJsonArray out = data.value("outputs").toArray();
    for (auto *s : outputs) { delete s; }
    outputs.clear();
    for (auto v : out) {
        QJsonObject o = v.toObject();
        // Force outputs to top-right regardless of saved position
        POSITION pos = RIGHT_TOP;
        QString socketType = o.value("socket_type").toString();
        auto *sok = new SocketNode(this, o.value("index").toInt(), pos, socketType);
        sok->deserialize(o, hashmap);
        outputs.push_back(sok);
    }

    refreshSocketsAndEdges();
    updateDimsLabel();
    return ok;
}

Determinant2x2Node::Determinant2x2Node(Scene *scene_, const std::string &title,
                                       vector<QString> input_size, vector<QString> output_size)
    : FunctionNode(scene_, title, std::move(input_size), std::move(output_size), false) {
}

void Determinant2x2Node::execute() {
    val = 0.0;
    if (inputs.empty()) return;
    auto *s = inputs[0];
    if (!s || !s->hasEdge() || !s->getFirstEdge() || !s->getFirstEdge()->startSocket) return;
    Node* prev = s->getFirstEdge()->startSocket->node;
    if (!prev) return;

    // Try to read matrix
    auto *prov = dynamic_cast<MatrixProvider*>(prev);
    if (!prov) { setInfoText("Input is not a matrix"); return; }
    if (prov->rowCount() != 2 || prov->colCount() != 2) {
        setInfoText("Determinant2x2 requires 2x2 matrix");
        return;
    }
    auto m = prov->getMatrixValues();
    double a = m[0][0], b = m[0][1], c = m[1][0], d = m[1][1];
    val = a * d - b * c;
}

QJsonObject Determinant2x2Node::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "Determinant2x2Node";
    obj["value"] = val;
    return obj;
}

bool Determinant2x2Node::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    val = data.value("value").toDouble(0.0);
    return FunctionNode::deserialize(data, hashmap);
}

MatrixIndexNode::MatrixIndexNode(Scene *scene_, const std::string &title,
                                 vector<QString> input_size, vector<QString> output_size)
    : FunctionNode(scene_, title, std::move(input_size), std::move(output_size), false) {
    showContent();
}

void MatrixIndexNode::showContent() {
    if (content) return;

    auto *widget = new WidgetNode(this);

    // Row/Col editors
    auto *rowCol = new QWidget(widget);
    auto *hl = new QHBoxLayout(rowCol);
    hl->setContentsMargins(0,0,0,0);
    hl->setSpacing(6);

    rowEdit = new QLineEdit(rowCol);
    colEdit = new QLineEdit(rowCol);

    auto *ivalR = new QIntValidator(0, 1000000, rowEdit);
    auto *ivalC = new QIntValidator(0, 1000000, colEdit);
    rowEdit->setValidator(ivalR);
    colEdit->setValidator(ivalC);

    rowEdit->setPlaceholderText("Row");
    colEdit->setPlaceholderText("Col");
    rowEdit->setText(QString::number(rowIndex_));
    colEdit->setText(QString::number(colIndex_));

    QObject::connect(rowEdit, &QLineEdit::textChanged, [this](const QString& t){
        bool ok=false; int v = t.toInt(&ok); rowIndex_ = ok ? v : 0; });
    QObject::connect(colEdit, &QLineEdit::textChanged, [this](const QString& t){
        bool ok=false; int v = t.toInt(&ok); colIndex_ = ok ? v : 0; });

    hl->addWidget(new QLabel("Row:" , rowCol));
    hl->addWidget(rowEdit);
    hl->addSpacing(8);
    hl->addWidget(new QLabel("Col:", rowCol));
    hl->addWidget(colEdit);
    rowCol->setLayout(hl);

    widget->addContent({rowCol});
    setContentHeight(70);
    setContent(widget);
}

void MatrixIndexNode::execute() {
    val = 0.0;
    if (inputs.empty()) return;

    // Matrix input
    Node* mnode = getPrevNode(0);
    auto *prov = dynamic_cast<MatrixProvider*>(mnode);
    if (!prov) { setInfoText("Input is not a matrix"); return; }

    int r = rowIndex_;
    int c = colIndex_;

    if (r < 0 || c < 0 || r >= prov->rowCount() || c >= prov->colCount()) {
        setInfoText("Index out of bounds");
        return;
    }

    auto mat = prov->getMatrixValues();
    val = mat[r][c];
}

QJsonObject MatrixIndexNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "MatrixIndexNode";
    obj["value"] = val;
    obj["row"] = rowIndex_;
    obj["col"] = colIndex_;
    return obj;
}

bool MatrixIndexNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    val = data.value("value").toDouble(0.0);
    bool ok = FunctionNode::deserialize(data, hashmap);
    rowIndex_ = data.value("row").toInt(0);
    colIndex_ = data.value("col").toInt(0);
    if (rowEdit) rowEdit->setText(QString::number(rowIndex_));
    if (colEdit) colEdit->setText(QString::number(colIndex_));
    return ok;
}

void MatrixInputNode::resetMatrixValues() {
    // Set all stored values to 0 and clear UI edits
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (r < (int)values.size() && c < (int)values[r].size()) {
                values[r][c] = 0.0;
            }
            if (r < (int)edits.size() && c < (int)edits[r].size() && edits[r][c]) {
                edits[r][c]->clear(); // blank per requirement
            }
        }
    }
}
