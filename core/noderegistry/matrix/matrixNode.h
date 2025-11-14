#ifndef MATRIXNODE_H
#define MATRIXNODE_H

#include "../function/functionNode.h"
#include <QVector>
#include <vector>

class QLineEdit;
class QWidget;
class QGridLayout;
class QPushButton;
class QScrollArea;
class QLabel;

// Lightweight provider interface so ops can read matrices
class MatrixProvider {
public:
    virtual ~MatrixProvider() = default;
    virtual std::vector<std::vector<double>> getMatrixValues() const = 0;
    virtual int rowCount() const = 0;
    virtual int colCount() const = 0;
};

// MatrixInputNode: lets user enter an N x M matrix; outputs a Matrix object through its socket.
class MatrixInputNode : public Node, public MatrixProvider {
public:
    MatrixInputNode(Scene* scene_, const std::string &title = "Input Matrix",
                    std::vector<QString> input_size = {}, std::vector<QString> output_size = {QString("Output")});

    // UI management
    void buildContent();

    // MatrixProvider
    std::vector<std::vector<double>> getMatrixValues() const override;
    int rowCount() const override { return rows; }
    int colCount() const override { return cols; }

    // Serialization
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t>& hashmap) override;

    // Remove-mode control (for Alt toggle)
    void setRemoveMode(bool on);
    bool isRemoveMode() const;
    // Reset all matrix cell values to blank and zero internal storage
    void resetMatrixValues();

private:
    // Helpers
    void rebuildGrid();
    void resizeMatrix(int newRows, int newCols);
    void updateButtonsLabel();
    void updateDimsLabel();

    // Data
    int rows = 1;
    int cols = 1;
    std::vector<std::vector<double>> values; // mirrors UI

    // UI elements we cache
    QWidget* container = nullptr;      // inside scroll area
    QGridLayout* grid = nullptr;
    QScrollArea* scroll = nullptr;
    QPushButton* addRowBtn = nullptr;
    QPushButton* addColBtn = nullptr;
    QLabel* dimsLabel = nullptr;
    std::vector<std::vector<QLineEdit*>> edits;

    bool removeMode = false; // toggled by Alt key
};

// Determinant of a 2x2 matrix: requires 2x2, outputs a number
class Determinant2x2Node : public FunctionNode {
public:
    Determinant2x2Node(Scene* scene_, const std::string &title = "Matrix Determinant 2x2",
                       std::vector<QString> input_size = {QString("Matrix")}, std::vector<QString> output_size = {QString("Output")});

    void execute() override;
    double getValues() override { return val; }

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

private:
    double val = 0.0;
};

// Matrix Index: get element at [row, col]
class MatrixIndexNode : public FunctionNode {
public:
    MatrixIndexNode(Scene* scene_, const std::string &title = "Matrix Index",
                    std::vector<QString> input_size = {QString("Matrix")},
                    std::vector<QString> output_size = {QString("Value")});

    void execute() override;
    double getValues() override { return val; }

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

    void showContent() override;

private:
    double val = 0.0;
    int rowIndex_ = 0;
    int colIndex_ = 0;
    QLineEdit* rowEdit = nullptr;
    QLineEdit* colEdit = nullptr;
};

#endif // MATRIXNODE_H

