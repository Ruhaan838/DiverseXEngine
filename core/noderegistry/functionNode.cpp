//
// Created by Ruhaan on 19/08/25.
//

#include "functionNode.h"

#include "inoutNode.h"
#include "../nodes/edge.h"
#include "../nodes/socket.h"


FunctionNode::FunctionNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
: Node(scene_, title, input_size, output_size){}

long double FunctionNode::getNodeValue(Node* node) {
    if (!node) return 0;
    if (auto* fn = dynamic_cast<FunctionNode*>(node)) {
        fn->execute();
        return fn->getValues();
    }
    if (auto* in = dynamic_cast<InputNode*>(node)) {
        return in->vals;
    }
    return 0;
}

Node* FunctionNode::getPrevNode(const int idx) const {
    if (this->inputs[idx] && this->inputs[idx]->hasEdge() && this->inputs[idx]->edge && this->inputs[idx]->edge->startSocket) {
        return this->inputs[idx]->edge->startSocket->node;
    }
    return nullptr;
}


/* ==========================================================
 This function Nodes must follow the rules:
 1. It must have the condition checking for the number of inputs in execute function and input_size and the
    input vector must be greater than or equal to the number of inputs required.
 2. It must get the node first and then convert it into the input node be using function getPrevNode(idx) at bound index.
 3. (optional) make serializing and deserializing methods for function node.
   ==========================================================
*/

// AddNode add the two numbers
AddNode::AddNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
: FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void AddNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        vals = v1 + v2;
    }
}

// SubNode subtracts the two numbers
SubNode::SubNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
    :FunctionNode(scene_, title, input_size, output_size){
    vals = 0;
}

void SubNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        vals = v1 - v2;
    }
}

// MulNode multiplies the two numbers
MulNode::MulNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
    :FunctionNode(scene_, title, input_size, output_size){
    vals = 0;
}

void MulNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        vals = v1 * v2;
    }
}

// DivNode divides the two numbers
DivNode::DivNode(Scene *scene_, const string &title, vector<SOCKETTYPES> input_size, vector<SOCKETTYPES> output_size)
    :FunctionNode(scene_, title, input_size, output_size) {
    vals = 0;
}

void DivNode::execute() {
    if (inputs.size() >= 2) {
        auto *prev1 = getPrevNode(0);
        auto *prev2 = getPrevNode(1);
        if (prev1 == nullptr || prev2 == nullptr) {
            return;
        }
        long double v1 = getNodeValue(prev1);
        long double v2 = getNodeValue(prev2);
        if (v2 != 0) {
            vals = v1 / v2;
        } else {
            vals = INFINITY;
        }
    }
}