#include "mainwindow.h"
#include <QApplication>
//#include "devlin.h"

int main(int argc, char *argv[])
{
// DEVLin device;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

//    QApplication::connect(&a, SIGNAL(aboutToQuit()), &device, SLOT(onFinishing()));
//    device.start();

    return a.exec();
}

