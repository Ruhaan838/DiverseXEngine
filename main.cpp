
#include <QApplication>
#include "ui/mainwindow/mainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    auto *window = new MainWindow();
    window->setWindowDimensions(1200, 800);
    window->show();

    return app.exec();
}
