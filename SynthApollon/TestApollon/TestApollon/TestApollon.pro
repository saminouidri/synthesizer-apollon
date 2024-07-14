QT += testlib
QT += core gui
QT += widgets
QT += multimedia


CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_testapplication.cpp \
    ../../sound.cpp

HEADERS += \
    ../../sound.h
