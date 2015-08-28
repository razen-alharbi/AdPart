#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T09:29:04
#
#-------------------------------------------------

QT       -= core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = common
TEMPLATE = lib
QMAKE_CXX = mpic++
QMAKE_LINK = mpic++
CONFIG += staticlib \
        c++11 \
        warn_off
INCLUDEPATH += \
    /usr/include/mpi/ \
    ../../include/

SOURCES += \
    utils.cpp \
    profiler.cpp

HEADERS += \
    ../../include/common/utils.h \
    ../../include/common/constants.h \
    ../../include/common/profiler.h \
    ../../include/common/xpm_images.h \
    ../../include/common/gnuplot-iostream.h

DESTDIR = ../../lib

