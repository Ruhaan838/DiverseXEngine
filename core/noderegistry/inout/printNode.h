
#ifndef PRINTNODE_H
#define PRINTNODE_H

#include "../function/functionNode.h"
#include <QLineEdit>

class PrintIfNode : public FunctionNode {
public:
    PrintIfNode(Scene* scene_, const std::string &title = "Print",
                std::vector<QString> input_size = {}, std::vector<QString> output_size = {});

    void execute() override;
    double getValues() override { return lastActive ? 1.0 : 0.0; }

    void showContent() override;
    void hideContent() override;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

    QString getMessage() const { return message; }
    void setMessage(const QString &m) { message = m; }

private:
    bool lastActive = false;
    QString message = "";
    QLineEdit* lineEdit = nullptr;
};

void registerPrintNodeType();

#endif // PRINTNODE_H

