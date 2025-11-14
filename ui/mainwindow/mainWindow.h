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
#include <QToolButton>
#include <QFrame>
#include <QHBoxLayout>

// Forward declarations
class NodeRegistery; // needed for pointer member
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
    void setWindowTitleWithFilename(const QString& filename);

    QGraphicsView* getView() const { return view; }
    Scene* getScene() const { return scene; }
    CanvasScene* getGraphicsScene() const { return grScene; }

private slots:
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onToggleNodes();
    void onToggleCodeEditor();
    void onRun();
    void onRefresh();

private:
    void centerOnScreen();
    void setupLeftIconPanel(); // menu bar removed
    void updateLeftPanelTheme(); // adjust colors when both panels are visible

    QGraphicsView* view{};
    QVBoxLayout* layout{}; // top-level vertical layout
    QHBoxLayout* centralLayout{}; // holds left panel + splitter
    CanvasScene* grScene{};
    Scene* scene{};
    QSplitter* splitter{};
    EditorWindow* editWin{};
    NodeRegistery* nodeList{};

    QWidget* leftPanel{};
    QVBoxLayout* leftPanelLayout{};
    QFrame* leftSeparator{};

    // Tool buttons
    QToolButton* runButton{};
    QToolButton* toggleCodeButton{};
    QToolButton* toggleNodesButton{};
    QToolButton* openButton{};
    QToolButton* saveButton{};
    QToolButton* saveAsButton{};
    QToolButton* refreshButton{};

    struct Config {
        int windowWidth = 1200;
        int windowHeight = 800;
        QString windowTitle = "DiversXEngine";
        Qt::Orientation splitterOrientation = Qt::Horizontal;
        int leftPanelWidth = 35; // fixed width similar to IDE icon bar
    } config;
};

#endif //MAINWINDOW_H