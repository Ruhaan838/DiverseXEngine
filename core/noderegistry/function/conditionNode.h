//
// Created by Ruhaan on 14/09/25.
//

#ifndef CONDITIONNODE_H
#define CONDITIONNODE_H


#include <QComboBox>

#include "../../nodes/node.h"
#include "../../widgets/widgets.h"
#include "functionNode.h"

class InputNode;

class ConditionNode : public FunctionNode {
public:
    ConditionNode(Scene* scene_, const string &title = "Condition");

    bool evaluateCondition();

    void execute() override;

    double getValues() override;
    // Return value based on which output socket is queried: True -> 1 if condition true else 0; False -> opposite
    double getValuesForOutput(SocketNode* startSocket) override;
    void showContent() override;
    void hideContent() override;

    bool result = false;
    QComboBox* bool_list = nullptr;

    long double fetchInputValue(int idx) const;
    double activeValue = 0.0;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

void registerConditionNodeType();

#endif //CONDITIONNODE_H
