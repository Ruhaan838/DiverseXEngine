#include "matrixOpsNode.h"
#include "matrixNode.h"
#include "../function/functionNode.h"
#include "../../nodes/socket.h"
#include "../../nodes/edge.h"
#include <QJsonObject>

MatrixBinaryOpNode::MatrixBinaryOpNode(Scene* scene_, const std::string &title, Op op,
                                       std::vector<QString> input_size,
                                       std::vector<QString> output_size)
    : FunctionNode(scene_, title, std::move(input_size), std::move(output_size), false), op_(op) {}

void MatrixBinaryOpNode::execute() {
    out_.clear();
    if (inputs.size() < 2) return;
    Node* aNode = getPrevNode(0);
    Node* bNode = getPrevNode(1);
    auto *a = dynamic_cast<MatrixProvider*>(aNode);
    auto *b = dynamic_cast<MatrixProvider*>(bNode);
    if (!a || !b) { setInfoText("Inputs must be matrices"); return; }
    if (a->rowCount() != b->rowCount() || a->colCount() != b->colCount()) {
        setInfoText("Matrix sizes must match"); return;
    }
    auto A = a->getMatrixValues();
    auto B = b->getMatrixValues();
    int R = a->rowCount();
    int C = a->colCount();
    out_.assign(R, std::vector<double>(C, 0.0));
    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            switch (op_) {
                case Op::Add: out_[r][c] = A[r][c] + B[r][c]; break;
                case Op::Sub: out_[r][c] = A[r][c] - B[r][c]; break;
                case Op::Mul: out_[r][c] = A[r][c] * B[r][c]; break;
                case Op::Div: out_[r][c] = B[r][c] == 0.0 ? 0.0 : (A[r][c] / B[r][c]); break;
            }
        }
    }
}

double MatrixBinaryOpNode::getValues() {
    double sum = 0.0;
    for (const auto &row : out_) {
        for (double v : row) sum += v;
    }
    return sum;
}

QJsonObject MatrixBinaryOpNode::serialize() {
    QJsonObject obj = FunctionNode::serialize();
    obj["node_type"] = "MatrixBinaryOpNode";
    obj["op"] = static_cast<int>(op_);
    // store result size only (values are computed at runtime)
    obj["rows"] = static_cast<int>(out_.size());
    obj["cols"] = out_.empty() ? 0 : static_cast<int>(out_[0].size());
    return obj;
}

bool MatrixBinaryOpNode::deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) {
    // op_ set by concrete subclass ctor; ignore stored op field
    return FunctionNode::deserialize(data, hashmap);
}
