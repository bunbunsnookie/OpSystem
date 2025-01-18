#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if (argc > 1 && QString(argv[1]) == "-kiosk")
    {
        w.setWindowFlag(Qt::WindowStaysOnTopHint);
        w.showFullScreen();
        w.can_close = false; // Ctrl+Shift+Alt+Space -> Alt+F4
    }
    else
    {
        w.show();
    }

    return a.exec();
}
