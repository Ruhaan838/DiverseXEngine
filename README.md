# DiverseXEngine

## Overview

### Purpose and Scope
DiversXEngine is Qt-based visual node programming editor that enables users to create and execute computational graphs through an intuative drag-and-drop interface.

### System Architecture
**DiverseXEngine System Architecture**

```mermaid
graph TD
  %% ===================== Application Layer =====================
  subgraph Application_Layer["Application Layer"]
    main_cpp[main.cpp]
    MainWindow[MainWindow]
    NodeRegistry[NodeRegistry]
    main_cpp --> MainWindow
    MainWindow --> NodeRegistry
  end

  %% ===================== UI / Canvas Layer =====================
  subgraph UI_Canvas_Layer["UI / Canvas Layer"]
    CanvasView[CanvasView]
    CanvasScene[CanvasScene]
    Helper[Helper]
    CanvasView --> CanvasScene
    CanvasView --> Helper
  end

  MainWindow --> CanvasView

  %% ===================== Graphics Layer =====================
  subgraph Graphics_Layer["Graphics Layer"]
    SocketGraphics[SocketGraphics]
    EdgeGraphics[EdgeGraphics]
    NodeGraphics[NodeGraphics]
    CutlineGraphics[CutlineGraphics]
    CanvasScene --> SocketGraphics
    CanvasScene --> EdgeGraphics
    CanvasScene --> NodeGraphics
    CanvasScene --> CutlineGraphics
  end

  %% ===================== Core Data Model =====================
  subgraph Core_Data_Model["Core Data Model"]
    Scene[Scene]
    EdgesNode[EdgesNode]
    Node[Node]
    SocketNode[SocketNode]
    Scene --> Node
    Scene --> EdgesNode
  end

  %% Connections from graphics to data-model (dotted)
  SocketGraphics -.-> SocketNode
  EdgeGraphics   -.-> EdgesNode
  NodeGraphics   -.-> Node

  %% CanvasScene maps to Scene (solid)
  CanvasScene --> Scene

  %% ===================== Persistence =====================
  subgraph Persistence["Persistence"]
    Serializable[Serializable]
    Serializer[Serializer]
    JSON_Files[JSON Files]
    Serializable --> Serializer
    Serializer --> JSON_Files
  end

  %% Data model -> persistence (dotted)
  Scene     -.-> Serializable
  Node      -.-> Serializable
  EdgesNode -.-> Serializable
  SocketNode-.-> Serializable
  EdgesNode --> SocketNode

  %% ===================== Widget System =====================
  subgraph Widget_System["Widget System"]
    WidgetNode[WidgetNode]
    TextEdit[TextEdit]
    WidgetNode --> TextEdit
  end

  Node --> WidgetNode


```

### Core Components
The engine is organized into several key subsystem that work together to provide visual node pragamming capablities:

| Component| Purpose| Key Classes|
|---------------|--------------------------------|------------------------------|
|Application Layer| Application entry point and main UI| `main.cpp`, `MainWindow`, `NodeRegistry`|
|Canvas System| Interactive editing area for node manipulation| `CanvasView`, `CanvasScene`, `Helper`   |
|Graphics Layer| Visual representation of graph elements| `NodeGraphics`, `EdgeGraphics`, `SocketGraphics`|
|Data Model| Core graph data structures and logic| `Scene`, `Node`, `EdgesNode`, `SocketNode`|
|Widget System| Custom UI components within nodes| `WidgetNode`, `TextEdit`|
|Save and Load| Serialization and file I/O operations|`Serializable`, `Serializator`|

### Node-Based Programming Model
DataFlow for programming as below:


```mermaid
graph TD
  subgraph Input_Nodes
    in1[InputNode]
  end

  subgraph Connection_System
    sock1[SocketNode]
    edge1[EdgesNode]
    sock2[SocketNode]
    edge2[EdgesNode]
  end

  subgraph Function_Nodes
    add[AddNode]
    mul[MulNode]
    sub[SubNode]
    div[DivNode]
  end

  subgraph Output_Nodes
    out1[OutputNode]
  end

in1-->sock1-->edge1
edge1-->add-->sock2
edge1-->sub-->sock2
edge1-->mul-->sock2
edge1-->div-->sock2

sock2-->edge2-->out1




  
```

### Current Features
- **Visual Node Editor:** Canvas with drag-and-drop-node manipulation
- **Edge Connection System:** Connection between node socket with graphics rendering
- **Node Selection and Deletion:** Multi-select operations and deletion.
- **Math Operations:** Addition(`AddNode`), Subtraction(`SubNode`), multiplication(`MulNode`), and division(`DivNode`) nodes
- **Edge Cutting:** Interactive edge delection using cutting tools
- **Serialization:** Save and load node graphs to/from JSON format
- **Socket System:** Typed connection points with multiple socket types

### Techonolgy Stack

- **Qt Framework**: Cross-platform GUI framework.
- **Qt Graphics View**: Scene-based graphics randering for node editor.
- **C++**: core implementation language
- **JSON**: Graph serialization and deserialization.

##
