#-------------------------------------------------
#
# Project created by QtCreator 2014-09-24T14:01:07
#
#-------------------------------------------------

QT       += core gui
QT       += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotoAlbum
TEMPLATE = app


SOURCES += main.cpp\
        photoalbum.cpp\
        crop.cpp \
        photoalbum_extra_functionality.cpp

HEADERS  += photoalbum.h\
            crop.h

CONFIG   += console

FORMS    += photoalbum.ui

QMAKE_CXXFLAGS += -std=c++11
