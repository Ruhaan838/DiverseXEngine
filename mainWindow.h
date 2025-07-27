//
// Created by Ruhaan on 27/07/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QGraphicsView>
#include <QVBoxLayout>
#include "Canvas/Scene/CanvasScene.h"

class MainWindow : public QWidget {
public:
    MainWindow(QWidget *parent = nullptr);
    void initUI();

    QGraphicsView* view;
    QVBoxLayout* layout;
    CanvasScene *grScene;
};

#endif //MAINWINDOW_H
