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

using namespace std;

class Scene;
class Node;

class NodeRegistery : public QTreeWidget {
public:
    NodeRegistery(Scene* parant = nullptr);

    void addFunctions();
    void addInputs();
    void addOutputs();

    QHash<QString, Node*> getfunctionalNode() const;
    vector<Node*> getInputNode() const;
    vector<Node*> getOutputNode() const;

    QTreeWidgetItem* functions;
    QTreeWidgetItem* inputs;
    QTreeWidgetItem* outputs;
    QTreeWidgetItem* containers;

    Scene* scene;
};

#endif //NODEREGISTER_H
