//
// Created by Ruhaan on 02/10/25.
//

#include "Permutation.h"
#include <numeric>

GCDNode::GCDNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

FactorialNode::FactorialNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

void GCDNode::execute() {
    if (inputs.size() < 2) return;

    auto *baseNode = getPrevNode(0);
    auto *exponentNode = getPrevNode(1);

    if (!baseNode || !exponentNode) return;

    auto num1 = getNodeValue(baseNode);
    auto num2 = getNodeValue(exponentNode);

    vals = static_cast<double>(std::gcd(static_cast<long long>(num1), static_cast<long long>(num2)));
}

double factorial(double n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;
    double result = 1;
    for (int i = 2; i <= static_cast<int>(n); ++i) {
        result *= i;
    }
    return result;
}

void FactorialNode::execute() {

    auto *baseNode = getPrevNode(0);

    if (!baseNode) return;

    auto num1 = getNodeValue(baseNode);
    vals = factorial(num1);
}

QJsonObject GCDNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "PowNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject FactorialNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "PowNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool GCDNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool FactorialNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}