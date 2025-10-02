#ifndef CANVASNODE_H
#define CANVASNODE_H

#include "../nodes/node.h"
#include <vector>

class CanvasNode : public Node {
public:
    CanvasNode(Scene* scene_ = nullptr, const std::string &title = "Canvas", std::vector<QString> input_size = {"Input"}, std::vector<QString> output_size = {"Output"});

    void show() override;

    void addNode(Node* node);
    void removeNode(Node* node);

    void updateSizeToFitChildren();
    void tryEmbedSelectedNodes();

    std::vector<Node*> inner_nodes;

    int child_padding = 20;
};

void registerCanvasNodeType();

#endif // CANVASNODE_H
