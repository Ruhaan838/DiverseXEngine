#ifndef CANVASNODE_H
#define CANVASNODE_H

#include "../nodes/node.h"
#include <vector>
#include <utility>
#include <QPointF>

class CanvasNode : public Node {
public:
    CanvasNode(Scene* scene_ = nullptr, const std::string &title = "Canvas", std::vector<QString> input_size = {"Input (bool)"}, std::vector<QString> output_size = {});

    void show() override;

    void addNode(Node* node);
    void removeNode(Node* node);

    void updateSizeToFitChildren();
    void tryEmbedSelectedNodes();

    // Returns true if Canvas boolean input evaluates true (header flag unused for gating now)
    bool isEnabled();

    // Serialize including inner child membership
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t>& hashmap) override;

    void setHeaderEnabled(bool enabled) { headerEnabled = enabled; }
    bool getHeaderEnabled() const { return headerEnabled; }

    std::vector<Node*> inner_nodes;

    int child_padding = 20;

    // Filled during deserialize; processed later by Scene to embed children.
    std::vector<std::pair<uintptr_t, QPointF>> pending_children; // id + local pos

private:
    bool headerEnabled = false;
};

void registerCanvasNodeType();

#endif // CANVASNODE_H
