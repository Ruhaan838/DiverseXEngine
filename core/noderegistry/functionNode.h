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
    FunctionNode(Scene* scene_ = nullptr, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {}, vector<SOCKETTYPES> output_size = {});
    Node* getPrevNode(const int idx) const;
    virtual void execute() = 0;
    virtual long double getValues() = 0;
    long double getNodeValue(Node* node);
};

class AddNode : public FunctionNode {
public:
    AddNode(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {}, vector<SOCKETTYPES> output_size = {});

    long double vals = 0;
    void execute() override;
    long double getValues() override {return vals;};

};

class SubNode : public FunctionNode {
public:
    SubNode(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {}, vector<SOCKETTYPES> output_size = {});

    long double vals = 0;
    void execute() override;
    long double getValues() override {return vals;};
};

class MulNode : public FunctionNode {
public:
    MulNode(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {}, vector<SOCKETTYPES> output_size = {});

    long double vals = 0;
    void execute() override;
    long double getValues() override {return vals;};
};

class DivNode : public FunctionNode {
public:
    DivNode(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {}, vector<SOCKETTYPES> output_size = {});

    long double vals = 0;
    void execute() override;
    long double getValues() override {return vals;};
};

#endif //FUNCTIONNODE_H
