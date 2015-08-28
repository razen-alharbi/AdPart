#-------------------------------------------------
#
# Project created by QtCreator 2015-07-02T16:37:54
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = parallel_partitioner
TEMPLATE = app
QMAKE_CXX = mpic++
QMAKE_LINK = mpic++
CONFIG += c++11 \
        warn_off

INCLUDEPATH += \
    /usr/include/mpi/ \
    ../../include/

SOURCES += main.cpp\
        parallelpartitioner.cpp \
    partitioner_client.cpp \
    partitioner_communicator.cpp

HEADERS  += ../../include/parallel_partitioner/parallelpartitioner.h \
    ../../include/parallel_partitioner/partitioner_client.h \
    ../../include/parallel_partitioner/partitioner_communicator.h

FORMS    += ../../ui/parallel_partitioner/parallelpartitioner.ui

DESTDIR = ../../Release

LIBS += -L../../lib/ -lcommon
PRE_TARGETDEPS += ../../lib/libcommon.a
