#-------------------------------------------------
#
# Project created by QtCreator 2015-07-31T00:59:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = engine
TEMPLATE = app
QMAKE_CXX = mpic++
QMAKE_LINK = mpic++
CONFIG += c++11 \
        warn_off

INCLUDEPATH += \
    /usr/include/mpi/ \
    ../../include/


SOURCES += main.cpp\
    plan.cpp \
    client.cpp \
    communicator.cpp \
    datastore.cpp \
    index.cpp \
    local_node.cpp \
    logger.cpp \
    master_executer.cpp \
    node.cpp \
    predicate_stat.cpp \
    query_count.cpp \
    query_index_node.cpp \
    query_index.cpp \
    query.cpp \
    replica_index_node.cpp \
    statisticsManager.cpp \
    mastergui.cpp \
    walk_edge.cpp \
    SPARQLParser.cpp \
    SPARQLLexer.cpp \
    comparisontab.cpp

HEADERS  += ../../include/engine/mastergui.h \
    ../../include/engine/client.h \
    ../../include/engine/communicator.h \
    ../../include/engine/datastore.h \
    ../../include/engine/index.h \
    ../../include/engine/local_node.h \
    ../../include/engine/logger.h \
    ../../include/engine/master_executer.h \
    ../../include/engine/node.h \
    ../../include/engine/plan.h \
    ../../include/engine/predicate_stat.h \
    ../../include/engine/query_count.h \
    ../../include/engine/query_index_node.h \
    ../../include/engine/query_index.h \
    ../../include/engine/query.h \
    ../../include/engine/replica_index_node.h \
    ../../include/engine/statisticsManager.h \
    ../../include/engine/walk_edge.h \
    ../../include/engine/SPARQLParser.hpp \
    ../../include/engine/SPARQLLexer.hpp \
    ../../include/engine/comparisontab.h

FORMS    += ../../ui/engine/mastergui.ui \
    ../../ui/engine/comparisontab.ui

DESTDIR = ../../Release

LIBS += -L../../lib/ -lcommon -lboost_iostreams -lboost_system -lboost_filesystem
PRE_TARGETDEPS += ../../lib/libcommon.a

DISTFILES +=
