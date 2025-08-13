//
// Created by Ruhaan on 12/08/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QWidget>

class CanvasScene;
class Scene;

class MainWindow : public QWidget {
public:
    MainWindow(QWidget *parent = nullptr);
    void initUI();
    void addDebugContent();
    void addNodes();

    QGraphicsView* view{};
    QVBoxLayout* layout{};
    CanvasScene *grScene{};
    Scene *scene{};
};

#endif //MAINWINDOW_H
