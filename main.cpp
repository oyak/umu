#include "mainwindow.h"
#include <QApplication>
//#include "devlin.h"

int main(int argc, char* argv[])
{
    // DEVLin device;
#ifdef ANDROID
    QApplication::setStyle("fusion");
    QApplication::setEffectEnabled(Qt::UI_AnimateMenu, false);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip, false);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);
    QApplication::setEffectEnabled(Qt::UI_AnimateToolBox, false);
#endif

    QApplication a(argc, argv);
    MainWindow w;
#ifdef ANDROID
    w.showFullScreen();
#else
    w.show();
#endif

    //    QApplication::connect(&a, SIGNAL(aboutToQuit()), &device, SLOT(onFinishing()));
    //    device.start();

    return a.exec();
}
