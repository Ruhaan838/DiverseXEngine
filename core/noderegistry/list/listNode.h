#ifndef LISTNODE_H
#define LISTNODE_H

#include "../../nodes/node.h"
#include <vector>
#include <QJsonObject>

class QWidget;
class QScrollArea;
class QVBoxLayout;
class QPushButton;
class QLineEdit;

// Simple provider so later function nodes can consume list values
class ListProvider {
public:
    virtual ~ListProvider() = default;
    virtual std::vector<double> getListValues() const = 0;
    virtual int count() const = 0;
};

class ListInputNode : public Node, public ListProvider {
public:
    ListInputNode(Scene* scene_, const std::string &title = "Input List",
                  std::vector<QString> input_size = {}, std::vector<QString> output_size = {QString("List")});

    // Provider interface
    std::vector<double> getListValues() const override { return values; }
    int count() const override { return static_cast<int>(values.size()); }

    // Serialization
    QJsonObject serialize() override;
    bool deserialize(const QJsonObject &data, std::unordered_map<std::string, uintptr_t>& hashmap) override;

    // Reset values
    void resetValues();

private:
    void buildContent();
    void addEntry(double initial = 0.0);
    void removeLastEntry();
    void rebuildEntries();

    std::vector<double> values;

    // UI cached widgets
    QWidget* container = nullptr; // main widget
    QScrollArea* scroll = nullptr;
    QWidget* listHolder = nullptr; // inside scroll
    QVBoxLayout* listLayout = nullptr;
    QPushButton* addBtn = nullptr;

    std::vector<QLineEdit*> edits;
};

#endif // LISTNODE_H

