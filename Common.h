//
// Created by Ruhaan on 31/07/25.
//

#ifndef COMMON_H
#define COMMON_H
#include <QColor>

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

enum SOCKETTYPES {
    COLOR_1 = 1,
    COLOR_2 = 2,
    COLOR_3 = 3,
    COLOR_4 = 4,
    COLOR_5 = 5,
    COLOR_6 = 6,
};

inline SOCKETTYPES getSocketType(int pos) {
    switch (pos) {
        case 1: return COLOR_1;
        case 2: return COLOR_2;
        case 3: return COLOR_3;
        case 4: return COLOR_4;
        case 5: return COLOR_5;
        case 6: return COLOR_6;
        default: return COLOR_1;
    }
}

inline QColor getSocketColor(SOCKETTYPES type) {
    switch (type) {
        case COLOR_1: return QColor("#FFFF7700");
        case COLOR_2: return QColor("#FF52e220");
        case COLOR_3: return QColor("#FF0056a6");
        case COLOR_4: return QColor("#FFa86db1");
        case COLOR_5: return QColor("#FF40E0D0");
        case COLOR_6: return QColor("#FFdbe220");
        default: return QColor("#FFFF7700");
    }
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


#endif //COMMAN_H
