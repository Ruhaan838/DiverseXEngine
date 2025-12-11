#ifndef PRINTMATRIXNODE_H
#define PRINTMATRIXNODE_H

#include "matrixNode.h" // for MatrixProvider
#include "../function/functionNode.h"
#include <QString>

class TextEdit;

class PrintMatrixNode : public FunctionNode {
public:
    PrintMatrixNode(Scene* scene_, const std::string &title = "Print Matrix",
                    std::vector<QString> input_size = {QString("Matrix")},
                    std::vector<QString> output_size = {});

    void execute() override;
    double getValues() override { return 0.0; }
    void showContent() override;
    void hideContent() override;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

private:
    TextEdit* display = nullptr;
    QString lastDump;
};

#endif // PRINTMATRIXNODE_H

