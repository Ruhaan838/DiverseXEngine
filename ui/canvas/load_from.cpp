//
// Created by Ruhaan on 15/09/25.
//

#include "open_load.h"
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QWidget>
#include "../../core/scene/nodescene.h"

namespace FileIO {
    static QString lastPath;

    static QString ensureJsonExtension(const QString &path) {
        if (path.isEmpty()) return path;
        if (!path.endsWith(".json", Qt::CaseInsensitive)) return path + ".json";
        return path;
    }

    bool saveScene(Scene* scene, QWidget* parent, bool forceDialog) {
        if (!scene) return false;
        QString chosenPath = lastPath;

        if (forceDialog || chosenPath.isEmpty()) {
            QString suggest = chosenPath.isEmpty() ? QStringLiteral("Graph.json") : QFileInfo(chosenPath).fileName();
            QString newPath = getSaveFilePath(parent, suggest);
            if (newPath.isEmpty()) return false;
            chosenPath = newPath;
        }

        chosenPath = ensureJsonExtension(chosenPath);

        scene->saveToFile(chosenPath.toStdString());
        lastPath = chosenPath;
        return true;
    }

    QString loadScene(Scene* scene, QWidget* parent) {
        if (!scene) return QString();
        QString path = getOpenFilePath(parent);
        if (path.isEmpty()) return QString();
        path = ensureJsonExtension(path);
        scene->loadFromFile(path.toStdString());
        lastPath = path;

        // Extract just the filename from the full path
        QFileInfo fileInfo(path);
        return fileInfo.baseName(); // Returns filename without extension
    }
}
