#include <QApplication>
#include "ui/mainwindow/mainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // Global dark theme stylesheet (refined)
    app.setStyleSheet(
        // Base
        "QWidget { background-color: #1e1f22; color: #e0e0e0; }"
        // Tabs
        "QTabWidget::pane { border: 1px solid #2b2d31; background: #232428; }"
        "QTabBar { font-weight: 500; }"
        "QTabBar::tab { background: #2a2c31; color: #cfd3da; padding: 6px 18px; padding-right: 44px; border: 1px solid #2f3136; border-bottom: none; margin-right: 2px; border-top-left-radius: 6px; border-top-right-radius: 6px; }"
        "QTabBar::tab:hover { background: #34363c; }"
        "QTabBar::tab:selected { background: #3a3d44; color: #ffffff; border: 1px solid #3f4249; }"
        // Bigger, round close button with circular background and correct origin
        // Use 'image' (Qt-supported) and fine-tune size/margins for macOS
        "QTabBar::close-button { subcontrol-origin: padding; subcontrol-position: right; image: url(:/icons/close.svg); margin-left: 10px; margin-right: 8px; margin-top: 1px; width: 20px; height: 20px; border-radius: 10px; background-color: rgba(255,255,255,0.08); }"
        "QTabBar::close-button:hover { background-color: rgba(255,255,255,0.16); }"
        "QTabBar::close-button:pressed { background-color: rgba(255,255,255,0.24); }"
        // Splitter
        "QSplitter::handle { background-color: #2b2d31; }"
        // Node list (tree)
        "QTreeWidget { background: #1f2023; border: 1px solid #2b2d31; }"
        "QTreeWidget::item { height: 22px; }"
        "QTreeWidget::item:selected { background: #34404a; color: #ffffff; }"
        "QHeaderView::section { background: #232428; color: #bfc5cf; border: none; padding: 4px 8px; }"
        // Editor
        "QTextEdit { background: #111214; border: 1px solid #2b2d31; selection-background-color: #3a4a57; selection-color: #ffffff; }"
        // Left icon panel buttons
        "QToolButton { background: transparent; color: #cfd3da; border: none; padding: 6px; border-radius: 8px; }"
        "QToolButton:hover { background: #2a2c31; }"
        "QToolButton:checked { background: #3a3d44; color: #ffffff; }"
        // Scrollbars
        "QScrollBar:vertical { background: #232428; width: 10px; margin: 0px; }"
        "QScrollBar::handle:vertical { background: #3a3d44; min-height: 20px; border-radius: 5px; }"
        "QScrollBar:horizontal { background: #232428; height: 10px; margin: 0px; }"
        "QScrollBar::handle:horizontal { background: #3a3d44; min-width: 20px; border-radius: 5px; }"
        "QScrollBar::add-line, QScrollBar::sub-line { background: transparent; height: 0; width: 0; }"
    );

    auto *window = new MainWindow();
    window->setWindowDimensions(1200, 800);
    window->show();

    return app.exec();
}
