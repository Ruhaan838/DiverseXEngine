//
// Created by Ruhaan on 02/10/25.
//

#ifndef MATHNODE_H
#define MATHNODE_H
#include "functionNode.h"

class PowNode : public FunctionNode {
public:
    PowNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class ModNode : public FunctionNode {
public:
    ModNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class SqrtNode : public FunctionNode {
public:
    SqrtNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class SinNode : public FunctionNode {
public:
    SinNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class CosNode : public FunctionNode {
public:
    CosNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class TanNode : public FunctionNode {
public:
    TanNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

#endif //MATHNODE_H
