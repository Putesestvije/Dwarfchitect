#-------------------------------------------------
#
# Project created by QtCreator 2017-04-18T00:21:57
#
#-------------------------------------------------

QT       += core gui\

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dwarfchitect
TEMPLATE = app

QMAKE_LFLAGS =
QMAKE_LFLAGS += -Wl,-O1 -Wl,-rpath,lib -Wl,-verbose

QMAKE_LIBS =
QMAKE_LIBS += -Llib -lQt5Designer -L/usr/lib64 -lQt5Widgets -lQt5Gui -lQt5Xml -lQt5Core -lGL -lpthread


#CONFIG += -static
#QMAKE_LFLAGS += -static

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp\
        tileface.cpp \
    makenewdialog.cpp \
    picker.cpp \
    floor.cpp \
    site.cpp \
    macroalgorithm.cpp \
    maximalrectangle.cpp \
    graphicsview.cpp \
    macrotypedialog.cpp


HEADERS  += mainwindow.h\
            tileface.h \
    makenewdialog.h \
    picker.h \
    coords.h \
    floor.h \
    tileback.h \
    site.h \
    macroalgorithm.h \
    maximalrectangle.h \
    designations.h \
    graphicsview.h \
    macrotypedialog.h

FORMS    += mainwindow.ui \
    newprojectdialog_1.ui \
    macrotypedialog.ui

RESOURCES += \
    otherforms.qrc \
    fonts.qrc \
    images.qrc

DISTFILES +=
