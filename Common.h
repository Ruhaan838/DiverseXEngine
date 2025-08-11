//
// Created by Ruhaan on 31/07/25.
//

#ifndef COMMAN_H
#define COMMAN_H
#include <QColor>

inline int EDGE_CP_ROUNDNESS = 100;

enum POSITION {
    LEFT_TOP = 1,
    LEFT_BOTTOM = 2,
    RIGHT_TOP = 3,
    RIGHT_BOTTOM = 4,
};

enum EDGETYPES {
    EDGE_TYPE_DIRECT = 1,
    EDGE_TYPE_BEZIER = 2
};

enum SOCKETTYPES {
    COLOR_1 = 1,
    COLOR_2 = 2,
    COLOR_3 = 3,
    COLOR_4 = 4,
    COLOR_5 = 5,
    COLOR_6 = 6,
};

enum EDGEDRAGMODS {
    MODE_NO_OP = 1,
    MODE_EDGE_DRAG = 2,

    EDGE_DRAG_START_THRESHOLD = 10
};

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

#endif //COMMAN_H
