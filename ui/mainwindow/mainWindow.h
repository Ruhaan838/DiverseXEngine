//
// Created by Ruhaan on 12/08/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QSplitter>
#include <QWidget>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>

class EditorWindow;
class CanvasScene;
class Scene;
class CanvasView;

class MainWindow : public QWidget {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void initUI();

    void setWindowDimensions(int width, int height);
    void setWindowTitle(const QString& title);
    void setWindowTitleWithFilename(const QString& filename); // New method for filename title

    QGraphicsView* getView() const { return view; }
    Scene* getScene() const { return scene; }
    CanvasScene* getGraphicsScene() const { return grScene; }

private slots:
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();

private:
    void centerOnScreen();
    void setupMenuBar();
    void setupFileActions();

    QGraphicsView* view{};
    QVBoxLayout* layout{};
    CanvasScene* grScene{};
    Scene* scene{};
    QSplitter* splitter{};
    EditorWindow* editWin{};

    QMenuBar* menuBar{};
    QAction* openAction{};
    QAction* saveAction{};
    QAction* saveAsAction{};

    struct Config {
        int windowWidth = 1200;
        int windowHeight = 800;
        QString windowTitle = "DiversXEngine";
        Qt::Orientation splitterOrientation = Qt::Horizontal;

    } config;
};

#endif //MAINWINDOW_H