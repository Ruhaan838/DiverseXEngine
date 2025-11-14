//
// Created by Ruhaan on 18/08/25.
//

#include "bitOps/BitOp.h"
#include "noderegister.h"
#include "../nodes/node.h"
#include "inout/inoutNode.h"
#include "function/functionNode.h"
#include "function/conditionNode.h"
#include "function/ifElseNode.h"
#include "canvasNode.h"
#include "arithmetic/mathNode.h"
#include "permutation/Permutation.h"
#include "inout/printNode.h"
#include "matrix/matrixNode.h"

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

    // Matrix nodes
    NodeRegistery::registerNode("Input Matrix", NodeCategory::MATRIX,
        [](Scene* scene) -> Node* {
            return new MatrixInputNode(scene, "Input Matrix",
                std::vector<QString>{}, std::vector<QString>{QString("Output")});
        },
        260, 280);

    NodeRegistery::registerNode("Matrix Determinant 2x2", NodeCategory::MATRIX,
        [](Scene* scene) -> Node* {
            return new Determinant2x2Node(scene, "Matrix Determinant 2x2",
                std::vector<QString>{"Matrix"},
                std::vector<QString>{"Output"});
        });

    NodeRegistery::registerNode("Matrix Index", NodeCategory::MATRIX,
        [](Scene* scene) -> Node* {
            return new MatrixIndexNode(scene, "Matrix Index",
                std::vector<QString>{"Matrix"},
                std::vector<QString>{"Value"});
        });

    // Containers
    NodeRegistery::registerNode("Canvas", NodeCategory::CONTAINER,
        [](Scene* scene) -> Node* {
            return new CanvasNode(scene, "Canvas",
                std::vector<QString>{"Input (bool)"});
        },
        300, 200);

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

    // If/Else node with nested else-if support
    NodeRegistery::registerNode("If / Else", NodeCategory::FUNCTION,
        [](Scene* scene) -> Node* {
            return new IfElseNode(scene, "If / Else");
        },
        260, 160);

    // Register Print If node under Function category
    NodeRegistery::registerNode("Print If", NodeCategory::FUNCTION,
        [](Scene* scene) -> Node* {
            return new PrintIfNode(scene, "Print If",
                std::vector<QString>{"Condition (Bool)"},
                std::vector<QString>{});
        },
        240, 120);

    NodeRegistery::registerNode("Input", NodeCategory::INPUT,
        [](Scene* scene) -> Node* {
            return new InputNode(scene, "Input", std::vector<QString>{""});
        });

    NodeRegistery::registerNode("Output", NodeCategory::OUTPUT,
        [](Scene* scene) -> Node* {
            return new OutputNode(scene, "Output", std::vector<QString>{""});
        });

    NodeRegistery::registerNode("Bool Output", NodeCategory::OUTPUT,
        [](Scene* scene) -> Node* {
            return new BoolOutputNode(scene, "Bool Output", std::vector<QString>{""});
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

    // Matrix types for deserialization
    Node::registerType("Determinant2x2Node", [](Scene* scene){ return new Determinant2x2Node(scene); });
    Node::registerType("MatrixInputNode", [](Scene* scene){ return new MatrixInputNode(scene); });
    Node::registerType("MatrixIndexNode", [](Scene* scene){ return new MatrixIndexNode(scene); });
    Node::registerType("CanvasNode", [](Scene* scene){ return new CanvasNode(scene); });

    Node::registerType("InputNode", [](Scene* scene){ return new InputNode(scene); });
    Node::registerType("OutputNode", [](Scene* scene){ return new OutputNode(scene); });
    Node::registerType("BoolOutputNode", [](Scene* scene){ return new BoolOutputNode(scene); });
    Node::registerType("ConditionNode", [](Scene* scene){ return new ConditionNode(scene); });
    // Register PrintIfNode type for deserialization
    Node::registerType("PrintIfNode", [](Scene* scene){ return new PrintIfNode(scene); });
    Node::registerType("GCDNode", [](Scene* scene){ return new GCDNode(scene); });
    Node::registerType("FactorialNode", [](Scene* scene){ return new FactorialNode(scene); });

    Node::registerType("AndNode", [](Scene* scene){ return new AndNode(scene); });
    Node::registerType("OrNode", [](Scene* scene){ return new OrNode(scene); });
    Node::registerType("NotNode", [](Scene* scene){ return new NotNode(scene); });
    Node::registerType("XorNode", [](Scene* scene){ return new XorNode(scene); });
    // IfElse node type for deserialization
    Node::registerType("IfElseNode", [](Scene* scene){ return new IfElseNode(scene); });
}
