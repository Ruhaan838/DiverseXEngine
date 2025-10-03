//
// Created by Ruhaan on 02/10/25.
//

#include "mathNode.h"
#include <cmath>

PowNode::PowNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

ModNode::ModNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

SqrtNode::SqrtNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

SinNode::SinNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

CosNode::CosNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

TanNode::TanNode(Scene *scene_, const string &title, vector<QString> input_size, vector<QString> output_size)
: FunctionNode(scene_, title, input_size, output_size, false) {
    vals = 0;
}

void PowNode::execute() {

    auto *baseNode = getPrevNode(0);
    auto *exponentNode = getPrevNode(1);

    if (!baseNode || !exponentNode) return;

    const int base = static_cast<int>(getNodeValue(baseNode));
    const int exponent = static_cast<int>(getNodeValue(exponentNode));

    vals = pow(base, exponent);
}

void ModNode::execute() {

    auto *baseNode = getPrevNode(0);
    auto *exponentNode = getPrevNode(1);

    if (!baseNode || !exponentNode) return;

    const int num1 = static_cast<int>(getNodeValue(baseNode));
    const int num2 = static_cast<int>(getNodeValue(exponentNode));

    vals = num1 % num2;
}


void SqrtNode::execute() {

    auto *baseNode = getPrevNode(0);

    if (!baseNode) return;

    double num1 = getNodeValue(baseNode);
    vals = sqrt(num1);
}

void SinNode::execute() {

    auto *baseNode = getPrevNode(0);

    if (!baseNode) return;

    double num1 = getNodeValue(baseNode);
    vals = sin(num1);
}

void CosNode::execute() {

    auto *baseNode = getPrevNode(0);

    if (!baseNode) return;

    double num1 = getNodeValue(baseNode);
    vals = cos(num1);
}

void TanNode::execute() {

    auto *baseNode = getPrevNode(0);

    if (!baseNode) return;

    double num1 = getNodeValue(baseNode);
    vals = tan(num1);
}

QJsonObject PowNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "PowNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject ModNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "ModNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject SqrtNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "SqrtNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject SinNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "SinNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject CosNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "CosNode";
    old_obj["value"] = vals;
    return old_obj;
}

QJsonObject TanNode::serialize() {
    auto old_obj = FunctionNode::serialize();
    old_obj["node_type"] = "TanNode";
    old_obj["value"] = vals;
    return old_obj;
}

bool PowNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool ModNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool SqrtNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool SinNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool CosNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}

bool TanNode::deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) {
    vals = data["value"].toDouble();
    return FunctionNode::deserialize(data, hashmap);
}


