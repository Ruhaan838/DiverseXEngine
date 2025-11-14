// filepath: /Users/ruhaan/Documents/DiverseXEngine/core/noderegistry/function/ifElseNode.h
#ifndef IFELSENODE_H
#define IFELSENODE_H

#include "functionNode.h"
#include <QPushButton>

class IfElseNode : public FunctionNode {
public:
    IfElseNode(Scene* scene_, const std::string &title = "If/Else");

    void execute() override;
    double getValues() override { return activeValue; }

    void showContent() override;
    void hideContent() override;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;

private:
    // Number of (Cond, Val) pairs; final input is always Else
    int pairCount = 1;
    double activeValue = 0.0;

    QPushButton* addBtn = nullptr;

    void addElseIfPair();
    long double fetchInputValue(int idx) const;
};

void registerIfElseNodeType();

#endif // IFELSENODE_H

