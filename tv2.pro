
CONFIG += qt

DEFINES += QT57

QT += core widgets

HEADERS += TreeMainWindow.h
HEADERS += TreeWidget.h
HEADERS += TextEdit.h

SOURCES += TreeAppli.cpp
SOURCES += TreeMainWindow.cpp
SOURCES += TreeWidget.cpp
SOURCES += TextEdit.cpp

RC_FILE += tv2.rc
win32:QMAKE_RC = rc -D_MSC_VER

TARGET = tv2

