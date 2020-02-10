#include "mainwindow.h"
#include <QApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <iostream>

int main(int argc, char* argv[])
{
QString semaphoreName("UMUSemaphore");
QString memoryBlockName("UMUMemory");

#ifdef Q_OS_WIN32
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(".");
    paths.append("platforms");
    QCoreApplication::setLibraryPaths(paths);
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
    QSystemSemaphore semaphore(semaphoreName, 1);
    semaphore.acquire();

#ifndef Q_OS_WIN32
    // в linux разделяемая память не освобождается при аварийном завершении приложения,
    QSharedMemory fixSharedMemory(memoryBlockName);
    if(fixSharedMemory.attach()){
        fixSharedMemory.detach();
    }
#endif

    QSharedMemory sharedMemory(memoryBlockName);
    bool isRunning;
    if (sharedMemory.attach()){

        isRunning = true;
    }else{
        sharedMemory.create(1);
        isRunning = false;
    }
    semaphore.release();

    if(isRunning){
        std::cerr << "umuEmulator main(): there was an attempt to execute the 2-nd process. Aborted"  << std::endl;
        return 1;
    }

    MainWindow w;
#ifdef ANDROID
    w.showFullScreen();
#else
    w.show();
#endif

    return a.exec();
}
