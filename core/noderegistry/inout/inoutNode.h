//
// Created by Ruhaan on 19/08/25.
//

#ifndef INPUTNODE_H
#define INPUTNODE_H
#include <QLineEdit>

#include "../../nodes/node.h"
#include "../../widgets/widgets.h"

class InputNode : public Node {
public:
    InputNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> output_size = {});
    double vals = 0;
    long double getValues() {return vals;};
    QLineEdit *lineEdit;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
};

class OutputNode : public Node {
public:
    OutputNode(Scene* scene_, const string &title = "Undefine Node", vector<QString> input_size = {});
    void execute();
    double vals = 0;
    TextEdit* textEdit = nullptr;
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
    Node* getPrevNode(int inputIndex);
};

class BoolOutputNode : public Node {
public:
    BoolOutputNode(Scene* scene_, const string &title = "Bool Output", vector<QString> input_size = {});
    void execute();
    bool bval = false;
    TextEdit* textEdit = nullptr;
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, unordered_map<string, uintptr_t> &hashmap) override;
    Node* getPrevNode(int inputIndex);
};

void registerInputNodeType();
void registerOutputNodeType();

#endif //INPUTNODE_H
