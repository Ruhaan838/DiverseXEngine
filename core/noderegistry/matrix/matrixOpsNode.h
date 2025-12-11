#ifndef MATRIXOPSNODE_H
#define MATRIXOPSNODE_H

#include "matrixNode.h"
#include "../function/functionNode.h"
#include <vector>

// Base for binary pointwise ops on matrices
class MatrixBinaryOpNode : public FunctionNode, public MatrixProvider {
public:
    enum class Op { Add, Sub, Mul, Div };

    MatrixBinaryOpNode(Scene* scene_, const std::string &title,
                       Op op,
                       std::vector<QString> input_size = {QString("Matrix A"), QString("Matrix B")},
                       std::vector<QString> output_size = {QString("Result")});

    void execute() override;
    // Provide a numeric value to satisfy FunctionNode API; return sum of elements
    double getValues() override;

    // MatrixProvider
    std::vector<std::vector<double>> getMatrixValues() const override { return out_; }
    int rowCount() const override { return static_cast<int>(out_.size()); }
    int colCount() const override { return out_.empty() ? 0 : static_cast<int>(out_[0].size()); }

    std::vector<std::vector<double>> getMatrix() const { return out_; }

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

    Op op() const { return op_; }

private:
    Op op_;
    std::vector<std::vector<double>> out_;
};

class MatrixAddPointwiseNode : public MatrixBinaryOpNode {
public:
    MatrixAddPointwiseNode(Scene* scene_, const std::string &title = "Matrix Add (Pointwise)")
      : MatrixBinaryOpNode(scene_, title, Op::Add) {}
};
class MatrixSubPointwiseNode : public MatrixBinaryOpNode {
public:
    MatrixSubPointwiseNode(Scene* scene_, const std::string &title = "Matrix Subtract (Pointwise)")
      : MatrixBinaryOpNode(scene_, title, Op::Sub) {}
};
class MatrixMulPointwiseNode : public MatrixBinaryOpNode {
public:
    MatrixMulPointwiseNode(Scene* scene_, const std::string &title = "Matrix Multiply (Pointwise)")
      : MatrixBinaryOpNode(scene_, title, Op::Mul) {}
};
class MatrixDivPointwiseNode : public MatrixBinaryOpNode {
public:
    MatrixDivPointwiseNode(Scene* scene_, const std::string &title = "Matrix Divide (Pointwise)")
      : MatrixBinaryOpNode(scene_, title, Op::Div) {}
};

#endif // MATRIXOPSNODE_H

