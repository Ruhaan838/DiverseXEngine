//
// Created by Ruhaan on 31/07/25.
//

#ifndef COMMON_H
#define COMMON_H
#include <QColor>
#include <QString>
#include <QRandomGenerator>

inline int EDGE_CP_ROUNDNESS = 100;

enum POSITION {
    LEFT_TOP = 1,
    LEFT_BOTTOM = 2,
    RIGHT_TOP = 3,
    RIGHT_BOTTOM = 4,
};

inline POSITION getPosition(int pos) {
    switch (pos) {
        case 1: return LEFT_TOP;
        case 2: return LEFT_BOTTOM;
        case 3: return RIGHT_TOP;
        case 4: return RIGHT_BOTTOM;
        default: return LEFT_TOP;
    }
}

enum EDGETYPES {
    EDGE_TYPE_DIRECT = 1,
    EDGE_TYPE_BEZIER = 2
};

inline EDGETYPES getEdgeEnum(int pos) {
    switch (pos) {
        case 1: return EDGE_TYPE_DIRECT;
        case 2: return EDGE_TYPE_BEZIER;
        default: return EDGE_TYPE_BEZIER;
    }
}

// Socket type names as strings
inline QStringList getSocketTypeNames() {
    return {"Input", "Output", "Data", "Control", "Signal", "Event", "Trigger", "Parameter"};
}

inline QString getSocketType(int pos) {
    QStringList types = getSocketTypeNames();
    if (pos >= 1 && pos <= types.size()) {
        return types[pos - 1];
    }
    return types[0]; // Default to first type
}

// Generate random color for sockets
inline QColor getSocketColor(const QString& socketType = "") {
    // Generate random RGB values
    int r = QRandomGenerator::global()->bounded(50, 255);   // Avoid too dark colors
    int g = QRandomGenerator::global()->bounded(50, 255);
    int b = QRandomGenerator::global()->bounded(50, 255);

    return {r, g, b, 255}; // Full opacity
}

// Alternative: Get consistent color based on socket type string
inline QColor getSocketColorByType(const QString& socketType) {
    // Use hash of string to generate consistent color for same type
    uint hash = qHash(socketType);
    int r = static_cast<int>((hash & 0xFF0000) >> 16);
    int g = static_cast<int>((hash & 0x00FF00) >> 8);
    int b = static_cast<int>(hash & 0x0000FF);

    // Ensure colors are not too dark
    r = qMax(r, 50);
    g = qMax(g, 50);
    b = qMax(b, 50);

    return {r, g, b, 255};
}

enum EDGEDRAGMODS {
    MODE_NO_OP = 1,
    MODE_EDGE_DRAG = 2,
    MODE_EDGE_CUT = 3,

    EDGE_DRAG_START_THRESHOLD = 10
};

inline EDGEDRAGMODS getEdgeDragMode(int pos) {
    switch (pos) {
        case 1: return MODE_NO_OP;
        case 2: return MODE_EDGE_DRAG;
        case 3: return MODE_EDGE_CUT;
        case 4: return EDGE_DRAG_START_THRESHOLD;
        default: return MODE_NO_OP;
    }
}


#endif //COMMON_H
