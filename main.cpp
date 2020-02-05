#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
#ifdef DEFCORE_OS_WIN
//    QStringList paths = QCoreApplication::libraryPaths();
//    paths.append(".");
//    paths.append("platforms");
//    QCoreApplication::setLibraryPaths(paths);
#endif

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

    return a.exec();
}
