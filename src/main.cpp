
#include <QApplication>
#include "mainwindow.h"

/**
 * @file main.cpp
 * @author Lukas Bozinov
 * @brief Main method of the program.
 * 
 * Initializes the Qt application, applies a dark mode theme to the UI, ensures the 
 * native title bar is independant of Qt styles and launches the MainWindow instance.
 * Handles user interactions by entering the QT event loop.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return int The success of app.exec().
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Apply dark mode styling for the window’s interior.
    // The native title bar remains unchanged by Qt.
    app.setStyleSheet(
        "QMainWindow { background-color: #2d2d30; }"
        "QWidget { background-color: #2d2d30; color: #ffffff; }"
        "QStatusBar { background-color: #2d2d30; color: #ffffff; }"
        "QLabel { color: #ffffff; }"
        "QPushButton { background-color: #3e3e42; color: #ffffff; border: none; padding: 6px; }"
        "QPushButton:hover { background-color: #505055; }"
    );

    MainWindow window;
    window.show();
    return app.exec();
}