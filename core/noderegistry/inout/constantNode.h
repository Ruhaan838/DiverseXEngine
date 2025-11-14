// filepath: /Users/ruhaan/Documents/DiverseXEngine/core/noderegistry/inout/constantNode.h
//
// Created by Copilot on 14/11/25.
//

#ifndef CONSTANTNODE_H
#define CONSTANTNODE_H

#include <QLineEdit>
#include "../../nodes/node.h"
#include "../../widgets/widgets.h"

class ConstantNode : public Node {
public:
    ConstantNode(Scene* scene_, const std::string &title = "Constant", std::vector<QString> output_size = {});

    long double vals = 0.0L;
    QLineEdit *lineEdit = nullptr;

    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t> &hashmap) override;
};

#endif // CONSTANTNODE_H

