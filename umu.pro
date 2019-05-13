#-------------------------------------------------
#
# Project created by QtCreator 2019-04-02T19:58:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = umu
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    EMULATOR/ascanpulse.cpp \
    EMULATOR/pldemu.cpp \
    EMULATOR/umudevice.cpp \
    EMULATOR/unitlin.cpp \
    DETECTOR/emulator.cpp \
    DETECTOR/objectlib.cpp \
    DETECTOR/pathmodel.cpp \
    DETECTOR/scanobject.cpp \
    DETECTOR/signalsdata.cpp \
    DETECTOR/test.cpp \
    DETECTOR/trolley.cpp \
    DETECTOR/trolleylin.cpp \
    UMU3204/MAIN/MISC133.C \
    UMU3204/MAIN/MISC46.C \
    UMU3204/US/_umuvar32.cpp \
    UMU3204/US/_us320444.cpp \
    UMU3204/US/_us4619.cpp \
    UMU3204/US/_us_32048.cpp \
    defcoreM/datatransfers/datatransfer_lan.cpp \
    defcoreM/datatransfers/idatatransfer.cpp \
    defcoreM/sockets/isocket.cpp \
    defcoreM/sockets/socket_lan.cpp \
    defcoreM/CriticalSection_Lin.cpp \
    defcoreM/ThreadClassList_Lin.cpp \
    defcoreM/LinThread.cpp \
    defcoreM/platforms.cpp \
    defcoreM/TickCount.cpp \
    DETECTOR/objectstor.cpp \
    DETECTOR/sobfile.cpp \
    DETECTOR/sobfmaker.cpp \
    DETECTOR/objectsarray.cpp \
    config.cpp \
    defcoreM/datacontainer/dc_functions.cpp



HEADERS  += mainwindow.h \
    EMULATOR/ascanpulse.h \
    EMULATOR/pldemu.h \
    EMULATOR/umudevice.h \
    EMULATOR/unitlin.h \
    DETECTOR/emulator.h \
    DETECTOR/objectlib.h \
    DETECTOR/pathmodel.h \
    DETECTOR/scanobject.h \
    DETECTOR/signalsdata.h \
    DETECTOR/test.h \
    DETECTOR/trolley.h \
    DETECTOR/trolleylin.h \
    UMU3204/ETHER/IPOPT43.H \
    UMU3204/MAIN/MISC133.H \
    UMU3204/MAIN/MISC46_2.H \
    UMU3204/US/us465d.h \
    defcoreM/datatransfers/datatransfer_lan.h \
    defcoreM/datatransfers/idatatransfer.h \
    defcoreM/sockets/isocket.h \
    defcoreM/sockets/socket_lan.h \
    defcoreM/CriticalSection_Lin.h \
    defcoreM/ThreadClassList_Lin.h \
    defcoreM/LinThread.h \
    defcoreM/platforms.h \
    defcoreM/TickCount.h \
    DETECTOR/objectstor.h \
    DETECTOR/sobfile.h \
    DETECTOR/sobfmaker.h \
    DETECTOR/objectsarray.h \
    config.h \
    defcoreM/datacontainer/dc_functions.h \
    defcoreM/datacontainer/dc_definitions.h


INCLUDEPATH += UMU3204/MAIN \
               UMU3204/US \
               EMULATOR \
               DETECTOR \
               UMU3204/ETHER \
               defcoreM/datatransfers \
               defcoreM/sockets \
               defcoreM \
               defcoreM/datacontainer




FORMS    += mainwindow.ui

DEFINES += DEVICE_EMULATION

