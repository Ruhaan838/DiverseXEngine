
#include <QApplication>
#include "mainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    auto window = MainWindow();

    return app.exec();
}
