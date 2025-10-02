//
// Created by Ruhaan on 02/10/25.
//

#ifndef PERMUTATION_H
#define PERMUTATION_H
#include "functionNode.h"

class GCDNode : public FunctionNode {
public:
    GCDNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class FactorialNode : public FunctionNode {
public:
    FactorialNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

#endif //PERMUTATION_H
