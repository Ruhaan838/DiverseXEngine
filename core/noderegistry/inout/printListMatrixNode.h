#ifndef PRINTLISTMATRIXNODE_H
#define PRINTLISTMATRIXNODE_H

#include "../function/functionNode.h"
#include <QString>

class TextEdit;
class ListProvider;
class MatrixProvider;
class SocketNode;

// Node that prints connected List and/or Matrix values.
class PrintListMatrixNode : public FunctionNode {
public:
    PrintListMatrixNode(Scene* scene_, const std::string &title = "Print List+Matrix",
                        std::vector<QString> input_size = {QString("List"), QString("Matrix")},
                        std::vector<QString> output_size = {});

    void execute() override;
    double getValues() override { return 0.0; } // No numeric output

    void showContent() override;
    void hideContent() override;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

private:
    TextEdit* display = nullptr;
    QString lastDump;
    QString formatList(ListProvider* lp) const;
    QString formatMatrix(MatrixProvider* mp) const;
};

#endif // PRINTLISTMATRIXNODE_H

