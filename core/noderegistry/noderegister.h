//
// Created by Ruhaan on 18/08/25.
//

#ifndef NODEREGISTER_H
#define NODEREGISTER_H
#include <QListWidget>
#include <QTreeWidget>
#include <QHash>
#include <QString>
#include <vector>
#include <functional>

using namespace std;

class Scene;
class Node;

enum class NodeCategory {
    FUNCTION,
    INPUT,
    OUTPUT,
    ARITHMETIC,
    MATH,
    PERMUTATION,
    BITOPERATIONS,
};

struct NodeInfo {
    QString name;
    NodeCategory category;
    std::function<Node*(Scene*)> creator;
    int width = 180;
    int height = 90;
};

class NodeRegistery : public QTreeWidget {
public:
    NodeRegistery(Scene* parant = nullptr);

    static void registerNode(const QString& name, NodeCategory category,
                           const std::function<Node*(Scene*)>& creator,
                           int width = 180, int height = 90);

    static QList<NodeInfo> getNodesByCategory(NodeCategory category);

    static Node* createNode(const QString& name, Scene* scene);

private:
    void buildUI();
    void setupDoubleClickHandler();

    static QHash<QString, NodeInfo>& getRegistry();

    QTreeWidgetItem* functions;
    QTreeWidgetItem* arithmetic;
    QTreeWidgetItem* math;
    QTreeWidgetItem* permutation;
    QTreeWidgetItem* bitoperations;
    QTreeWidgetItem* inputs;
    QTreeWidgetItem* outputs;
    QTreeWidgetItem* containers;

    Scene* scene;

    QHash<QString, Node*> getfunctionalNode() const;
    vector<Node*> getInputNode() const;
    vector<Node*> getOutputNode() const;
};

#define REGISTER_NODE(name, category, nodeClass, ...) \
    static const bool name##_registered = (NodeRegistery::registerNode( \
        #name, NodeCategory::category, \
        [](Scene* scene) -> Node* { return new nodeClass(scene, ##__VA_ARGS__); }), true);

#endif //NODEREGISTER_H
