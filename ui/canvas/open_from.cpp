//
// Created by Ruhaan on 15/09/25.
//

#include "open_load.h"
#include <QFileDialog>
#include <QWidget>
#include <QStandardPaths>

namespace FileIO {
    QString getSaveFilePath(QWidget* parent, const QString& suggestedName) {
        QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        if (!suggestedName.isEmpty()) {
            defaultPath += "/" + suggestedName;
        }

        return QFileDialog::getSaveFileName(
            parent,
            "Save Scene",
            defaultPath,
            "JSON Files (*.json);;All Files (*)"
        );
    }

    QString getOpenFilePath(QWidget* parent) {
        QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);

        return QFileDialog::getOpenFileName(
            parent,
            "Open Scene",
            defaultPath,
            "JSON Files (*.json);;All Files (*)"
        );
    }
}
