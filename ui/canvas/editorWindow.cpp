//
// Created by Ruhaan on 29/08/25.
//

#include "editorWindow.h"

#include <QLabel>
#include <QVBoxLayout>


EditorWindow::EditorWindow(QWidget* parent) : QWidget(parent) {
    setFixedWidth(420);
    QLabel* label = new QLabel("Text", this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(label);

    setLayout(layout);

}
