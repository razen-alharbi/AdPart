#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T09:53:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mgmt
TEMPLATE = app
QMAKE_CXX = mpic++
QMAKE_LINK = mpic++
CONFIG += c++11 \
        warn_off

INCLUDEPATH += \
    /usr/include/mpi/ \
    ../../include/

SOURCES +=  adhashmgmt.cpp \
    main.cpp\
    browsedialog.cpp \
    clusterdialog.cpp \
    clustermanager.cpp \
    database.cpp \
    dataencodingdialog.cpp \
    loaddatadialog.cpp \
    testconnectivity.cpp \
    dbdeletedialog.cpp \
    partitioningdialog.cpp \
    cluster_configuration.cpp \
    confdeldialog.cpp \
    contentdeldialog.cpp \
    TurtleParser.cpp

HEADERS  += \
    ../../include/mgmt/adhashmgmt.h \
    ../../include/mgmt/browsedialog.h \
    ../../include/mgmt/clusterdialog.h \
    ../../include/mgmt/clustermanager.h \
    ../../include/mgmt/database.h \
    ../../include/mgmt/dataencodingdialog.h \
    ../../include/mgmt/loaddatadialog.h \
    ../../include/mgmt/testconnectivity.h \
    ../../include/mgmt/dbdeletedialog.h \
    ../../include/mgmt/partitioningdialog.h \
    ../../include/mgmt/cluster_configuration.h \
    ../../include/mgmt/confdeldialog.h \
    ../../include/mgmt/contentdeldialog.h \
    ../../include/mgmt/Type.hpp \
    ../../include/mgmt/TurtleParser.hpp

FORMS    += ../../ui/mgmt/adhashmgmt.ui \
    ../../ui/mgmt/testconnectivity.ui \
    ../../ui/mgmt/loaddatadialog.ui \
    ../../ui/mgmt/dataencodingdialog.ui \
    ../../ui/mgmt/clustermanager.ui \
    ../../ui/mgmt/clusterdialog.ui \
    ../../ui/mgmt/browsedialog.ui \
    ../../ui/mgmt/dbdeletedialog.ui \
    ../../ui/mgmt/partitioningdialog.ui \
    ../../ui/mgmt/confdeldialog.ui \
    ../../ui/mgmt/contentdeldialog.ui

DESTDIR = ../../Release

LIBS += -L../../lib/ -lcommon
PRE_TARGETDEPS += ../../lib/libcommon.a
