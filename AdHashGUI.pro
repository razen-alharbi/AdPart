#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T09:23:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = subdirs
CONFIG += c++11
QMAKE_CXX = mpic++
QMAKE_LINK = mpic++

SUBDIRS += src/common \
    src/mgmt \
    src/parallel_partitioner \
    src/engine

src/mgmt.depends = src/common
src/parallel_partitioner.depends = src/common
src/engine.depends = src/common
