//
// Created by Ruhaan on 27/07/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QVBoxLayout>
#include <string>

#include "Canvas/Scene/CanvasScene.h"
#include "Canvas/View/CanvasView.h"

class MainWindow : public QWidget {
public:
    MainWindow(QWidget *parent = nullptr);
    void initUI();
    void addDebugContent();

    QGraphicsView* view{};
    QVBoxLayout* layout{};
    CanvasScene *grScene{};
    Scene *scene{};
};

#endif //MAINWINDOW_H
