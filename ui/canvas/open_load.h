//
// Created by Ruhaan on 15/09/25.
//

#ifndef OPEN_LOAD_H
#define OPEN_LOAD_H

#include <QString>

class QWidget;
class Scene;

namespace FileIO {
    QString getSaveFilePath(QWidget* parent, const QString& suggestedName = QString());
    QString getOpenFilePath(QWidget* parent);

    bool saveScene(Scene* scene, QWidget* parent, bool forceDialog = false);
    QString loadScene(Scene* scene, QWidget* parent);
}

#endif // OPEN_LOAD_H
