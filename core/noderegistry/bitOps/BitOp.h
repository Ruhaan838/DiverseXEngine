//
// Created by Ruhaan on 02/10/25.
//

#ifndef BITOP_H
#define BITOP_H
#include "../function/functionNode.h"

class AndNode : public FunctionNode {
public:
    AndNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class OrNode : public FunctionNode {
public:
    OrNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class NotNode : public FunctionNode {
public:
    NotNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class XorNode : public FunctionNode {
public:
    XorNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

#endif //BITOP_H
