//
// Created by Ruhaan on 19/08/25.
//

#ifndef FUNCTIONNODE_H
#define FUNCTIONNODE_H

class Scene;
class InputNode;

#include <vector>
#include "../../Common.h"
#include "../nodes/node.h"
using namespace std;

class FunctionNode : public Node {
public:
    FunctionNode(Scene* scene_ = nullptr, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {}, bool allow_addsocket = true);
    Node* getPrevNode(const int idx) const;
    virtual void execute() = 0;
    virtual double getValues() = 0;
    double getNodeValue(Node* node);
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;

    virtual void showContent();
    virtual void hideContent();
    WidgetNode* widget;
    bool allow_addsocket = true;
};

class AddNode : public FunctionNode {
public:
    AddNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;

};

class SubNode : public FunctionNode {
public:
    SubNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;

};

class MulNode : public FunctionNode {
public:
    MulNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;

};

class DivNode : public FunctionNode {
public:
    DivNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {}, vector<QString> output_size = {});

    double vals = 0;
    void execute() override;
    double getValues() override {return vals;};
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;

};

void registerFunctionNodeType();

#endif //FUNCTIONNODE_H
