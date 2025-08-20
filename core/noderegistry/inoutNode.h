//
// Created by Ruhaan on 19/08/25.
//

#ifndef INPUTNODE_H
#define INPUTNODE_H
#include "../nodes/node.h"
#include "../widgets/widgets.h"

class InputNode : public Node {
public:
    InputNode(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> output_size = {});
    long double vals = 0;
    long double getValues() {return vals;};
};

class OutputNode : public Node {
public:
    OutputNode(Scene* scene_, const string &title = "Undefine Node", vector<SOCKETTYPES> input_size = {});
    void execute();
    long double vals = 0;
    TextEdit* textEdit = nullptr;
};

#endif //INPUTNODE_H
