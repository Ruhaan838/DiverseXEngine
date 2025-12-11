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
#include <QTabWidget>
#include <QList>
#include <QPushButton>

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

    QGraphicsView* getView() const { return currentView(); }
    Scene* getScene() const { return currentScene(); }
    CanvasScene* getGraphicsScene() const { return currentGraphicsScene(); }

private slots:
    void onFileOpen();
    void onFileSave();
    void onFileSaveAs();
    void onToggleNodes();
    void onToggleCodeEditor();
    void onRun();
    void onRefresh();
    void onNewTab();
    void onTabChanged(int index);
    void onTabCloseRequested(int index);

private:
    void centerOnScreen();
    void setupLeftIconPanel(); // menu bar removed
    void updateLeftPanelTheme(); // adjust colors when both panels are visible
    void installTabCloseButton(int index); // custom close button per tab

    // Per-tab bundle
    struct Tab {
        QWidget* container{nullptr};
        QSplitter* splitter{nullptr};
        NodeRegistery* nodeList{nullptr};
        CanvasView* view{nullptr};
        EditorWindow* editor{nullptr};
        Scene* scene{nullptr};
        CanvasScene* grScene{nullptr};
        QString currentFilePath; // optional: last opened/saved file
    };

    // Helpers to access current tab components safely
    Tab* currentTab() const;
    QGraphicsView* currentView() const;
    Scene* currentScene() const;
    CanvasScene* currentGraphicsScene() const;

    // Tab lifecycle
    int createTab(const QString& title = QString());
    void setTabTitle(int index, const QString& title);

    QVBoxLayout* layout{}; // top-level vertical layout
    QHBoxLayout* centralLayout{}; // holds left panel + tabs
    QTabWidget* tabs{};
    QList<Tab*> tabList;

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
    QToolButton* newTabButton{};

    struct Config {
        int windowWidth = 1200;
        int windowHeight = 800;
        QString windowTitle = "DiversXEngine";
        Qt::Orientation splitterOrientation = Qt::Horizontal;
        int leftPanelWidth = 35; // fixed width similar to IDE icon bar
    } config;
};

#endif //MAINWINDOW_H