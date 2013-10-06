#-------------------------------------------------
#
# Project created by QtCreator 2012-12-12T15:04:44
#
#-------------------------------------------------

QT       += core gui phonon webkit
QT       += declarative

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BioVision
TEMPLATE = app

win32:INCLUDEPATH += C:\opencv\build\include
macx:INCLUDEPATH += /usr/local/include

SOURCES += main.cpp \
    AboutWindow.cpp \
    Analyzer.cpp \
    BvSystem.cpp \
    DetailAnalyzer.cpp \
    MainWindow.cpp \
    RegionWindow.cpp \
    Result.cpp \
    ThreadManager.cpp \
    Video.cpp \
    VideoCopier.cpp \
    WindowManager.cpp \
    Project.cpp \
    OpenCV.cpp \
    ProjectManager.cpp \
    BvThreadWorker.cpp \
    BvRegion.cpp \
    OptionsWindow.cpp \
    AnalyzeCheckDialog.cpp \
    EnlargedFrameWindow.cpp

HEADERS  += \
    AboutWindow.h \
    Analyzer.h \
    BvSystem.h \
    DetailAnalyzer.h \
    EnlargedFrameWindow.h \
    MainWindow.h \
    RegionWindow.h \
    Result.h \
    ThreadManager.h \
    Video.h \
    VideoCopier.h \
    WindowManager.h \
    Project.h \
    OpenCV.h \
    ProjectManager.h \
    MainWindow.h \
    AboutWindow.h \
    BvThreadWorker.h \
    BvRegion.h \
    OptionsWindow.h \
    AnalyzeCheckDialog.h \
    EnlargedFrameWindow.h

FORMS    += \
    RegionWindow.ui \
    AboutWindow.ui \
    OptionsWindow.ui \
    AnalyzeCheckDialog.ui \
    EnlargedFrameWindow.ui \
    MainWindow_win.ui \
    MainWindow_mac.ui

RC_FILE = myapp.rc
ICON = BioVision.icns

win32:LIBS += C:\opencv\build\x86\vc10\lib\*.lib
macx:LIBS += /usr/local/lib/*.dylib

RESOURCES += \
    BvResources.qrc



