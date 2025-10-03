//
// Created by Ruhaan on 18/08/25.
//

#include "BitOp.h"
#include "noderegister.h"
#include "../nodes/node.h"
#include "inoutNode.h"
#include "functionNode.h"
#include "conditionNode.h"
#include "canvasNode.h"
#include "mathNode.h"
#include "Permutation.h"

void registerAllNodeTypes() {
    // Arithmetic nodes
    NodeRegistery::registerNode("Addition", NodeCategory::ARITHMETIC,
        [](Scene* scene) -> Node* {
            return new AddNode(scene, "Addition",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Subtract", NodeCategory::ARITHMETIC,
        [](Scene* scene) -> Node* {
            return new SubNode(scene, "Subtract",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Multiply", NodeCategory::ARITHMETIC,
        [](Scene* scene) -> Node* {
            return new MulNode(scene, "Multiply",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Divide", NodeCategory::ARITHMETIC,
        [](Scene* scene) -> Node* {
            return new DivNode(scene, "Divide",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    // Math nodes
    NodeRegistery::registerNode("Power", NodeCategory::MATH,
        [](Scene* scene) -> Node* {
            return new PowNode(scene, "Power",
                std::vector<QString>{"Base", "Exponent"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Modulo", NodeCategory::MATH,
        [](Scene* scene) -> Node* {
            return new ModNode(scene, "Modulo",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Square Root", NodeCategory::MATH,
        [](Scene* scene) -> Node* {
            return new SqrtNode(scene, "Square root",
                std::vector<QString>{"Number"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Sinusoidal (Sin)", NodeCategory::MATH,
        [](Scene* scene) -> Node* {
            return new SinNode(scene, "Sinusoidal (Sin)",
                std::vector<QString>{"Number"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Cosinusoidal (Cos)", NodeCategory::MATH,
        [](Scene* scene) -> Node* {
            return new CosNode(scene, "Cosinusoidal (Cos)",
                std::vector<QString>{"Number"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Tangent (Tan)", NodeCategory::MATH,
        [](Scene* scene) -> Node* {
            return new TanNode(scene, "Tangent (Tan)",
                std::vector<QString>{"Number"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Greatest Common Divisor (GCD)", NodeCategory::PERMUTATION,
        [](Scene* scene) -> Node* {
            return new GCDNode(scene, "Greatest Common Divisor (GCD)",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Factorial", NodeCategory::PERMUTATION,
        [](Scene* scene) -> Node* {
            return new FactorialNode(scene, "Factorial",
                std::vector<QString>{"Number"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Bitwise-AND", NodeCategory::BITOPERATIONS,
        [](Scene* scene) -> Node* {
            return new AndNode(scene, "Bitwise-AND",
                std::vector<QString>{"Number 1", "Number 2"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Bitwise-OR", NodeCategory::BITOPERATIONS,
       [](Scene* scene) -> Node* {
           return new OrNode(scene, "Bitwise-OR",
               std::vector<QString>{"Number 1", "Number 2"},
               std::vector<QString>{"Output"});
       });

    NodeRegistery::registerNode("Bitwise-NOT", NodeCategory::BITOPERATIONS,
       [](Scene* scene) -> Node* {
           return new NotNode(scene, "Bitwise-NOT",
               std::vector<QString>{"Number"},
               std::vector<QString>{"Output"});
       });

    NodeRegistery::registerNode("Bitwise-XOR", NodeCategory::BITOPERATIONS,
       [](Scene* scene) -> Node* {
           return new XorNode(scene, "Bitwise-XOR",
               std::vector<QString>{"Number 1", "Number 2"},
               std::vector<QString>{"Output"});
       });

    NodeRegistery::registerNode("Condition", NodeCategory::FUNCTION,
        [](Scene* scene) -> Node* { return new ConditionNode(scene, "Condition"); },
        200, 150);

    NodeRegistery::registerNode("Input", NodeCategory::INPUT,
        [](Scene* scene) -> Node* {
            return new InputNode(scene, "Input", std::vector<QString>{""});
        });

    NodeRegistery::registerNode("Output", NodeCategory::OUTPUT,
        [](Scene* scene) -> Node* {
            return new OutputNode(scene, "Output", std::vector<QString>{""});
        });


    Node::registerType("AddNode", [](Scene* scene){ return new AddNode(scene); });
    Node::registerType("SubNode", [](Scene* scene){ return new SubNode(scene); });
    Node::registerType("MulNode", [](Scene* scene){ return new MulNode(scene); });
    Node::registerType("DivNode", [](Scene* scene){ return new DivNode(scene); });

    Node::registerType("PowNode", [](Scene* scene){ return new PowNode(scene); });
    Node::registerType("ModNode", [](Scene* scene){ return new ModNode(scene); });
    Node::registerType("SqrtNode", [](Scene* scene){ return new SqrtNode(scene); });
    Node::registerType("SinNode", [](Scene* scene){ return new SinNode(scene); });
    Node::registerType("CosNode", [](Scene* scene){ return new CosNode(scene); });
    Node::registerType("TanNode", [](Scene* scene){ return new TanNode(scene); });

    Node::registerType("InputNode", [](Scene* scene){ return new InputNode(scene); });
    Node::registerType("OutputNode", [](Scene* scene){ return new OutputNode(scene); });
    Node::registerType("ConditionNode", [](Scene* scene){ return new ConditionNode(scene); });
    Node::registerType("GCDNode", [](Scene* scene){ return new GCDNode(scene); });
    Node::registerType("FactorialNode", [](Scene* scene){ return new FactorialNode(scene); });

    Node::registerType("AndNode", [](Scene* scene){ return new AndNode(scene); });
    Node::registerType("OrNode", [](Scene* scene){ return new OrNode(scene); });
    Node::registerType("NotNode", [](Scene* scene){ return new NotNode(scene); });
    Node::registerType("XorNode", [](Scene* scene){ return new XorNode(scene); });
}
